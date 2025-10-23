#include "systemcontroller.h"

// Hardware Devices
#include "hardware/devices/daycameracontroldevice.h"
#include "hardware/devices/cameravideostreamdevice.h"
#include "hardware/devices/imudevice.h"
#include "hardware/devices/joystickdevice.h"
#include "hardware/devices/lensdevice.h"
#include "hardware/devices/lrfdevice.h"
#include "hardware/devices/nightcameracontroldevice.h"
#include "hardware/devices/plc21device.h"
#include "hardware/devices/plc42device.h"
#include "hardware/devices/servoactuatordevice.h"
#include "hardware/devices/servodriverdevice.h"

// Data Models
#include "models/domain/gyrodatamodel.h"
#include "models/domain/joystickdatamodel.h"
#include "models/domain/lensdatamodel.h"
#include "models/domain/lrfdatamodel.h"
#include "models/domain/plc21datamodel.h"
#include "models/domain/plc42datamodel.h"
#include "models/domain/servoactuatordatamodel.h"
#include "models/domain/servodriverdatamodel.h"
#include "models/domain/systemstatemodel.h"

// Hardware Controllers
#include "controllers/gimbalcontroller.h"
#include "controllers/weaponcontroller.h"
#include "controllers/cameracontroller.h"
#include "controllers/joystickcontroller.h"

// QML System
#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "controllers/osdcontroller.h"
#include "controllers/zonedefinitioncontroller.h"
#include "controllers/systemstatuscontroller.h"
#include "controllers/aboutcontroller.h"
#include "models/osdviewmodel.h"
#include "models/menuviewmodel.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "models/areazoneparameterviewmodel.h"
#include "models/sectorscanparameterviewmodel.h"
#include "models/trpparameterviewmodel.h"
#include "models/zeroingviewmodel.h"
#include "models/windageviewmodel.h"
#include "models/systemstatusviewmodel.h"
#include "models/aboutviewmodel.h"
#include "video/videoimageprovider.h"

#include "deviceconfiguration.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QDebug>

SystemController::SystemController(QObject *parent)
    : QObject(parent)
{
}

SystemController::~SystemController()
{
    qInfo() << "SystemController: Shutting down...";

    // Stop video processors
    if (m_dayVideoProcessor && m_dayVideoProcessor->isRunning()) {
        m_dayVideoProcessor->stop();
        m_dayVideoProcessor->wait(2000);
    }
    if (m_nightVideoProcessor && m_nightVideoProcessor->isRunning()) {
        m_nightVideoProcessor->stop();
        m_nightVideoProcessor->wait(2000);
    }

    // Stop servo threads
    if (m_servoAzThread && m_servoAzThread->isRunning()) {
        m_servoAzThread->quit();
        m_servoAzThread->wait(1000);
    }
    if (m_servoElThread && m_servoElThread->isRunning()) {
        m_servoElThread->quit();
        m_servoElThread->wait(1000);
    }

    qInfo() << "SystemController: Shutdown complete.";
}

