#include "osdcontroller.h"
#include "models/osdviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "hardware/devices/cameravideostreamdevice.h"
#include <QDebug>

OsdController::OsdController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_stateModel(nullptr)
{
}

void OsdController::setViewModel(OsdViewModel* viewModel)
{
    m_viewModel = viewModel;
    qDebug() << "OsdController: ViewModel set:" << m_viewModel;
}

void OsdController::setStateModel(SystemStateModel* stateModel)
{
    m_stateModel = stateModel;
    qDebug() << "OsdController: StateModel set:" << m_stateModel;
}

void OsdController::initialize()
{
    qDebug() << "OsdController::initialize()";

    if (!m_viewModel) {
        qCritical() << "OsdController: ViewModel is null! Call setViewModel() first.";
        return;
    }

    if (!m_stateModel) {
        qCritical() << "OsdController: StateModel is null! Call setStateModel() first.";
        return;
    }

    // ⭐ Initialize active camera from state
    const auto& initialData = m_stateModel->data();
    m_activeCameraIndex = initialData.activeCameraIsDay ? 0 : 1;

    // Connect to state changes to track camera switching
    connect(m_stateModel, &SystemStateModel::dataChanged,
            this, &OsdController::onSystemStateChanged);

    // Connect to color changes
    connect(m_stateModel, &SystemStateModel::colorStyleChanged,
            this, &OsdController::onColorStyleChanged);

    // Set initial state
    m_viewModel->setAccentColor(initialData.colorStyle);

    qDebug() << "OsdController initialized successfully";

    // =========================================================================
    // PHASE 2: Connect to CameraVideoStreamDevice (Add LATER)
    // =========================================================================
    // When you want frame-synchronized OSD updates, uncomment this:
    /*
    CameraVideoStreamDevice* dayCamera = ...; // Get from SystemController
    CameraVideoStreamDevice* nightCamera = ...;

    if (dayCamera) {
        connect(dayCamera, &CameraVideoStreamDevice::frameDataReady,
                this, &OsdController::onFrameDataReady);
    }
    if (nightCamera) {
        connect(nightCamera, &CameraVideoStreamDevice::frameDataReady,
                this, &OsdController::onFrameDataReady);
    }
    */
}


void OsdController::onSystemStateChanged(const SystemStateData& data)
{
    // Update active camera index when it changes
    int newActiveCameraIndex = data.activeCameraIsDay ? 0 : 1;

    if (m_activeCameraIndex != newActiveCameraIndex) {
        m_activeCameraIndex = newActiveCameraIndex;
        qDebug() << "OsdController: Active camera switched to"
                 << (m_activeCameraIndex == 0 ? "DAY" : "THERMAL");
    }
}


