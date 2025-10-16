#include "osdviewmodel.h"
#include <QDebug>
#include <cmath>

OsdViewModel::OsdViewModel(QObject *parent)
    : QObject(parent)
    , m_accentColor(70, 226, 165) // Default green
    , m_modeText("MODE: IDLE")
    , m_motionText("MOTION: MAN")
    , m_stabText("STAB: OFF")
    , m_cameraText("CAM: DAY")
    , m_speedText("SPD: 0.0%")
    , m_azimuth(0.0f)
    , m_elevation(0.0f)
    , m_statusText("SYS: --- SAF NRD")
    , m_rateText("RATE: SINGLE SHOT")
    , m_lrfText("LRF: --- m")
    , m_fovText("FOV: 45.0°")
    , m_trackingBox(0, 0, 0, 0)
    , m_trackingBoxVisible(false)
    , m_trackingBoxColor(Qt::yellow)
    , m_trackingBoxDashed(false)
    , m_acquisitionBox(0, 0, 0, 0)
    , m_acquisitionBoxVisible(false)
    , m_reticleType(ReticleType::BoxCrosshair)
    , m_reticleOffsetX(0.0f)
    , m_reticleOffsetY(0.0f)
    , m_currentFov(45.0f)
    , m_zeroingText("")
    , m_zeroingVisible(false)
    , m_windageText("")
    , m_windageVisible(false)
    , m_zoneWarningText("")
    , m_zoneWarningVisible(false)
    , m_leadAngleText("")
    , m_leadAngleVisible(false)
    , m_scanNameText("")
    , m_scanNameVisible(false)
    , m_sysCharged(false)
    , m_sysArmed(false)
    , m_sysReady(false)
    , m_fireMode(FireMode::SingleShot)
    , m_screenWidth(1024)
    , m_screenHeight(768)
{
}

void OsdViewModel::setAccentColor(const QColor& color)
{
    if (m_accentColor != color) {
        m_accentColor = color;
        emit accentColorChanged();
    }
}

void OsdViewModel::updateMode(OperationalMode mode)
{
    QString newText;
    switch (mode) {
    case OperationalMode::Idle: newText = "MODE: IDLE"; break;
    case OperationalMode::Surveillance: newText = "MODE: OBS"; break;
    case OperationalMode::Tracking: newText = "MODE: TRACKING"; break;
    case OperationalMode::Engagement: newText = "MODE: ENGAGE"; break;
    case OperationalMode::EmergencyStop: newText = "MODE: EMERGENCY STOP"; break;
    default: newText = "MODE: UNKNOWN"; break;
    }

    if (m_modeText != newText) {
        m_modeText = newText;
        emit modeTextChanged();
    }
}

void OsdViewModel::updateMotionMode(MotionMode mode)
{
    QString newText;
    switch (mode) {
    case MotionMode::Manual: newText = "MOTION: MAN"; break;
    case MotionMode::AutoSectorScan: newText = "MOTION: SCAN"; break;
    case MotionMode::TRPScan: newText = "MOTION: TRP"; break;
    case MotionMode::ManualTrack: newText = "MOTION: TRACK"; break;
    case MotionMode::AutoTrack: newText = "MOTION: AUTO TRACK"; break;
    case MotionMode::RadarSlew: newText = "MOTION: RADAR"; break;
    default: newText = "MOTION: N/A"; break;
    }

    if (m_motionText != newText) {
        m_motionText = newText;
        emit motionTextChanged();
    }
}

void OsdViewModel::updateStabilization(bool enabled)
{
    QString newText = enabled ? "STAB: ON" : "STAB: OFF";
    if (m_stabText != newText) {
        m_stabText = newText;
        emit stabTextChanged();
    }
}

void OsdViewModel::updateCameraType(const QString& type)
{
    QString newText = QString("CAM: %1").arg(type.toUpper());
    if (m_cameraText != newText) {
        m_cameraText = newText;
        emit cameraTextChanged();
    }
}

void OsdViewModel::updateSpeed(double speed)
{
    QString newText = QString("SPD: %1%").arg(speed, 0, 'f', 1);
    if (m_speedText != newText) {
        m_speedText = newText;
        emit speedTextChanged();
    }
}

void OsdViewModel::updateAzimuth(float azimuth)
{
    // Normalize azimuth to 0-360 range
    while (azimuth < 0.0f) azimuth += 360.0f;
    while (azimuth >= 360.0f) azimuth -= 360.0f;

    if (m_azimuth != azimuth) {
        m_azimuth = azimuth;
        emit azimuthChanged();
    }
}

