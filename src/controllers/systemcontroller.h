#ifndef SYSTEMCONTROLLER_H
#define SYSTEMCONTROLLER_H

#include <QObject>
#include <QThread>

// Forward declarations - Hardware
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

// Forward declarations - Data Models
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
class SystemStateModel;

// Forward declarations - Hardware Controllers
class GimbalController;
class WeaponController;
class CameraController;
class JoystickController;

// Forward declarations - QML System
class VideoImageProvider;
class OsdViewModel;
class OsdController;
class ZoneDefinitionViewModel;
class ZoneMapViewModel;
class AreaZoneParameterViewModel;
class SectorScanParameterViewModel;
class TRPParameterViewModel;
class ZoneDefinitionController;
class MenuViewModel;
class MainMenuController;
class ReticleMenuController;
class ColorMenuController;
class ZeroingViewModel;
class ZeroingController;
class WindageViewModel;
class WindageController;
class SystemStatusController;
class SystemStatusViewModel;
class AboutController;
class AboutViewModel;
class ApplicationController;

class QQmlApplicationEngine;

class SystemController : public QObject
{
    Q_OBJECT
public:
    explicit SystemController(QObject *parent = nullptr);
    ~SystemController();

    // Three-phase initialization
    void initializeHardware();
    void initializeQmlSystem(QQmlApplicationEngine* engine);
    void startSystem();

private:
    // Helper methods
    void connectDevicesToModels();
    void connectModelsToSystemState();
    void connectVideoToProvider();
    void createQmlControllers();      // NEW
    void connectQmlControllers();     // NEW

    // === HARDWARE DEVICES ===
    DayCameraControlDevice* m_dayCamControl = nullptr;
    CameraVideoStreamDevice* m_dayVideoProcessor = nullptr;
    ImuDevice* m_gyroDevice = nullptr;
    JoystickDevice* m_joystickDevice = nullptr;
    LensDevice* m_lensDevice = nullptr;
    LRFDevice* m_lrfDevice = nullptr;
    NightCameraControlDevice* m_nightCamControl = nullptr;
    CameraVideoStreamDevice* m_nightVideoProcessor = nullptr;
    Plc21Device* m_plc21Device = nullptr;
    Plc42Device* m_plc42Device = nullptr;
    ServoActuatorDevice* m_servoActuatorDevice = nullptr;
    ServoDriverDevice* m_servoAzDevice = nullptr;
    ServoDriverDevice* m_servoElDevice = nullptr;

    // === DEVICE THREADS ===
    QThread* m_servoAzThread = nullptr;
    QThread* m_servoElThread = nullptr;

    // === DATA MODELS ===
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
    SystemStateModel* m_systemStateModel = nullptr;

    // === HARDWARE CONTROLLERS ===
    GimbalController* m_gimbalController = nullptr;
    WeaponController* m_weaponController = nullptr;
    CameraController* m_cameraController = nullptr;
    JoystickController* m_joystickController = nullptr;

    // === QML SYSTEM ===
    VideoImageProvider* m_videoProvider = nullptr;

    // ViewModels
    OsdViewModel* m_osdViewModel = nullptr;
    ZoneDefinitionViewModel* m_zoneDefinitionViewModel = nullptr;
    ZoneMapViewModel* m_zoneMapViewModel = nullptr;
    AreaZoneParameterViewModel* m_areaZoneParameterViewModel = nullptr;
    SectorScanParameterViewModel* m_sectorScanParameterViewModel = nullptr;
    TRPParameterViewModel* m_trpParameterViewModel = nullptr;
    SystemStatusViewModel* m_systemStatusViewModel = nullptr;
    AboutViewModel* m_aboutViewModel = nullptr;

    // Separate MenuViewModels for each menu
    MenuViewModel* m_mainMenuViewModel = nullptr;
    MenuViewModel* m_reticleMenuViewModel = nullptr;
    MenuViewModel* m_colorMenuViewModel = nullptr;

    ZeroingViewModel* m_zeroingViewModel = nullptr;
    WindageViewModel* m_windageViewModel = nullptr;

    // QML Controllers
    OsdController* m_osdController = nullptr;
    ZoneDefinitionController* m_zoneDefinitionController = nullptr;
    MainMenuController* m_mainMenuController = nullptr;
    ReticleMenuController* m_reticleMenuController = nullptr;
    ColorMenuController* m_colorMenuController = nullptr;
    ZeroingController* m_zeroingController = nullptr;
    WindageController* m_windageController = nullptr;
    SystemStatusController* m_systemStatusController = nullptr;
    AboutController* m_aboutController = nullptr;
    ApplicationController* m_appController = nullptr;
 };

#endif // SYSTEMCONTROLLER_H
