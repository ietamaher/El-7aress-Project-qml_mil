#ifndef OSDVIEWMODEL_H
#define OSDVIEWMODEL_H

#include <QObject>
#include <QColor>
#include <QRectF>
#include <QString>
#include "models/domain/systemstatedata.h" // For enums

class OsdViewModel : public QObject
{
    Q_OBJECT

    // ========================================================================
    // CORE DISPLAY PROPERTIES
    // ========================================================================
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY accentColorChanged)
    Q_PROPERTY(QString modeText READ modeText NOTIFY modeTextChanged)
    Q_PROPERTY(QString motionText READ motionText NOTIFY motionTextChanged)
    Q_PROPERTY(QString stabText READ stabText NOTIFY stabTextChanged)
    Q_PROPERTY(QString cameraText READ cameraText NOTIFY cameraTextChanged)
    Q_PROPERTY(QString speedText READ speedText NOTIFY speedTextChanged)

    // ========================================================================
    // GIMBAL POSITION
    // ========================================================================
    Q_PROPERTY(float azimuth READ azimuth NOTIFY azimuthChanged)
    Q_PROPERTY(float elevation READ elevation NOTIFY elevationChanged)

    // ========================================================================
    // SYSTEM STATUS
    // ========================================================================
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString rateText READ rateText NOTIFY rateTextChanged)
    Q_PROPERTY(QString lrfText READ lrfText NOTIFY lrfTextChanged)
    Q_PROPERTY(QString fovText READ fovText NOTIFY fovTextChanged)

    // ========================================================================
    // TRACKING
    // ========================================================================
    Q_PROPERTY(QRectF trackingBox READ trackingBox NOTIFY trackingBoxChanged)
    Q_PROPERTY(bool trackingBoxVisible READ trackingBoxVisible NOTIFY trackingBoxVisibleChanged)
    Q_PROPERTY(QColor trackingBoxColor READ trackingBoxColor NOTIFY trackingBoxColorChanged)
    Q_PROPERTY(bool trackingBoxDashed READ trackingBoxDashed NOTIFY trackingBoxDashedChanged)

    // Acquisition box (for Tracking_Acquisition phase)
    Q_PROPERTY(QRectF acquisitionBox READ acquisitionBox NOTIFY acquisitionBoxChanged)
    Q_PROPERTY(bool acquisitionBoxVisible READ acquisitionBoxVisible NOTIFY acquisitionBoxVisibleChanged)

    // ========================================================================
    // RETICLE
    // ========================================================================
    Q_PROPERTY(int reticleType READ reticleType NOTIFY reticleTypeChanged)
    Q_PROPERTY(float reticleOffsetX READ reticleOffsetX NOTIFY reticleOffsetChanged)
    Q_PROPERTY(float reticleOffsetY READ reticleOffsetY NOTIFY reticleOffsetChanged)
    Q_PROPERTY(float currentFov READ currentFov NOTIFY currentFovChanged)

    // ========================================================================
    // PROCEDURES (Zeroing, Windage)
    // ========================================================================
    Q_PROPERTY(QString zeroingText READ zeroingText NOTIFY zeroingTextChanged)
    Q_PROPERTY(bool zeroingVisible READ zeroingVisible NOTIFY zeroingVisibleChanged)

    Q_PROPERTY(QString windageText READ windageText NOTIFY windageTextChanged)
    Q_PROPERTY(bool windageVisible READ windageVisible NOTIFY windageVisibleChanged)

    // ========================================================================
    // ZONE WARNINGS
    // ========================================================================
    Q_PROPERTY(QString zoneWarningText READ zoneWarningText NOTIFY zoneWarningTextChanged)
    Q_PROPERTY(bool zoneWarningVisible READ zoneWarningVisible NOTIFY zoneWarningVisibleChanged)

    // ========================================================================
    // LEAD ANGLE & SCAN
    // ========================================================================
    Q_PROPERTY(QString leadAngleText READ leadAngleText NOTIFY leadAngleTextChanged)
    Q_PROPERTY(bool leadAngleVisible READ leadAngleVisible NOTIFY leadAngleVisibleChanged)

    Q_PROPERTY(QString scanNameText READ scanNameText NOTIFY scanNameTextChanged)
    Q_PROPERTY(bool scanNameVisible READ scanNameVisible NOTIFY scanNameVisibleChanged)