// ============================================================================
// PHASE 1: INITIALIZE HARDWARE
// ============================================================================
void SystemController::initializeHardware()
{
    qInfo() << "=== PHASE 1: Hardware Initialization ===";

    // Get configuration
    const auto& videoConf = DeviceConfiguration::video();
    const auto& imuConf = DeviceConfiguration::imu();
    const auto& lrfConf = DeviceConfiguration::lrf();
    const auto& plc21Conf = DeviceConfiguration::plc21();
    const auto& plc42Conf = DeviceConfiguration::plc42();
    const auto& servoAzConf = DeviceConfiguration::servoAz();
    const auto& servoElConf = DeviceConfiguration::servoEl();
    const auto& actuatorConf = DeviceConfiguration::actuator();

    // 1. Create System State Model
    m_systemStateModel = new SystemStateModel(this);
    qInfo() << "  ✓ SystemStateModel created";

    // 2. Create Hardware Devices using configuration
    m_dayCamControl = new DayCameraControlDevice(this);
    m_gyroDevice = new ImuDevice(imuConf.port, imuConf.baudRate, imuConf.slaveId, this);
    m_joystickDevice = new JoystickDevice(this);
    m_lensDevice = new LensDevice(this);
    m_lrfDevice = new LRFDevice(this);
    m_nightCamControl = new NightCameraControlDevice(this);
    m_plc21Device = new Plc21Device(plc21Conf.port, plc21Conf.baudRate, plc21Conf.slaveId, plc21Conf.parity, this);
    m_plc42Device = new Plc42Device(plc42Conf.port, plc42Conf.baudRate, plc42Conf.slaveId, plc42Conf.parity, this);
    m_servoActuatorDevice = new ServoActuatorDevice(this);

    // Servo devices with configuration
    m_servoAzThread = new QThread(this);
    m_servoAzDevice = new ServoDriverDevice(servoAzConf.name, servoAzConf.port, servoAzConf.baudRate, servoAzConf.slaveId, servoAzConf.parity, nullptr);
    m_servoElThread = new QThread(this);
    m_servoElDevice = new ServoDriverDevice(servoElConf.name, servoElConf.port, servoElConf.baudRate, servoElConf.slaveId, servoElConf.parity, nullptr);

    // Video processors with configuration
    m_dayVideoProcessor = new CameraVideoStreamDevice(0, videoConf.dayDevicePath, videoConf.sourceWidth, videoConf.sourceHeight, m_systemStateModel, nullptr);
    m_nightVideoProcessor = new CameraVideoStreamDevice(1, videoConf.nightDevicePath, videoConf.sourceWidth, videoConf.sourceHeight, m_systemStateModel, nullptr);

    qInfo() << "  ✓ Hardware devices created from configuration";

    // 3. Create Data Models
    m_dayCamControlModel = new DayCameraDataModel(this);
    m_gyroModel = new GyroDataModel(this);
    m_joystickModel = new JoystickDataModel(this);
    m_lensModel = new LensDataModel(this);
    m_lrfModel = new LrfDataModel(this);
    m_nightCamControlModel = new NightCameraDataModel(this);
    m_plc21Model = new Plc21DataModel(this);
    m_plc42Model = new Plc42DataModel(this);
    m_servoActuatorModel = new ServoActuatorDataModel(this);
    m_servoAzModel = new ServoDriverDataModel(this);
    m_servoElModel = new ServoDriverDataModel(this);

    qInfo() << "  ✓ Data models created";

    // 4. Connect Devices to Models
    connectDevicesToModels();

    // 5. Connect Models to System State
    connectModelsToSystemState();

    // 6. Create Hardware Controllers
    m_gimbalController = new GimbalController(m_servoAzDevice, m_servoElDevice, m_plc42Device, m_systemStateModel, this);
    m_weaponController = new WeaponController(m_systemStateModel, m_servoActuatorDevice, m_plc42Device, this);
    m_cameraController = new CameraController(m_dayCamControl, m_dayVideoProcessor, m_nightCamControl, m_nightVideoProcessor, m_lensDevice, m_systemStateModel);
    m_joystickController = new JoystickController(m_joystickModel, m_systemStateModel, m_gimbalController, m_cameraController, m_weaponController, this);

    qInfo() << "  ✓ Hardware controllers created";
    qInfo() << "=== PHASE 1 COMPLETE ===\n";
}

// ============================================================================
// PHASE 2: INITIALIZE QML SYSTEM
// ============================================================================
void SystemController::initializeQmlSystem(QQmlApplicationEngine* engine)
{
    qInfo() << "=== PHASE 2: QML System Initialization ===";

    if (!engine) {
        qCritical() << "QML engine is null!";
        return;
    }

    // 1. Create Video Provider
    m_videoProvider = new VideoImageProvider();
    engine->addImageProvider("video", m_videoProvider);
    qInfo() << "  ✓ VideoImageProvider registered";

    // 2. Connect Video Streams to Provider
    connectVideoToProvider();

    // 3. Create ViewModels
    m_osdViewModel = new OsdViewModel(this);

    // Separate MenuViewModels for each menu
    m_mainMenuViewModel = new MenuViewModel(this);
    m_reticleMenuViewModel = new MenuViewModel(this);
    m_colorMenuViewModel = new MenuViewModel(this);

    m_zoneDefinitionViewModel = new ZoneDefinitionViewModel(this);
    m_zoneMapViewModel = new ZoneMapViewModel(this);
    m_areaZoneParameterViewModel = new AreaZoneParameterViewModel(this);
    m_sectorScanParameterViewModel = new SectorScanParameterViewModel(this);
    m_trpParameterViewModel = new TRPParameterViewModel(this);
    m_zeroingViewModel = new ZeroingViewModel(this);
    m_windageViewModel = new WindageViewModel(this);
    m_systemStatusViewModel = new SystemStatusViewModel(this);
    m_aboutViewModel = new AboutViewModel(this);

    qInfo() << "  ✓ ViewModels created";

    // 4. Create QML Controllers
    createQmlControllers();

    // 5. Connect QML Controllers
    connectQmlControllers();

    // 6. Expose to QML
    QQmlContext* rootContext = engine->rootContext();

    // ViewModels
    rootContext->setContextProperty("osdViewModel", m_osdViewModel);
    rootContext->setContextProperty("mainMenuViewModel", m_mainMenuViewModel);
    rootContext->setContextProperty("reticleMenuViewModel", m_reticleMenuViewModel);
    rootContext->setContextProperty("colorMenuViewModel", m_colorMenuViewModel);
    rootContext->setContextProperty("zoneDefinitionViewModel", m_zoneDefinitionViewModel);
    rootContext->setContextProperty("zoneMapViewModel", m_zoneMapViewModel);
    rootContext->setContextProperty("areaZoneParameterViewModel", m_areaZoneParameterViewModel);
    rootContext->setContextProperty("sectorScanParameterViewModel", m_sectorScanParameterViewModel);
    rootContext->setContextProperty("trpParameterViewModel", m_trpParameterViewModel);
    rootContext->setContextProperty("zeroingViewModel", m_zeroingViewModel);
    rootContext->setContextProperty("windageViewModel", m_windageViewModel);
    rootContext->setContextProperty("systemStatusViewModel", m_systemStatusViewModel);
    rootContext->setContextProperty("aboutViewModel", m_aboutViewModel);
    // System State (for debugging/direct access)
    rootContext->setContextProperty("systemStateModel", m_systemStateModel);

    // Application Controller (main entry point for QML)
    rootContext->setContextProperty("appController", m_appController);

    qInfo() << "  ✓ QML context properties set";
    qInfo() << "=== PHASE 2 COMPLETE ===\n";
}

