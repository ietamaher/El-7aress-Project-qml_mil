#include "osdcontroller.h"
#include "services/servicemanager.h"
#include <QDebug>

OsdController::OsdController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_stateModel(nullptr)
{
}

void OsdController::initialize()
{
    qDebug() << "OsdController::initialize()";

    // Get services
    m_viewModel = ServiceManager::instance()->get<OsdViewModel>();
    m_stateModel = ServiceManager::instance()->get<SystemStateModel>();

    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_stateModel);

    qDebug() << "  m_viewModel:" << m_viewModel;
    qDebug() << "  m_stateModel:" << m_stateModel;

    // =========================================================================
    // PHASE 1: Connect to SystemStateModel (Active NOW)
    // =========================================================================
    connect(m_stateModel, &SystemStateModel::dataChanged,
            this, &OsdController::onSystemStateChanged);

    // Connect to color changes
    connect(m_stateModel, &SystemStateModel::colorStyleChanged,
            this, &OsdController::onColorStyleChanged);

    // Set initial state
    const auto& initialData = m_stateModel->data();
    updateViewModelFromSystemState(initialData);
    m_viewModel->setAccentColor(initialData.colorStyle);

    qDebug() << "OsdController initialized successfully";

    // =========================================================================
    // PHASE 2: Connect to CameraVideoStreamDevice (Add LATER)
    // =========================================================================
    // When you implement CameraVideoStreamDevice, add this connection:
    /*
    CameraVideoStreamDevice* cameraDevice = ServiceManager::instance()->get<CameraVideoStreamDevice>();
    if (cameraDevice) {
        connect(cameraDevice, &CameraVideoStreamDevice::frameDataReady,
                this, &OsdController::onFrameDataReady);
        qDebug() << "OsdController: Connected to CameraVideoStreamDevice for frame-level updates";
    }
    */
}

// ============================================================================
// PHASE 1 UPDATE PATH: From SystemStateModel
// ============================================================================
void OsdController::onSystemStateChanged(const SystemStateData& data)
{
    // Update ViewModel from SystemStateModel data
    updateViewModelFromSystemState(data);
}

// ============================================================================
// PHASE 2 UPDATE PATH: From FrameData (Uncomment when ready)
// ============================================================================
/*
void OsdController::onFrameDataReady(const FrameData& data)
{
    // When CameraVideoStreamDevice is implemented, this provides frame-synchronized updates

    // Update basic OSD data
    m_viewModel->updateMode(data.currentOpMode);
    m_viewModel->updateMotionMode(data.motionMode);
    m_viewModel->updateStabilization(data.stabEnabled);
    m_viewModel->updateAzimuth(data.azimuth);
    m_viewModel->updateElevation(data.elevation);
    m_viewModel->updateSpeed(data.speed);
    m_viewModel->updateFov(data.cameraFOV);

    // Update system status
    m_viewModel->updateSystemStatus(data.sysCharged, data.sysArmed, data.sysReady);
    m_viewModel->updateFiringMode(data.fireMode);
    m_viewModel->updateLrfDistance(data.lrfDistance);

    // Update tracking
    m_viewModel->updateTrackingBox(
        data.trackingBbox.x(),
        data.trackingBbox.y(),
        data.trackingBbox.width(),
        data.trackingBbox.height()
    );
    m_viewModel->updateTrackingState(data.trackingState);

    // Update reticle
    m_viewModel->updateReticleType(data.reticleType);
    m_viewModel->updateReticleOffset(
        data.reticleAimpointImageX_px,
        data.reticleAimpointImageY_px
    );

    // Update zeroing
    m_viewModel->updateZeroingDisplay(
        data.zeroingModeActive,
        data.zeroingAppliedToBallistics,
        data.zeroingAzimuthOffset,
        data.zeroingElevationOffset
    );

    // Update windage
    m_viewModel->updateWindageDisplay(
        data.windageModeActive,
        data.windageAppliedToBallistics,
        data.windageSpeedKnots
    );

    // Update zones
    m_viewModel->updateZoneWarning(
        data.isReticleInNoFireZone,
        data.gimbalStoppedAtNTZLimit
    );

    // Update lead angle
    m_viewModel->updateLeadAngleDisplay(data.leadStatusText);

    // Update scan name
    m_viewModel->updateCurrentScanName(data.currentScanName);

    // Update tracking phase display
    m_viewModel->updateTrackingPhase(
        data.currentTrackingPhase,
        data.trackerHasValidTarget,
        QRectF(data.acquisitionBoxX_px, data.acquisitionBoxY_px,
               data.acquisitionBoxW_px, data.acquisitionBoxH_px)
    );
}
*/

