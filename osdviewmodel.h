#ifndef OSDVIEWMODEL_H
#define OSDVIEWMODEL_H

#include <QObject>
#include <QColor>
#include <QRectF>
#include <QPointF>
#include <QString>

// --- OSD Data Enums (from your existing code) ---
// We register these enums so QML can understand them.
class OsdEnums : public QObject
{
    Q_OBJECT         // Required for QML meta-object system

public: // Enums should be public if you want to access them from outside the class.

    enum OperationalMode {
        Idle, Surveillance, Tracking, Engagement, EmergencyStop, Unknown
    };
    Q_ENUM(OperationalMode) // <-- Changed from Q_ENUM_NS

    enum ReticleType {
        Basic, BoxCrosshair, StandardCrosshair, PrecisionCrosshair, MilDot, NoReticle
    };
    Q_ENUM(ReticleType) // <-- Changed from Q_ENUM_NS
};


/**
 * @brief OsdViewModel exposes OSD data to QML using Q_PROPERTY.
 *
 * This class replaces the data and calculation logic of OsdRenderer. The QML frontend
 * binds to these properties to render the UI elements.
 */
class OsdViewModel : public QObject
{
    Q_OBJECT

    // --- Data Properties ---
    // Operational Mode Text: Used in the top-left text display.
    Q_PROPERTY(QString modeText READ modeText NOTIFY modeTextChanged)

    // Azimuth Value for the Azimuth Indicator Needle and Text.
    Q_PROPERTY(float azimuth READ azimuth NOTIFY azimuthChanged)

    // Tracking Box Data (position and size).
    Q_PROPERTY(QRectF trackingBox READ trackingBox NOTIFY trackingBoxChanged)

    // Reticle Type (Used to select which reticle to display in QML).
    Q_PROPERTY(OsdEnums::ReticleType reticleType READ reticleType NOTIFY reticleTypeChanged)

    // Reticle Position Offset (Calculated based on zeroing and lead angles, in pixels).
    Q_PROPERTY(QPointF reticleOffsetPx READ reticleOffsetPx NOTIFY reticleOffsetPxChanged)

    // OSD Color (Used to change the color style of all QML elements).
    Q_PROPERTY(QColor osdColor READ osdColor WRITE setOsdColor NOTIFY osdColorChanged)

public:
    explicit OsdViewModel(int screenWidth, int screenHeight, QObject *parent = nullptr);

    // --- Public Getters (read-only for properties) ---
    QString modeText() const { return m_modeText; }
    float azimuth() const { return m_azimuth; }
    QRectF trackingBox() const { return m_trackingBox; }
    OsdEnums::ReticleType reticleType() const { return m_reticleType; }
    QPointF reticleOffsetPx() const { return m_reticleOffsetPx; }
    QColor osdColor() const { return m_osdColor; }

    // --- Public Setters (called from C++ logic or simulated data) ---
    void setOsdColor(const QColor &osdColor);
    int screenWidth() const { return m_screenWidth; }
    int screenHeight() const { return m_screenHeight; }

public slots:
    // --- Update Slots from existing OsdRenderer (now update C++ properties) ---
    // We keep these names for easier migration.
    void updateMode(OsdEnums::OperationalMode mode);
    void updateAzimuth(float azimuth);
    void updateTrackingBox(float x, float y, float width, float height);
    void updateReticleType(OsdEnums::ReticleType type);

    /**
     * @brief Calculates reticle pixel offset based on angular inputs.
     * @param offsetAzDegrees Azimuth offset from gun to camera boresight.
     * @param offsetElDegrees Elevation offset from gun to camera boresight.
     * @param fov current horizontal field of view in degrees.
     */
    void updateReticleOffset(float offsetAzDegrees, float offsetElDegrees, float fov);

signals:
    // Signals to notify QML that a property has changed.
    void modeTextChanged();
    void azimuthChanged();
    void trackingBoxChanged();
    void reticleTypeChanged();
    void reticleOffsetPxChanged();
    void osdColorChanged();

private:
    // --- Internal State Variables ---
    QString m_modeText = "MODE: IDLE";
    float m_azimuth = 0.0f;
    QRectF m_trackingBox = QRectF(0, 0, 0, 0);
    OsdEnums::ReticleType m_reticleType = OsdEnums::ReticleType::StandardCrosshair;
    QPointF m_reticleOffsetPx = QPointF(0, 0);
    QColor m_osdColor = QColor(70, 226, 165); // Default green color

    // --- Constants from OsdRenderer for calculations ---
    int m_screenWidth;
    int m_screenHeight;
    float m_currentFov = 63.7f; // Store for offset calculations
    float m_reticleScaleFactor = 1.0f; // For MilDot scaling (if implemented)
};

#endif // OSDVIEWMODEL_H