// ============================================================================
// PHASE 3: START SYSTEM
// ============================================================================
void SystemController::startSystem()
{
    qInfo() << "=== PHASE 3: System Startup ===";

    // Get configuration
    const auto& videoConf = DeviceConfiguration::video();
    const auto& lrfConf = DeviceConfiguration::lrf();
    const auto& actuatorConf = DeviceConfiguration::actuator();

    // Open Device Connections using configuration
    m_dayCamControl->openSerialPort(videoConf.dayControlPort);
    m_gyroDevice->connectDevice();
    m_lrfDevice->openSerialPort(lrfConf.port);
    m_nightCamControl->openSerialPort(videoConf.nightControlPort);
    m_plc21Device->connectDevice();
    m_plc42Device->connectDevice();
    m_servoActuatorDevice->openSerialPort(actuatorConf.port);

    if (m_servoAzDevice) m_servoAzDevice->connectDevice();
    if (m_servoElDevice) m_servoElDevice->connectDevice();

    qInfo() << "  ✓ Device connections opened";

    // 2. Initialize Cameras
    m_dayCamControl->zoomOut();
    m_dayCamControl->zoomStop();
    m_nightCamControl->setDigitalZoom(0);

    qInfo() << "  ✓ Cameras initialized";

    // 3. Start Video Processing Threads
    if (m_dayVideoProcessor) {
        m_dayVideoProcessor->start();
        qInfo() << "  ✓ Day camera thread started";
    }

    if (m_nightVideoProcessor) {
        m_nightVideoProcessor->start();
        qInfo() << "  ✓ Night camera thread started";
    }

    // 4. Clear Gimbal Alarms
    if (m_gimbalController) {
        m_gimbalController->clearAlarms();
        qInfo() << "  ✓ Gimbal alarms cleared";
    }

    qInfo() << "=== PHASE 3 COMPLETE - SYSTEM RUNNING ===\n";
}