// ============================================================================
// SHARED UPDATE LOGIC
// ============================================================================
void OsdController::updateViewModelFromSystemState(const SystemStateData& data)
{
    // Basic OSD data
    m_viewModel->updateMode(data.opMode);
    m_viewModel->updateMotionMode(data.motionMode);
    m_viewModel->updateStabilization(data.enableStabilization);
    m_viewModel->updateAzimuth(data.gimbalAz);
    m_viewModel->updateElevation(data.gimbalEl);
    m_viewModel->updateSpeed(data.gimbalSpeed);

    // FOV (select based on active camera)
    float fov = data.activeCameraIsDay ? data.dayCurrentHFOV : data.nightCurrentHFOV;
    m_viewModel->updateFov(fov);

    // System status
    m_viewModel->updateSystemStatus(data.ammoLoaded, data.gunArmed, data.isReady());
    m_viewModel->updateFiringMode(data.fireMode);
    m_viewModel->updateLrfDistance(data.lrfDistance);

    // Camera type
    QString cameraType = data.activeCameraIsDay ? "DAY" : "THERMAL";
    m_viewModel->updateCameraType(cameraType);

    // Tracking (from SystemStateModel tracking data)
    // Note: This is simplified. Full tracking bbox will come from FrameData later
    if (data.trackingActive) {
        // For now, we might not have the full tracking box from SystemStateModel
        // This will be properly updated when CameraVideoStreamDevice provides FrameData
        m_viewModel->updateTrackingBox(0, 0, 0, 0); // Placeholder
    } else {
        m_viewModel->updateTrackingBox(0, 0, 0, 0);
    }

    // Reticle
    m_viewModel->updateReticleType(data.reticleType);
    m_viewModel->updateReticleOffset(
        data.reticleAimpointImageX_px,
        data.reticleAimpointImageY_px
        );

    // Zeroing
    m_viewModel->updateZeroingDisplay(
        data.zeroingModeActive,
        data.zeroingAppliedToBallistics,
        data.zeroingAzimuthOffset,
        data.zeroingElevationOffset
        );

    // Windage
    m_viewModel->updateWindageDisplay(
        data.windageModeActive,
        data.windageAppliedToBallistics,
        data.windageSpeedKnots
        );

    // Zone warnings
    m_viewModel->updateZoneWarning(
        data.isReticleInNoFireZone,
        data.isReticleInNoTraverseZone
        );

    // Lead angle
    m_viewModel->updateLeadAngleDisplay(data.leadStatusText);

    // Scan name
    m_viewModel->updateCurrentScanName(data.currentScanName);

    // Tracking phase
    m_viewModel->updateTrackingPhase(
        data.currentTrackingPhase,
        false, // trackerHasValidTarget - will be updated from FrameData later
        QRectF(data.acquisitionBoxX_px, data.acquisitionBoxY_px,
               data.acquisitionBoxW_px, data.acquisitionBoxH_px)
        );
}

void OsdController::onColorStyleChanged(const QColor& color)
{
    qDebug() << "OsdController: Color changed to" << color;
    if (m_viewModel) {
        m_viewModel->setAccentColor(color);
    }
}