void OsdViewModel::updateElevation(float elevation)
{
    if (m_elevation != elevation) {
        m_elevation = elevation;
        emit elevationChanged();
    }
}

void OsdViewModel::updateSystemStatus(bool charged, bool armed, bool ready)
{
    m_sysCharged = charged;
    m_sysArmed = armed;
    m_sysReady = ready;

    QString newStatusText = QString("SYS: %1 %2 %3")
                                .arg(charged ? "CHG" : "---")
                                .arg(armed ? "ARM" : "SAF")
                                .arg(ready ? "RDY" : "NRD");

    if (m_statusText != newStatusText) {
        m_statusText = newStatusText;
        emit statusTextChanged();
    }
}

void OsdViewModel::updateFiringMode(FireMode mode)
{
    m_fireMode = mode;

    QString newRateText;
    switch (mode) {
    case FireMode::SingleShot: newRateText = "RATE: SINGLE SHOT"; break;
    case FireMode::ShortBurst: newRateText = "RATE: SHORT BURST"; break;
    case FireMode::LongBurst: newRateText = "RATE: LONG BURST"; break;
    default: newRateText = "RATE: UNKNOWN"; break;
    }

    if (m_rateText != newRateText) {
        m_rateText = newRateText;
        emit rateTextChanged();
    }
}

void OsdViewModel::updateLrfDistance(float distance)
{
    QString newText = (distance > 0.1f)
    ? QString::number(distance, 'f', 1) + " m"
    : "LRF: --- m";

    if (m_lrfText != newText) {
        m_lrfText = newText;
        emit lrfTextChanged();
    }
}

void OsdViewModel::updateFov(float fov)
{
    m_currentFov = fov;

    QString newText = QString("FOV: %1°").arg(fov, 0, 'f', 1);
    if (m_fovText != newText) {
        m_fovText = newText;
        emit fovTextChanged();
        emit currentFovChanged();
    }
}

// ============================================================================
// TRACKING UPDATES
// ============================================================================

void OsdViewModel::updateTrackingBox(float x, float y, float width, float height)
{
    QRectF newBox(x, y, width, height);
    bool newVisible = (width > 0 && height > 0);

    if (m_trackingBox != newBox) {
        m_trackingBox = newBox;
        emit trackingBoxChanged();
    }

    if (m_trackingBoxVisible != newVisible) {
        m_trackingBoxVisible = newVisible;
        emit trackingBoxVisibleChanged();
    }
}

void OsdViewModel::updateTrackingState(VPITrackingState state)
{
    QColor newColor;
    bool newDashed = false;

    switch (state) {
    case VPI_TRACKING_STATE_TRACKED:
        newColor = QColor(0, 255, 0); // Green - tracked
        newDashed = true;
        break;
    case VPI_TRACKING_STATE_LOST:
        newColor = QColor(255, 255, 0); // Yellow - lost
        newDashed = true;
        break;
    default:
        newColor = QColor(255, 255, 0); // Yellow - default
        newDashed = false;
        break;
    }

    if (m_trackingBoxColor != newColor) {
        m_trackingBoxColor = newColor;
        emit trackingBoxColorChanged();
    }

    if (m_trackingBoxDashed != newDashed) {
        m_trackingBoxDashed = newDashed;
        emit trackingBoxDashedChanged();
    }
}

void OsdViewModel::updateTrackingPhase(TrackingPhase phase, bool hasValidTarget, const QRectF& acquisitionBox)
{
    bool showAcquisition = false;
    bool showTracking = false;
    QColor boxColor = Qt::yellow;
    bool boxDashed = false;

    switch (phase) {
    case TrackingPhase::Acquisition:
        showAcquisition = true;
        showTracking = false;
        break;

    case TrackingPhase::Tracking_LockPending:
        showAcquisition = false;
        showTracking = true;
        boxColor = QColor(255, 255, 0); // Yellow - acquiring
        boxDashed = false;
        break;

    case TrackingPhase::Tracking_ActiveLock:
        showAcquisition = false;
        showTracking = hasValidTarget;
        boxColor = QColor(255, 0, 0); // Red - locked
        boxDashed = true;
        break;

    case TrackingPhase::Tracking_Coast:
        showAcquisition = false;
        showTracking = hasValidTarget;
        boxColor = QColor(255, 255, 0); // Yellow - coasting
        boxDashed = true;
        break;

    case TrackingPhase::Tracking_Firing:
        showAcquisition = false;
        showTracking = hasValidTarget;
        boxColor = QColor(0, 255, 0); // Green - firing
        boxDashed = true;
        break;

    case TrackingPhase::Off:
    default:
        showAcquisition = false;
        showTracking = false;
        break;
    }

    // Update acquisition box
    if (m_acquisitionBox != acquisitionBox) {
        m_acquisitionBox = acquisitionBox;
        emit acquisitionBoxChanged();
    }

    if (m_acquisitionBoxVisible != showAcquisition) {
        m_acquisitionBoxVisible = showAcquisition;
        emit acquisitionBoxVisibleChanged();
    }

    // Update tracking box visibility based on phase
    if (m_trackingBoxVisible != showTracking) {
        m_trackingBoxVisible = showTracking;
        emit trackingBoxVisibleChanged();
    }

    if (m_trackingBoxColor != boxColor) {
        m_trackingBoxColor = boxColor;
        emit trackingBoxColorChanged();
    }

    if (m_trackingBoxDashed != boxDashed) {
        m_trackingBoxDashed = boxDashed;
        emit trackingBoxDashedChanged();
    }
}