// ============================================================================
// HELPER: CREATE QML CONTROLLERS
// ============================================================================
void SystemController::createQmlControllers()
{
    // Create OSD Controller
    m_osdController = new OsdController(this);
    m_osdController->setViewModel(m_osdViewModel);
    m_osdController->setStateModel(m_systemStateModel);

    // Create Menu Controllers
    m_mainMenuController = new MainMenuController(this);
    m_mainMenuController->setViewModel(m_mainMenuViewModel);
    m_mainMenuController->setStateModel(m_systemStateModel);

    m_reticleMenuController = new ReticleMenuController(this);
    m_reticleMenuController->setViewModel(m_reticleMenuViewModel);
    m_reticleMenuController->setOsdViewModel(m_osdViewModel);
    m_reticleMenuController->setStateModel(m_systemStateModel);


    m_colorMenuController = new ColorMenuController(this);
    m_colorMenuController->setViewModel(m_colorMenuViewModel);
    m_colorMenuController->setOsdViewModel(m_osdViewModel);
    m_colorMenuController->setStateModel(m_systemStateModel);

    // Create Procedure Controllers
    m_zeroingController = new ZeroingController(this);
    m_zeroingController->setViewModel(m_zeroingViewModel);
    m_zeroingController->setStateModel(m_systemStateModel);

    m_windageController = new WindageController(this);
    m_windageController->setViewModel(m_windageViewModel);
    m_windageController->setStateModel(m_systemStateModel);

    // Create Zone Definition Controller
    m_zoneDefinitionController = new ZoneDefinitionController(this);
    m_zoneDefinitionController->setViewModel(m_zoneDefinitionViewModel);
    m_zoneDefinitionController->setMapViewModel(m_zoneMapViewModel);
    m_zoneDefinitionController->setParameterViewModels(
        m_areaZoneParameterViewModel,
        m_sectorScanParameterViewModel,
        m_trpParameterViewModel
        );
    m_zoneDefinitionController->setStateModel(m_systemStateModel);

    m_systemStatusController = new SystemStatusController();
    m_systemStatusController->setViewModel(m_systemStatusViewModel);
    m_systemStatusController->setStateModel(m_systemStateModel);


    m_aboutController = new AboutController();
    m_aboutController->setViewModel(m_aboutViewModel);
    m_aboutController->setStateModel(m_systemStateModel);


    // Create Application Controller (LAST - needs all other controllers)
    m_appController = new ApplicationController(this);

    // Inject dependencies into ApplicationController
    m_appController->setMainMenuController(m_mainMenuController);
    m_appController->setReticleMenuController(m_reticleMenuController);
    m_appController->setColorMenuController(m_colorMenuController);
    m_appController->setZeroingController(m_zeroingController);
    m_appController->setWindageController(m_windageController);
    m_appController->setZoneDefinitionController(m_zoneDefinitionController);
    m_appController->setSystemStatusController(m_systemStatusController);
    m_appController->setAboutController(m_aboutController);

    m_appController->setSystemStateModel(m_systemStateModel);

    qInfo() << "  ✓ QML Controllers created";
}

// ============================================================================
// HELPER: CONNECT QML CONTROLLERS
// ============================================================================
void SystemController::connectQmlControllers()
{
    // Initialize all controllers
    m_osdController->initialize();
    m_mainMenuController->initialize();
    m_reticleMenuController->initialize();
    m_colorMenuController->initialize();
    m_zeroingController->initialize();
    m_windageController->initialize();
    m_zoneDefinitionController->initialize();
    m_systemStatusController->initialize();
    m_aboutController->initialize();

    // Initialize ApplicationController LAST (it connects to all others)
    m_appController->initialize();
    // =========================================================================
    // OSD CONTROLLER CONNECTIONS
    // =========================================================================
    if (m_osdController) {
        qDebug() << "Connecting OsdController...";

        // Phase 1: Connect to SystemStateModel (already done in OsdController::initialize())
        // Phase 2: Connect to camera frame data (ACTIVATE HERE!)

        if (m_dayVideoProcessor) {
            connect(m_dayVideoProcessor, &CameraVideoStreamDevice::frameDataReady,
                    m_osdController, &OsdController::onFrameDataReady);
            qDebug() << "✅ Day camera frameDataReady → OsdController (Phase 2 ACTIVE)";
        } else {
            qWarning() << "⚠️ Day camera not available for OSD connection";
        }

        if (m_nightVideoProcessor) {
            connect(m_nightVideoProcessor, &CameraVideoStreamDevice::frameDataReady,
                    m_osdController, &OsdController::onFrameDataReady);
            qDebug() << "✅ Night camera frameDataReady → OsdController (Phase 2 ACTIVE)";
        } else {
            qWarning() << "⚠️ Night camera not available for OSD connection";
        }

        qDebug() << "=== OSD Phase 2: Frame-synchronized updates ENABLED ===";
    } else {
        qWarning() << "⚠️ OsdController is null, cannot connect cameras";
    }
    qInfo() << "  ✓ QML Controllers initialized and connected";
}

