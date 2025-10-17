#include "systemcontroller.h"

// Hardware Devices (Unchanged includes)

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

// Data Models (Unchanged includes)
#include "models/domain/gyrodatamodel.h"
#include "models/domain/joystickdatamodel.h"
#include "models/domain/lensdatamodel.h"
#include "models/domain/lrfdatamodel.h"
#include "models/domain/plc21datamodel.h"
#include "models/domain/plc42datamodel.h"
#include "models/domain/servoactuatordatamodel.h"
#include "models/domain/servodriverdatamodel.h"
#include "models/domain/systemstatemodel.h"

// Hardware Controllers (Unchanged includes)
#include "controllers/gimbalcontroller.h"
#include "controllers/weaponcontroller.h"
#include "controllers/cameracontroller.h"
#include "controllers/joystickcontroller.h"

// *** ADD: QML System includes ***
#include "controllers/osdcontroller.h"
#include "controllers/zonedefinitioncontroller.h"
#include "models/osdviewmodel.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "models/areazoneparameterviewmodel.h"
#include "models/sectorscanparameterviewmodel.h"
#include "models/trpparameterviewmodel.h"
#include "video/videoimageprovider.h"

#include <QQmlContext>
#include <QDebug>

SystemController::SystemController(QObject *parent)
    : QObject(parent)
{
}

SystemController::~SystemController()
{
    qInfo() << "SystemController: Shutting down...";

    // Stop video processors first
    if (m_dayVideoProcessor && m_dayVideoProcessor->isRunning()) {
        m_dayVideoProcessor->stop();
    }
    if (m_nightVideoProcessor && m_nightVideoProcessor->isRunning()) {
        m_nightVideoProcessor->stop();
    }

    bool stopped1 = m_dayVideoProcessor ? m_dayVideoProcessor->wait(2000) : true;
    bool stopped2 = m_nightVideoProcessor ? m_nightVideoProcessor->wait(2000) : true;

    if (!stopped1) qWarning() << "Day camera did not stop gracefully.";
    if (!stopped2) qWarning() << "Night camera did not stop gracefully.";

    // Stop servo threads
    if (m_servoAzThread && m_servoAzThread->isRunning()) {
        m_servoAzThread->quit();
    }
    if (m_servoElThread && m_servoElThread->isRunning()) {
        m_servoElThread->quit();
    }

    bool azStopped = m_servoAzThread ? m_servoAzThread->wait(1000) : true;
    bool elStopped = m_servoElThread ? m_servoElThread->wait(1000) : true;

    if (!azStopped) qWarning() << "Azimuth servo thread did not stop gracefully.";
    if (!elStopped) qWarning() << "Elevation servo thread did not stop gracefully.";

    qInfo() << "SystemController: Shutdown complete.";
}