// ============================================================================
// RETICLE UPDATES
// ============================================================================

void OsdViewModel::updateReticleType(ReticleType type)
{
    if (m_reticleType != type) {
        m_reticleType = type;
        emit reticleTypeChanged();
    }
}

void OsdViewModel::updateReticleOffset(float x_px, float y_px)
{
    // Calculate offset from screen center
    float centerX = m_screenWidth / 2.0f;
    float centerY = m_screenHeight / 2.0f;

    float offsetX = x_px - centerX;
    float offsetY = y_px - centerY;

    if (m_reticleOffsetX != offsetX || m_reticleOffsetY != offsetY) {
        m_reticleOffsetX = offsetX;
        m_reticleOffsetY = offsetY;
        emit reticleOffsetChanged();
    }
}

// ============================================================================
// PROCEDURE UPDATES (Zeroing, Windage)
// ============================================================================

void OsdViewModel::updateZeroingDisplay(bool modeActive, bool applied, float azOffset, float elOffset)
{
    QString newText;
    bool newVisible = false;

    if (modeActive) {
        newText = "ZEROING";
        newVisible = true;
    } else if (applied) {
        newText = "Z";
        newVisible = true;
    }

    if (m_zeroingText != newText) {
        m_zeroingText = newText;
        emit zeroingTextChanged();
    }

    if (m_zeroingVisible != newVisible) {
        m_zeroingVisible = newVisible;
        emit zeroingVisibleChanged();
    }
}

void OsdViewModel::updateWindageDisplay(bool modeActive, bool applied, float speedKnots)
{
    QString newText;
    bool newVisible = false;

    if (modeActive) {
        newText = QString("WINDAGE: %1 kt").arg(speedKnots, 0, 'f', 0);
        newVisible = true;
    } else if (applied) {
        newText = QString("W: %1 kt").arg(speedKnots, 0, 'f', 0);
        newVisible = true;
    }

    if (m_windageText != newText) {
        m_windageText = newText;
        emit windageTextChanged();
    }

    if (m_windageVisible != newVisible) {
        m_windageVisible = newVisible;
        emit windageVisibleChanged();
    }
}

// ============================================================================
// ZONE & STATUS UPDATES
// ============================================================================

void OsdViewModel::updateZoneWarning(bool inNoFireZone, bool inNoTraverseLimit)
{
    QString newText;
    bool newVisible = false;

    if (inNoFireZone) {
        newText = "NO FIRE ZONE";
        newVisible = true;
    } else if (inNoTraverseLimit) {
        newText = "NO TRAVERSE LIMIT";
        newVisible = true;
    }

    if (m_zoneWarningText != newText) {
        m_zoneWarningText = newText;
        emit zoneWarningTextChanged();
    }

    if (m_zoneWarningVisible != newVisible) {
        m_zoneWarningVisible = newVisible;
        emit zoneWarningVisibleChanged();
    }
}

void OsdViewModel::updateLeadAngleDisplay(const QString& statusText)
{
    bool newVisible = !statusText.isEmpty();

    if (m_leadAngleText != statusText) {
        m_leadAngleText = statusText;
        emit leadAngleTextChanged();
    }

    if (m_leadAngleVisible != newVisible) {
        m_leadAngleVisible = newVisible;
        emit leadAngleVisibleChanged();
    }
}

void OsdViewModel::updateCurrentScanName(const QString& scanName)
{
    bool newVisible = !scanName.isEmpty();

    if (m_scanNameText != scanName) {
        m_scanNameText = scanName;
        emit scanNameTextChanged();
    }

    if (m_scanNameVisible != newVisible) {
        m_scanNameVisible = newVisible;
        emit scanNameVisibleChanged();
    }
}