// ============================================================================
// HELPER: CONNECT DEVICES TO MODELS
// ============================================================================
void SystemController::connectDevicesToModels()
{
    connect(m_dayCamControl, &DayCameraControlDevice::dayCameraDataChanged,
            m_dayCamControlModel, &DayCameraDataModel::updateData);

    connect(m_gyroDevice, &ImuDevice::imuDataChanged,
            m_gyroModel, &GyroDataModel::updateData);

    connect(m_joystickDevice, &JoystickDevice::axisMoved,
            m_joystickModel, &JoystickDataModel::onRawAxisMoved);
    connect(m_joystickDevice, &JoystickDevice::buttonPressed,
            m_joystickModel, &JoystickDataModel::onRawButtonChanged);
    connect(m_joystickDevice, &JoystickDevice::hatMoved,
            m_joystickModel, &JoystickDataModel::onRawHatMoved);

    connect(m_lensDevice, &LensDevice::lensDataChanged,
            m_lensModel, &LensDataModel::updateData);

    connect(m_lrfDevice, &LRFDevice::lrfDataChanged,
            m_lrfModel, &LrfDataModel::updateData);

    connect(m_nightCamControl, &NightCameraControlDevice::nightCameraDataChanged,
            m_nightCamControlModel, &NightCameraDataModel::updateData);

    connect(m_plc21Device, &Plc21Device::panelDataChanged,
            m_plc21Model, &Plc21DataModel::updateData);

    connect(m_plc42Device, &Plc42Device::plc42DataChanged,
            m_plc42Model, &Plc42DataModel::updateData);

    connect(m_servoActuatorDevice, &ServoActuatorDevice::actuatorDataChanged,
            m_servoActuatorModel, &ServoActuatorDataModel::updateData);

    connect(m_servoAzDevice, &ServoDriverDevice::servoDataChanged,
            m_servoAzModel, &ServoDriverDataModel::updateData);

    connect(m_servoElDevice, &ServoDriverDevice::servoDataChanged,
            m_servoElModel, &ServoDriverDataModel::updateData);


    qInfo() << "  ✓ Devices connected to models";
}

// ============================================================================
// HELPER: CONNECT MODELS TO SYSTEM STATE
// ============================================================================
void SystemController::connectModelsToSystemState()
{
    connect(m_dayCamControlModel, &DayCameraDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onDayCameraDataChanged);

    connect(m_gyroModel, &GyroDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onGyroDataChanged);

    connect(m_joystickModel, &JoystickDataModel::axisMoved,
            m_systemStateModel, &SystemStateModel::onJoystickAxisChanged);
    connect(m_joystickModel, &JoystickDataModel::buttonPressed,
            m_systemStateModel, &SystemStateModel::onJoystickButtonChanged);
    connect(m_joystickModel, &JoystickDataModel::hatMoved,
            m_systemStateModel, &SystemStateModel::onJoystickHatChanged);

    connect(m_lensModel, &LensDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onLensDataChanged);

    connect(m_lrfModel, &LrfDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onLrfDataChanged);

    connect(m_nightCamControlModel, &NightCameraDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onNightCameraDataChanged);

    connect(m_plc21Model, &Plc21DataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onPlc21DataChanged);

    connect(m_plc42Model, &Plc42DataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onPlc42DataChanged);

    connect(m_servoActuatorModel, &ServoActuatorDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onServoActuatorDataChanged);

    connect(m_servoAzModel, &ServoDriverDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onServoAzDataChanged);

    connect(m_servoElModel, &ServoDriverDataModel::dataChanged,
            m_systemStateModel, &SystemStateModel::onServoElDataChanged);

    // Connect SystemStateModel back to cameras
    if (m_systemStateModel && m_dayVideoProcessor) {
        connect(m_systemStateModel, &SystemStateModel::dataChanged,
                m_dayVideoProcessor, &CameraVideoStreamDevice::onSystemStateChanged,
                Qt::QueuedConnection);
    }

    if (m_systemStateModel && m_nightVideoProcessor) {
        connect(m_systemStateModel, &SystemStateModel::dataChanged,
                m_nightVideoProcessor, &CameraVideoStreamDevice::onSystemStateChanged,
                Qt::QueuedConnection);
    }

    qInfo() << "  ✓ Models connected to SystemStateModel";
}

// ============================================================================
// HELPER: CONNECT VIDEO TO PROVIDER
// ============================================================================
void SystemController::connectVideoToProvider()
{
    if (!m_videoProvider) return;

    // Day camera
    if (m_dayVideoProcessor) {
        connect(m_dayVideoProcessor, &CameraVideoStreamDevice::frameDataReady,
                this, [this](const FrameData& data) {
                    if (data.cameraIndex == 0 && m_systemStateModel->data().activeCameraIsDay) {
                        m_videoProvider->updateImage(data.baseImage);
                    }
                });
        qInfo() << "  ✓ Day camera connected to video provider";
    }

    // Night camera
    if (m_nightVideoProcessor) {
        connect(m_nightVideoProcessor, &CameraVideoStreamDevice::frameDataReady,
                this, [this](const FrameData& data) {
                    if (data.cameraIndex == 1 && !m_systemStateModel->data().activeCameraIsDay) {
                        m_videoProvider->updateImage(data.baseImage);
                    }
                });
        qInfo() << "  ✓ Night camera connected to video provider";
    }
}
