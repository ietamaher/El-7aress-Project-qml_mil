#ifndef SYSTEMCONTROLLER_H
#define SYSTEMCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QThread>
#include <QQmlApplicationEngine>

// Forward declares (Hardware Devices)
class DayCameraControlDevice;
class CameraVideoStreamDevice;
class ImuDevice;
class JoystickDevice;
class LensDevice;
class LRFDevice;
class NightCameraControlDevice;
class Plc21Device;
class Plc42Device;
class ServoActuatorDevice;
class ServoDriverDevice;

// Forward declares (Data Models)
class DayCameraDataModel;
class GyroDataModel;
class JoystickDataModel;
class LensDataModel;
class LrfDataModel;
class NightCameraDataModel;
class Plc21DataModel;
class Plc42DataModel;
class ServoActuatorDataModel;
class ServoDriverDataModel;

// Forward declares (System Models & Controllers)
class SystemStateModel;
class GimbalController;
class WeaponController;
class CameraController;
class JoystickController;

// *** ADD: QML-related controllers ***
class OsdController;
class ZoneDefinitionController;

// *** ADD: ViewModels ***
class OsdViewModel;
class ZoneDefinitionViewModel;
class ZoneMapViewModel;
class AreaZoneParameterViewModel;
class SectorScanParameterViewModel;
class TRPParameterViewModel;

// *** ADD: Video provider for QML ***
class VideoImageProvider;

class SystemController : public QObject
{
    Q_OBJECT

public:
    explicit SystemController(QObject *parent = nullptr);
    ~SystemController();

    // Phase 1: Initialize hardware, models, controllers
    void initializeHardware();

    // Phase 2: Initialize QML system (ViewModels, UI Controllers, Video Provider)
    void initializeQmlSystem(QQmlApplicationEngine* engine);

    // Phase 3: Start the system (threads, video streams)
    void startSystem();

private:
    // ========================================================================
    // HARDWARE DEVICES (Unchanged)
    // ========================================================================
    DayCameraControlDevice* m_dayCamControl = nullptr;
    CameraVideoStreamDevice* m_dayVideoProcessor = nullptr;  // ✅ Keep this (has tracking)
    ImuDevice* m_gyroDevice = nullptr;
    JoystickDevice* m_joystickDevice = nullptr;
    LensDevice* m_lensDevice = nullptr;
    LRFDevice* m_lrfDevice = nullptr;
    CameraVideoStreamDevice* m_nightVideoProcessor = nullptr;  // ✅ Keep this
    NightCameraControlDevice* m_nightCamControl = nullptr;
    Plc21Device* m_plc21Device = nullptr;
    Plc42Device* m_plc42Device = nullptr;
    ServoActuatorDevice* m_servoActuatorDevice = nullptr;
    ServoDriverDevice* m_servoAzDevice = nullptr;
    ServoDriverDevice* m_servoElDevice = nullptr;
    QThread* m_servoAzThread = nullptr;
    QThread* m_servoElThread = nullptr;

    // ========================================================================
    // DATA MODELS (Unchanged)
    // ========================================================================
    DayCameraDataModel* m_dayCamControlModel = nullptr;
    GyroDataModel* m_gyroModel = nullptr;
    JoystickDataModel* m_joystickModel = nullptr;
    LensDataModel* m_lensModel = nullptr;
    LrfDataModel* m_lrfModel = nullptr;
    NightCameraDataModel* m_nightCamControlModel = nullptr;
    Plc21DataModel* m_plc21Model = nullptr;
    Plc42DataModel* m_plc42Model = nullptr;
    ServoActuatorDataModel* m_servoActuatorModel = nullptr;
    ServoDriverDataModel* m_servoAzModel = nullptr;
    ServoDriverDataModel* m_servoElModel = nullptr;

    // ========================================================================
    // SYSTEM STATE MODEL (Unchanged)
    // ========================================================================
    SystemStateModel* m_systemStateModel = nullptr;

    // ========================================================================
    // HARDWARE CONTROLLERS (Unchanged)
    // ========================================================================
    GimbalController* m_gimbalController = nullptr;
    WeaponController* m_weaponController = nullptr;
    CameraController* m_cameraController = nullptr;
    JoystickController* m_joystickController = nullptr;

    // ========================================================================
    // QML SYSTEM (NEW)
    // ========================================================================
    // Video provider for QML
    VideoImageProvider* m_videoProvider = nullptr;

    // QML Controllers
    OsdController* m_osdController = nullptr;
    ZoneDefinitionController* m_zoneDefinitionController = nullptr;

    // ViewModels (exposed to QML)
    OsdViewModel* m_osdViewModel = nullptr;
    ZoneDefinitionViewModel* m_zoneDefinitionViewModel = nullptr;
    ZoneMapViewModel* m_zoneMapViewModel = nullptr;
    AreaZoneParameterViewModel* m_areaZoneParameterViewModel = nullptr;
    SectorScanParameterViewModel* m_sectorScanParameterViewModel = nullptr;
    TRPParameterViewModel* m_trpParameterViewModel = nullptr;

    // Helper methods
    void connectDevicesToModels();
    void connectModelsToSystemState();
    void connectVideoToProvider();
};

#endif // SYSTEMCONTROLLER_H