void OsdController::onFrameDataReady(const FrameData& data)
{
    if (!m_viewModel) return;

    // ⭐ CRITICAL FIX: Only process frames from the ACTIVE camera!
    if (data.cameraIndex != m_activeCameraIndex) {
        // Ignore frames from inactive camera
        return;
    }

    // === BASIC OSD DATA ===
    m_viewModel->updateMode(data.currentOpMode);
    m_viewModel->updateMotionMode(data.motionMode);
    m_viewModel->updateStabilization(data.stabEnabled);
    m_viewModel->updateAzimuth(data.azimuth);
    m_viewModel->updateElevation(data.elevation);
    m_viewModel->updateImuData(
        data.imuConnected,
        data.imuYawDeg,      // Vehicle heading
        data.imuPitchDeg,
        data.imuRollDeg,
        data.imuTemp
    );
    m_viewModel->updateSpeed(data.speed);
    m_viewModel->updateFov(data.cameraFOV);

    // Camera type
    QString cameraType = (data.cameraIndex == 0) ? "DAY" : "THERMAL";
    m_viewModel->updateCameraType(cameraType);

    // === SYSTEM STATUS ===
    m_viewModel->updateSystemStatus(data.sysCharged, data.sysArmed, data.sysReady);
    m_viewModel->updateFiringMode(data.fireMode);
    m_viewModel->updateLrfDistance(data.lrfDistance);

    // ========================================================================
    // === RETICLE   ===
    // ========================================================================
    m_viewModel->updateReticleType(data.reticleType);
    // ⭐ CRITICAL: Verify that pixel position is correct based on LAC status!
    // SystemStateModel SHOULD have already calculated correct position,
    // but let's verify the logic here as a safety check.

    float finalReticleX = data.reticleAimpointImageX_px;
    float finalReticleY = data.reticleAimpointImageY_px;

    // ⭐ SAFETY CHECK: If LAC is active but status is ZoomOut,
    // the reticle should be at zero offset (just zeroing applied)
    // This should already be handled by SystemStateModel, but verify:
    if (data.leadAngleActive && data.leadAngleStatus == LeadAngleStatus::ZoomOut) {
        qWarning() << "OsdController: LAC active but ZoomOut status!"
                   << "Reticle offsets should not include lead."
                   << "Current position: X=" << finalReticleX << "Y=" << finalReticleY;
        // The position SHOULD already be correct (without lead offsets)
        // because SystemStateModel should have handled this.
    }

    // ⭐ DEBUG: Log reticle position when LAC is active
    if (data.leadAngleActive) {
        qDebug() << "OsdController: LAC active"
                 << "Status =" << static_cast<int>(data.leadAngleStatus)
                 << "ReticlePos: X=" << finalReticleX << "Y=" << finalReticleY;
    }

    m_viewModel->updateReticleOffset(finalReticleX, finalReticleY);

    // ========================================================================
    // === LAC VISUAL INDICATORS (for CCIP display elements) ===
    // ========================================================================
    // These are for visual feedback, not for reticle positioning

    // Determine if LAC is "effectively active" (On or Lag, not ZoomOut)
    bool lacEffectivelyActive = data.leadAngleActive &&
                                (data.leadAngleStatus == LeadAngleStatus::On ||
                                 data.leadAngleStatus == LeadAngleStatus::Lag);

    m_viewModel->updateLacActive(lacEffectivelyActive);
    m_viewModel->updateRangeMeters(data.lrfDistance);

    // Confidence level based on status
    float confidence = 1.0f;
    if (data.leadAngleActive) {
        switch (data.leadAngleStatus) {
        case LeadAngleStatus::On:
            confidence = 1.0f;
            break;
        case LeadAngleStatus::Lag:
            confidence = 0.5f;  // Reduced confidence
            break;
        case LeadAngleStatus::ZoomOut:
            confidence = 0.0f;  // No confidence - can't calculate lead
            break;
        default:
            confidence = 0.0f;
            break;
        }
    }
    m_viewModel->updateConfidenceLevel(confidence);

    // === TRACKING BOX ===
    m_viewModel->updateTrackingBox(
        data.trackingBbox.x(), data.trackingBbox.y(),
        data.trackingBbox.width(), data.trackingBbox.height()
        );
    m_viewModel->updateTrackingState(data.trackingState);

    // === TRACKING PHASE ===
    m_viewModel->updateTrackingPhase(
        data.currentTrackingPhase,
        data.trackerHasValidTarget,
        QRectF(data.acquisitionBoxX_px, data.acquisitionBoxY_px,
               data.acquisitionBoxW_px, data.acquisitionBoxH_px)
        );

    // === ZEROING ===
    m_viewModel->updateZeroingDisplay(
        data.zeroingModeActive,
        data.zeroingAppliedToBallistics,
        data.zeroingAzimuthOffset,
        data.zeroingElevationOffset
        );

    // === WINDAGE ===
    m_viewModel->updateWindageDisplay(
        data.windageModeActive,
        data.windageAppliedToBallistics,
        data.windageSpeedKnots
        );

    // === ZONE WARNINGS ===
    m_viewModel->updateZoneWarning(
        data.isReticleInNoFireZone,
        data.gimbalStoppedAtNTZLimit
        );

    // === LEAD ANGLE STATUS TEXT ===
    m_viewModel->updateLeadAngleDisplay(data.leadStatusText);

    // === SCAN NAME ===
    m_viewModel->updateCurrentScanName(data.currentScanName);
}
// ============================================================================
// SHARED UPDATE LOGIC
// ============================================================================
/*void OsdController::updateViewModelFromSystemState(const SystemStateData& data)
{
    if (!m_viewModel) return;

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

    // Tracking (simplified - full tracking box will come from FrameData later)
    if (data.trackingActive) {
        // For now, we don't have full tracking bbox from SystemStateModel
        // This will be properly updated when using FrameData
        m_viewModel->updateTrackingBox(0, 0, 0, 0);
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
}*/

void OsdController::onColorStyleChanged(const QColor& color)
{
    qDebug() << "OsdController: Color changed to" << color;
    if (m_viewModel) {
        m_viewModel->setAccentColor(color);
    }
}