// ============================================================================
// PHASE 1: INITIALIZE HARDWARE (Same as before, mostly)
// ============================================================================
void SystemController::initializeHardware()
{
    qInfo() << "SystemController: Phase 1 - Initializing hardware...";

    // Configuration
    const int sourceWidth = 1280;
    const int sourceHeight = 720;
    const QString dayDevicePath = "/dev/video0";
    const QString nightDevicePath = "/dev/video1";

    // ========================================================================
    // 1. CREATE SYSTEM STATE MODEL FIRST (needed by cameras)
    // ========================================================================
    m_systemStateModel = new SystemStateModel(this);
    qInfo() << "  ✓ SystemStateModel created";

    // ========================================================================
    // 2. CREATE DEVICES
    // ========================================================================
    m_dayCamControl = new DayCameraControlDevice(this);
    m_gyroDevice = new ImuDevice("/dev/ttyUSB2", 115200, 1, this);
    m_joystickDevice = new JoystickDevice(this);
    m_lensDevice = new LensDevice(this);
    m_lrfDevice = new LRFDevice(this);
    m_nightCamControl = new NightCameraControlDevice(this);
    m_plc21Device = new Plc21Device("/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if00", 115200, 31, QSerialPort::EvenParity, this);
    m_plc42Device = new Plc42Device("/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if02", 115200, 31, QSerialPort::EvenParity, this);
    m_servoActuatorDevice = new ServoActuatorDevice(this);

    // Servo threads
    m_servoAzThread = new QThread(this);
    m_servoAzDevice = new ServoDriverDevice("az", "/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04", 230400, 2, QSerialPort::NoParity, nullptr);
    m_servoElThread = new QThread(this);
    m_servoElDevice = new ServoDriverDevice("el", "/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06", 230400, 1, QSerialPort::NoParity, nullptr);

    // *** IMPORTANT: Create CameraVideoStreamDevice (with tracking) ***
    m_dayVideoProcessor = new CameraVideoStreamDevice(0, dayDevicePath, sourceWidth, sourceHeight, m_systemStateModel, nullptr);
    m_nightVideoProcessor = new CameraVideoStreamDevice(1, nightDevicePath, sourceWidth, sourceHeight, m_systemStateModel, nullptr);

    qInfo() << "  ✓ Devices created";

    // ========================================================================
    // 3. CREATE DATA MODELS
    // ========================================================================
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

    // ========================================================================
    // 4. CONNECT DEVICES TO MODELS
    // ========================================================================
    connectDevicesToModels();

    // ========================================================================
    // 5. CONNECT MODELS TO SYSTEM STATE
    // ========================================================================
    connectModelsToSystemState();

    // ========================================================================
    // 6. CREATE HARDWARE CONTROLLERS
    // ========================================================================
    m_gimbalController = new GimbalController(m_servoAzDevice, m_servoElDevice, m_plc42Device, m_systemStateModel, this);
    m_weaponController = new WeaponController(m_systemStateModel, m_servoActuatorDevice, m_plc42Device, this);
    m_cameraController = new CameraController(m_dayCamControl, m_dayVideoProcessor, m_nightCamControl, m_nightVideoProcessor, m_lensDevice, m_systemStateModel);
    m_joystickController = new JoystickController(m_joystickModel, m_systemStateModel, m_gimbalController, m_cameraController, m_weaponController, this);

    qInfo() << "  ✓ Hardware controllers created";

    qInfo() << "SystemController: Phase 1 complete - Hardware initialized";
}

// ============================================================================
// PHASE 2: INITIALIZE QML SYSTEM
// ============================================================================
void SystemController::initializeQmlSystem(QQmlApplicationEngine* engine)
{
    qInfo() << "SystemController: Phase 2 - Initializing QML system...";

    if (!engine) {
        qCritical() << "QML engine is null!";
        return;
    }

    // ========================================================================
    // 1. CREATE VIDEO IMAGE PROVIDER (Bridge between C++ video and QML)
    // ========================================================================
    m_videoProvider = new VideoImageProvider();
    engine->addImageProvider("video", m_videoProvider);
    qInfo() << "  ✓ VideoImageProvider registered";

    // ========================================================================
    // 2. CONNECT CAMERA VIDEO STREAMS TO QML PROVIDER
    // ========================================================================
    connectVideoToProvider();

    // ========================================================================
    // 3. CREATE VIEWMODELS
    // ========================================================================
    m_osdViewModel = new OsdViewModel(this);
    m_zoneDefinitionViewModel = new ZoneDefinitionViewModel(this);
    m_zoneMapViewModel = new ZoneMapViewModel(this);
    m_areaZoneParameterViewModel = new AreaZoneParameterViewModel(this);
    m_sectorScanParameterViewModel = new SectorScanParameterViewModel(this);
    m_trpParameterViewModel = new TRPParameterViewModel(this);

    qInfo() << "  ✓ ViewModels created";

    // ========================================================================
    // 4. CREATE QML CONTROLLERS
    // ========================================================================
    m_osdController = new OsdController(this);
    m_zoneDefinitionController = new ZoneDefinitionController(this);

    // Pass dependencies manually (since we're not using ServiceManager)
    m_osdController->setViewModel(m_osdViewModel);
    m_osdController->setStateModel(m_systemStateModel);

    m_zoneDefinitionController->setViewModel(m_zoneDefinitionViewModel);
    m_zoneDefinitionController->setMapViewModel(m_zoneMapViewModel);
    m_zoneDefinitionController->setParameterViewModels(
        m_areaZoneParameterViewModel,
        m_sectorScanParameterViewModel,
        m_trpParameterViewModel
        );
    m_zoneDefinitionController->setStateModel(m_systemStateModel);

    qInfo() << "  ✓ QML Controllers created";

    // ========================================================================
    // 5. INITIALIZE CONTROLLERS
    // ========================================================================
    m_osdController->initialize();
    m_zoneDefinitionController->initialize();

    qInfo() << "  ✓ QML Controllers initialized";

    // ========================================================================
    // 6. EXPOSE TO QML (Context Properties)
    // ========================================================================
    QQmlContext* rootContext = engine->rootContext();

    // ViewModels
    rootContext->setContextProperty("osdViewModel", m_osdViewModel);
    rootContext->setContextProperty("zoneDefinitionViewModel", m_zoneDefinitionViewModel);
    rootContext->setContextProperty("zoneMapViewModel", m_zoneMapViewModel);
    rootContext->setContextProperty("areaZoneParameterViewModel", m_areaZoneParameterViewModel);
    rootContext->setContextProperty("sectorScanParameterViewModel", m_sectorScanParameterViewModel);
    rootContext->setContextProperty("trpParameterViewModel", m_trpParameterViewModel);

    // System State Model (for debugging/access)
    rootContext->setContextProperty("systemStateModel", m_systemStateModel);

    qInfo() << "  ✓ QML context properties set";

    qInfo() << "SystemController: Phase 2 complete - QML system initialized";
}

