#include "osdviewmodel.h"
#include <QDebug>
#include <cmath>
#include <QQmlEngine>

// GStreamer video width/height (adjust to match your pipeline)
const int VIDEO_WIDTH = 1024;
const int VIDEO_HEIGHT = 768;

OsdViewModel::OsdViewModel(int screenWidth, int screenHeight, QObject *parent)
    : QObject(parent), m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    // Fix: This call now correctly uses a class type as template argument.
    qmlRegisterUncreatableType<OsdEnums>("com.yourcompany.osd", 1, 0, "OsdEnums", "OsdEnums namespace for enum access only.");
}

void OsdViewModel::setOsdColor(const QColor &osdColor)
{
    if (m_osdColor != osdColor) {
        m_osdColor = osdColor;
        emit osdColorChanged();
    }
}

void OsdViewModel::updateMode(OsdEnums::OperationalMode mode)
{
    QString newModeText;
    switch (mode) {
    case OsdEnums::Idle: newModeText = "MODE: IDLE"; break;
    case OsdEnums::Tracking: newModeText = "MODE: TRACKING"; break;
    case OsdEnums::Engagement: newModeText = "MODE: ENGAGE"; break;
    default: newModeText = "MODE: UNKNOWN"; break;
    }

    if (m_modeText != newModeText) {
        m_modeText = newModeText;
        emit modeTextChanged();
    }
}

void OsdViewModel::updateAzimuth(float azimuth)
{
    // The C++ code still performs normalization (logic from OsdRenderer::updateAzimuthIndicator)
    while (azimuth < 0.0f) azimuth += 360.0f;
    while (azimuth >= 360.0f) azimuth -= 360.0f;

    if (m_azimuth != azimuth) {
        m_azimuth = azimuth;
        emit azimuthChanged();
    }
}

void OsdViewModel::updateTrackingBox(float x, float y, float width, float height)
{
    QRectF newBox(x, y, width, height);
    if (m_trackingBox != newBox) {
        m_trackingBox = newBox;
        emit trackingBoxChanged();
    }
}

void OsdViewModel::updateReticleType(OsdEnums::ReticleType type)
{
    if (m_reticleType != type) {
        m_reticleType = type;
        emit reticleTypeChanged();
    }
}

void OsdViewModel::updateReticleOffset(float offsetAzDegrees, float offsetElDegrees, float fov)
{
    // --- C++ Calculation Logic (from OsdRenderer::convertAngularToPixelOffset) ---
    m_currentFov = fov; // Update current FOV for scaling logic

    if (m_currentFov <= 0.001f || m_screenWidth <= 0 || m_screenHeight <= 0) {
        if (m_reticleOffsetPx != QPointF(0, 0)) {
            m_reticleOffsetPx = QPointF(0, 0);
            emit reticleOffsetPxChanged();
        }
        return;
    }

    // Pixels per degree for horizontal (Azimuth)
    double pixelsPerDegreeAz = static_cast<double>(m_screenWidth) / m_currentFov;

    // Calculate vertical FOV based on aspect ratio (assuming square pixels)
    double vfov_approx = m_currentFov * (static_cast<double>(m_screenHeight) / static_cast<double>(m_screenWidth));
    double pixelsPerDegreeEl = static_cast<double>(m_screenHeight) / vfov_approx;

    // Calculate pixel offsets for QML.
    // In Qt's QML coordinate system, positive Y is down.
    // If gun needs to aim up (offsetElDegrees > 0), reticle moves down on screen (positive Y).
    qreal xOffsetPx = static_cast<qreal>(-offsetAzDegrees * pixelsPerDegreeAz);
    qreal yOffsetPx = static_cast<qreal>(offsetElDegrees * pixelsPerDegreeEl);

    QPointF newOffset(xOffsetPx, yOffsetPx);
    if (m_reticleOffsetPx != newOffset) {
        m_reticleOffsetPx = newOffset;
        emit reticleOffsetPxChanged();
    }
}