public:
    explicit OsdViewModel(QObject *parent = nullptr);

    // Getters
    QColor accentColor() const { return m_accentColor; }
    QString modeText() const { return m_modeText; }
    QString motionText() const { return m_motionText; }
    QString stabText() const { return m_stabText; }
    QString cameraText() const { return m_cameraText; }
    QString speedText() const { return m_speedText; }

    float azimuth() const { return m_azimuth; }
    float elevation() const { return m_elevation; }

    QString statusText() const { return m_statusText; }
    QString rateText() const { return m_rateText; }
    QString lrfText() const { return m_lrfText; }
    QString fovText() const { return m_fovText; }

    QRectF trackingBox() const { return m_trackingBox; }
    bool trackingBoxVisible() const { return m_trackingBoxVisible; }
    QColor trackingBoxColor() const { return m_trackingBoxColor; }
    bool trackingBoxDashed() const { return m_trackingBoxDashed; }

    QRectF acquisitionBox() const { return m_acquisitionBox; }
    bool acquisitionBoxVisible() const { return m_acquisitionBoxVisible; }

    int reticleType() const { return static_cast<int>(m_reticleType); }
    float reticleOffsetX() const { return m_reticleOffsetX; }
    float reticleOffsetY() const { return m_reticleOffsetY; }
    float currentFov() const { return m_currentFov; }

    QString zeroingText() const { return m_zeroingText; }
    bool zeroingVisible() const { return m_zeroingVisible; }

    QString windageText() const { return m_windageText; }
    bool windageVisible() const { return m_windageVisible; }

    QString zoneWarningText() const { return m_zoneWarningText; }
    bool zoneWarningVisible() const { return m_zoneWarningVisible; }

    QString leadAngleText() const { return m_leadAngleText; }
    bool leadAngleVisible() const { return m_leadAngleVisible; }

    QString scanNameText() const { return m_scanNameText; }
    bool scanNameVisible() const { return m_scanNameVisible; }

public slots:
    // Setters
    void setAccentColor(const QColor& color);

    // Update methods (called by OsdController)
    void updateMode(OperationalMode mode);
    void updateMotionMode(MotionMode mode);
    void updateStabilization(bool enabled);
    void updateCameraType(const QString& type);
    void updateSpeed(double speed);

    void updateAzimuth(float azimuth);
    void updateElevation(float elevation);

    void updateSystemStatus(bool charged, bool armed, bool ready);
    void updateFiringMode(FireMode mode);
    void updateLrfDistance(float distance);
    void updateFov(float fov);

    void updateTrackingBox(float x, float y, float width, float height);
    void updateTrackingState(VPITrackingState state);
    void updateTrackingPhase(TrackingPhase phase, bool hasValidTarget, const QRectF& acquisitionBox);

    void updateReticleType(ReticleType type);
    void updateReticleOffset(float x_px, float y_px);

    void updateZeroingDisplay(bool modeActive, bool applied, float azOffset, float elOffset);
    void updateWindageDisplay(bool modeActive, bool applied, float speedKnots);

    void updateZoneWarning(bool inNoFireZone, bool inNoTraverseLimit);
    void updateLeadAngleDisplay(const QString& statusText);
    void updateCurrentScanName(const QString& scanName);

signals:
    void accentColorChanged();
    void modeTextChanged();
    void motionTextChanged();
    void stabTextChanged();
    void cameraTextChanged();
    void speedTextChanged();

    void azimuthChanged();
    void elevationChanged();

    void statusTextChanged();
    void rateTextChanged();
    void lrfTextChanged();
    void fovTextChanged();

    void trackingBoxChanged();
    void trackingBoxVisibleChanged();
    void trackingBoxColorChanged();
    void trackingBoxDashedChanged();

    void acquisitionBoxChanged();
    void acquisitionBoxVisibleChanged();

    void reticleTypeChanged();
    void reticleOffsetChanged();
    void currentFovChanged();

    void zeroingTextChanged();
    void zeroingVisibleChanged();

    void windageTextChanged();
    void windageVisibleChanged();

    void zoneWarningTextChanged();
    void zoneWarningVisibleChanged();

    void leadAngleTextChanged();
    void leadAngleVisibleChanged();

    void scanNameTextChanged();
    void scanNameVisibleChanged();

private:
    // Member variables
    QColor m_accentColor;
    QString m_modeText;
    QString m_motionText;
    QString m_stabText;
    QString m_cameraText;
    QString m_speedText;

    float m_azimuth;
    float m_elevation;

    QString m_statusText;
    QString m_rateText;
    QString m_lrfText;
    QString m_fovText;

    QRectF m_trackingBox;
    bool m_trackingBoxVisible;
    QColor m_trackingBoxColor;
    bool m_trackingBoxDashed;

    QRectF m_acquisitionBox;
    bool m_acquisitionBoxVisible;

    ReticleType m_reticleType;
    float m_reticleOffsetX;
    float m_reticleOffsetY;
    float m_currentFov;

    QString m_zeroingText;
    bool m_zeroingVisible;

    QString m_windageText;
    bool m_windageVisible;

    QString m_zoneWarningText;
    bool m_zoneWarningVisible;

    QString m_leadAngleText;
    bool m_leadAngleVisible;

    QString m_scanNameText;
    bool m_scanNameVisible;

    // Internal state for calculations
    bool m_sysCharged;
    bool m_sysArmed;
    bool m_sysReady;
    FireMode m_fireMode;

    // Screen dimensions (for reticle offset calculations)
    int m_screenWidth;
    int m_screenHeight;
};

#endif // OSDVIEWMODEL_H