// ============================================================================
// PHASE 3: START SYSTEM
// ============================================================================
void SystemController::startSystem()
{
    qInfo() << "SystemController: Phase 3 - Starting system...";

    // ========================================================================
    // 1. OPEN DEVICE CONNECTIONS
    // ========================================================================
    m_dayCamControl->openSerialPort("/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BCD9DCABCD-if00");
    m_gyroDevice->connectDevice();
    m_lrfDevice->openSerialPort("/dev/ttyUSB1");
    m_nightCamControl->openSerialPort("/dev/serial/by-id/usb-1a86_USB_Single_Serial_56D1123075-if00");
    m_plc21Device->connectDevice();
    m_plc42Device->connectDevice();
    m_servoActuatorDevice->openSerialPort("/dev/ttyUSB0");

    if (m_servoAzDevice) m_servoAzDevice->connectDevice();
    if (m_servoElDevice) m_servoElDevice->connectDevice();

    qInfo() << "  ✓ Device connections opened";

    // ========================================================================
    // 2. INITIALIZE CAMERAS
    // ========================================================================
    m_dayCamControl->zoomOut();
    m_dayCamControl->zoomStop();
    m_nightCamControl->setDigitalZoom(0);

    qInfo() << "  ✓ Cameras initialized";

    // ========================================================================
    // 3. START VIDEO PROCESSING THREADS
    // ========================================================================
    if (m_dayVideoProcessor) {
        m_dayVideoProcessor->start();
        qInfo() << "  ✓ Day camera thread started";
    }

    if (m_nightVideoProcessor) {
        m_nightVideoProcessor->start();
        qInfo() << "  ✓ Night camera thread started";
    }

    // ========================================================================
    // 4. CLEAR ALARMS
    // ========================================================================
    if (m_gimbalController) {
        m_gimbalController->clearAlarms();
        qInfo() << "  ✓ Gimbal alarms cleared";
    }

    qInfo() << "SystemController: Phase 3 complete - System started and running!";
}

// ============================================================================
// HELPER METHODS
// ============================================================================

void SystemController::connectDevicesToModels()
{
    // (Same as your original code - unchanged)
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
}

void SystemController::connectModelsToSystemState()
{
    // (Same as your original code - unchanged)
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

    // *** IMPORTANT: Connect SystemStateModel back to cameras (for OSD data) ***
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
}

void SystemController::connectVideoToProvider()
{
    if (!m_videoProvider) return;

    // *** CRITICAL: Connect CameraVideoStreamDevice frameDataReady to VideoImageProvider ***

    // Day camera
    if (m_dayVideoProcessor) {
        connect(m_dayVideoProcessor, &CameraVideoStreamDevice::frameDataReady,
                this, [this](const FrameData& data) {
                    // Only update provider if this is the active camera
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
                    // Only update provider if this is the active camera
                    if (data.cameraIndex == 1 && !m_systemStateModel->data().activeCameraIsDay) {
                        m_videoProvider->updateImage(data.baseImage);
                    }
                });
        qInfo() << "  ✓ Night camera connected to video provider";
    }
}
