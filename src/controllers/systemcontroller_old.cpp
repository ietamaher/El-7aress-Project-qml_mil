#include "systemcontroller.h"

// Hardware Devices
#include "hardware/devices/daycameracontroldevice.h"
#include "hardware/devices/cameravideostreamdevice.h"
#include "hardware/devices/imudevice.h"
#include "hardware/devices/joystickdevice.h"
#include "hardware/devices/lrfdevice.h"
#include "hardware/devices/nightcameracontroldevice.h"
#include "hardware/devices/plc21device.h"
#include "hardware/devices/plc42device.h"
#include "hardware/devices/radardevice.h"
#include "hardware/devices/servoactuatordevice.h"
#include "hardware/devices/servodriverdevice.h"

// Transport & Protocol Parsers
#include "hardware/communication/modbustransport.h"
#include "hardware/communication/serialporttransport.h"
#include "hardware/protocols/ImuProtocolParser.h"
#include "hardware/protocols/DayCameraProtocolParser.h"
#include "hardware/protocols/NightCameraProtocolParser.h"
#include "hardware/protocols/JoystickProtocolParser.h"
#include "hardware/protocols/LrfProtocolParser.h"
#include "hardware/protocols/RadarProtocolParser.h"
#include "hardware/protocols/Plc21ProtocolParser.h"
#include "hardware/protocols/Plc42ProtocolParser.h"
#include "hardware/protocols/ServoDriverProtocolParser.h"
#include "hardware/protocols/ServoActuatorProtocolParser.h"

// Data Models
#include "models/domain/daycameradatamodel.h"
#include "models/domain/nightcameradatamodel.h"
#include "models/domain/gyrodatamodel.h"
#include "models/domain/joystickdatamodel.h"
#include "models/domain/lrfdatamodel.h"
#include "models/domain/radardatamodel.h"
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

#include "logger/systemdatalogger.h"
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

    // 2. Create Data Logger with custom configuration
    SystemDataLogger::LoggerConfig loggerConfig;
    loggerConfig.gimbalMotionBufferSize = 60000;  // 1 minute at 60 Hz
    loggerConfig.imuDataBufferSize = 120000;      // 20 minutes at 100 Hz
    loggerConfig.trackingDataBufferSize = 36000;  // 20 minutes at 30 Hz
    loggerConfig.enableDatabasePersistence = true; // Enable long-term storage
    loggerConfig.databasePath = "./data/rcws_history.db";

    m_dataLogger = new SystemDataLogger(loggerConfig, this);
    qInfo() << "  ✓ SystemDataLogger created";

    // 3. Connect SystemStateModel to DataLogger
    connect(m_systemStateModel, &SystemStateModel::dataChanged,
            m_dataLogger, &SystemDataLogger::onSystemStateChanged);

    qInfo() << "  ✓ DataLogger connected to SystemStateModel";

    // 3. Create Transport Layer (MIL-STD Architecture)
    m_imuTransport = new ModbusTransport(this);
    m_dayCameraTransport = new SerialPortTransport(this);
    m_nightCameraTransport = new SerialPortTransport(this);
    m_lrfTransport = new SerialPortTransport(this);
    m_radarTransport = new SerialPortTransport(this);
    m_plc21Transport = new ModbusTransport(this);
    m_plc42Transport = new ModbusTransport(this);
    m_servoAzTransport = new ModbusTransport(this);
    m_servoElTransport = new ModbusTransport(this);
    m_servoActuatorTransport = new SerialPortTransport(this);
    qInfo() << "  ✓ Transport layer created";

    // 4. Create Protocol Parsers (MIL-STD Architecture)
    m_imuParser = new ImuProtocolParser(this);
    m_dayCameraParser = new DayCameraProtocolParser(this);
    m_nightCameraParser = new NightCameraProtocolParser(this);
    m_joystickParser = new JoystickProtocolParser(this);
    m_lrfParser = new LrfProtocolParser(this);
    m_radarParser = new RadarProtocolParser(this);
    m_plc21Parser = new Plc21ProtocolParser(this);
    m_plc42Parser = new Plc42ProtocolParser(this);
    m_servoAzParser = new ServoDriverProtocolParser(this);
    m_servoElParser = new ServoDriverProtocolParser(this);
    m_servoActuatorParser = new ServoActuatorProtocolParser(this);
    qInfo() << "  ✓ Protocol parsers created";

    // 5. Create Hardware Devices using MIL-STD dependency injection

    // Day Camera (Pelco-D via Serial)
    m_dayCamControl = new DayCameraControlDevice("dayCamera", this);
    m_dayCamControl->setDependencies(m_dayCameraTransport, m_dayCameraParser);

    // IMU (Modbus RTU)
    m_gyroDevice = new ImuDevice("imu", this);
    m_gyroDevice->setDependencies(m_imuTransport, m_imuParser);

    // Joystick (SDL2 - no transport needed)
    m_joystickDevice = new JoystickDevice(this);
    m_joystickDevice->setParser(m_joystickParser);

    // LRF (Serial binary protocol)
    m_lrfDevice = new LRFDevice(this);
    m_lrfDevice->setDependencies(m_lrfTransport, m_lrfParser);

    // Night Camera (TAU2 via Serial)
    m_nightCamControl = new NightCameraControlDevice("nightCamera", this);
    m_nightCamControl->setDependencies(m_nightCameraTransport, m_nightCameraParser);

    // Radar (NMEA 0183 via Serial)
    m_radarDevice = new RadarDevice("radar", this);
    m_radarDevice->setDependencies(m_radarTransport, m_radarParser);

    // PLC21 (Modbus RTU)
    m_plc21Device = new Plc21Device("plc21", this);
    m_plc21Device->setDependencies(m_plc21Transport, m_plc21Parser);

    // PLC42 (Modbus RTU)
    m_plc42Device = new Plc42Device("plc42", this);
    m_plc42Device->setDependencies(m_plc42Transport, m_plc42Parser);

    // Servo Actuator (Serial ASCII protocol)
    m_servoActuatorDevice = new ServoActuatorDevice("servoActuator", this);
    m_servoActuatorDevice->setDependencies(m_servoActuatorTransport, m_servoActuatorParser);

    // Servo Driver devices (Modbus RTU) with MIL-STD architecture
    m_servoAzThread = new QThread(this);
    m_servoAzDevice = new ServoDriverDevice(servoAzConf.name, nullptr);
    m_servoAzDevice->setDependencies(m_servoAzTransport, m_servoAzParser);

    m_servoElThread = new QThread(this);
    m_servoElDevice = new ServoDriverDevice(servoElConf.name, nullptr);
    m_servoElDevice->setDependencies(m_servoElTransport, m_servoElParser);

    // Video processors with configuration
    m_dayVideoProcessor = new CameraVideoStreamDevice(0, videoConf.dayDevicePath, videoConf.sourceWidth, videoConf.sourceHeight, m_systemStateModel, nullptr);
    m_nightVideoProcessor = new CameraVideoStreamDevice(1, videoConf.nightDevicePath, videoConf.sourceWidth, videoConf.sourceHeight, m_systemStateModel, nullptr);

    qInfo() << "  ✓ Hardware devices created with dependency injection";

    // 3. Create Data Models
    m_dayCamControlModel = new DayCameraDataModel(this);
    m_gyroModel = new GyroDataModel(this);
    m_joystickModel = new JoystickDataModel(this);
    m_lrfModel = new LrfDataModel(this);
    m_nightCamControlModel = new NightCameraDataModel(this);
    m_plc21Model = new Plc21DataModel(this);
    m_plc42Model = new Plc42DataModel(this);
    m_radarModel = new RadarDataModel(this);
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
    m_cameraController = new CameraController(m_dayCamControl, m_dayVideoProcessor, m_nightCamControl, m_nightVideoProcessor, m_systemStateModel);
    m_joystickController = new JoystickController(m_joystickModel, m_systemStateModel, m_gimbalController, m_cameraController, m_weaponController, this);

    qInfo() << "  ✓ Hardware controllers created";

    m_apiServer = new QHttpServer(this);

// API endpoint: Get gimbal history
m_apiServer->route("/api/gimbal-history", [this](const QHttpServerRequest &request) {
    QDateTime endTime = QDateTime::currentDateTime();
    QDateTime startTime = endTime.addSecs(-60);
    
    auto history = m_dataLogger->getGimbalMotionHistory(startTime, endTime);
    
    QJsonArray jsonArray;
    for (const auto& point : history) {
        QJsonObject obj;
        obj["timestamp"] = point.timestamp.toMSecsSinceEpoch();
        obj["az"] = point.gimbalAz;
        obj["el"] = point.gimbalEl;
        jsonArray.append(obj);
    }
    
    return QHttpServerResponse(jsonArray);
});

m_apiServer->listen(QHostAddress::Any, 8080);
qInfo() << "API Server listening on port 8080";

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
    const auto& imuConf = DeviceConfiguration::imu();
    const auto& lrfConf = DeviceConfiguration::lrf();
    const auto& plc21Conf = DeviceConfiguration::plc21();
    const auto& plc42Conf = DeviceConfiguration::plc42();
    const auto& actuatorConf = DeviceConfiguration::actuator();
    const auto& servoAzConf = DeviceConfiguration::servoAz();
    const auto& servoElConf = DeviceConfiguration::servoEl();

    // 1. Configure and open Transport connections (MIL-STD Architecture)

    // IMU Transport (Modbus RTU)
    QJsonObject imuTransportConfig;
    imuTransportConfig["port"] = imuConf.port;
    imuTransportConfig["baudRate"] = imuConf.baudRate;
    imuTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);  // IMU uses no parity
    imuTransportConfig["slaveId"] = imuConf.slaveId;
    m_imuTransport->open(imuTransportConfig);

    // Day Camera Transport (Serial)
    QJsonObject dayCameraTransportConfig;
    dayCameraTransportConfig["port"] = videoConf.dayControlPort;
    dayCameraTransportConfig["baudRate"] = 9600;  // Pelco-D standard
    dayCameraTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    m_dayCameraTransport->open(dayCameraTransportConfig);

    // Night Camera Transport (Serial)
    QJsonObject nightCameraTransportConfig;
    nightCameraTransportConfig["port"] = videoConf.nightControlPort;
    nightCameraTransportConfig["baudRate"] = 921600;  // FLIR Boson 640 standard
    nightCameraTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    m_nightCameraTransport->open(nightCameraTransportConfig);

    // PLC21 Transport (Modbus RTU)
    QJsonObject plc21TransportConfig;
    plc21TransportConfig["port"] = plc21Conf.port;
    plc21TransportConfig["baudRate"] = plc21Conf.baudRate;
    plc21TransportConfig["parity"] = static_cast<int>(plc21Conf.parity);
    plc21TransportConfig["slaveId"] = plc21Conf.slaveId;
    m_plc21Transport->open(plc21TransportConfig);

    // PLC42 Transport (Modbus RTU)
    QJsonObject plc42TransportConfig;
    plc42TransportConfig["port"] = plc42Conf.port;
    plc42TransportConfig["baudRate"] = plc42Conf.baudRate;
    plc42TransportConfig["parity"] = static_cast<int>(plc42Conf.parity);
    plc42TransportConfig["slaveId"] = plc42Conf.slaveId;
    m_plc42Transport->open(plc42TransportConfig);

    // Servo Azimuth Transport (Modbus RTU)
    QJsonObject servoAzTransportConfig;
    servoAzTransportConfig["port"] = servoAzConf.port;
    servoAzTransportConfig["baudRate"] = servoAzConf.baudRate;
    servoAzTransportConfig["parity"] = static_cast<int>(servoAzConf.parity);
    servoAzTransportConfig["slaveId"] = servoAzConf.slaveId;
    m_servoAzTransport->open(servoAzTransportConfig);

    // Servo Elevation Transport (Modbus RTU)
    QJsonObject servoElTransportConfig;
    servoElTransportConfig["port"] = servoElConf.port;
    servoElTransportConfig["baudRate"] = servoElConf.baudRate;
    servoElTransportConfig["parity"] = static_cast<int>(servoElConf.parity);
    servoElTransportConfig["slaveId"] = servoElConf.slaveId;
    m_servoElTransport->open(servoElTransportConfig);

    // Servo Actuator Transport (Serial)
    QJsonObject servoActuatorTransportConfig;
    servoActuatorTransportConfig["port"] = actuatorConf.port;
    servoActuatorTransportConfig["baudRate"] = actuatorConf.baudRate;
    servoActuatorTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    m_servoActuatorTransport->open(servoActuatorTransportConfig);

    // LRF Transport (Serial binary protocol)
    QJsonObject lrfTransportConfig;
    lrfTransportConfig["port"] = lrfConf.port;
    lrfTransportConfig["baudRate"] = lrfConf.baudRate;
    lrfTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    m_lrfTransport->open(lrfTransportConfig);

    // Radar Transport (Serial NMEA 0183)
    /*const auto& radarConf = DeviceConfiguration::radar();
    QJsonObject radarTransportConfig;
    radarTransportConfig["port"] = radarConf.port;
    radarTransportConfig["baudRate"] = radarConf.baudRate;
    radarTransportConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    m_radarTransport->open(radarTransportConfig);*/

    qInfo() << "  ✓ Transport connections opened";

    // 2. Initialize all MIL-STD refactored devices
    m_dayCamControl->initialize();
    m_gyroDevice->initialize();
    m_joystickDevice->initialize();
    m_nightCamControl->initialize();
    m_plc21Device->initialize();
    m_plc42Device->initialize();
    m_lrfDevice->initialize();
    m_radarDevice->initialize();
    m_servoActuatorDevice->initialize();

    if (m_servoAzDevice) m_servoAzDevice->initialize();
    if (m_servoElDevice) m_servoElDevice->initialize();

    qInfo() << "  ✓ All MIL-STD devices initialized";

    // 4. Configure camera defaults
    m_dayCamControl->zoomOut();
    m_dayCamControl->zoomStop();
    m_nightCamControl->setDigitalZoom(0);

    qInfo() << "  ✓ Camera defaults configured";

    // 5. Start Video Processing Threads
    if (m_dayVideoProcessor) {
        m_dayVideoProcessor->start();
        qInfo() << "  ✓ Day camera thread started";
    }

    if (m_nightVideoProcessor) {
        m_nightVideoProcessor->start();
        qInfo() << "  ✓ Night camera thread started";
    }

    // 6. Clear Gimbal Alarms
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


    // LRFDevice uses shared_ptr, need to dereference for model
    connect(m_lrfDevice, &LRFDevice::lrfDataChanged,
            m_lrfModel, [this](std::shared_ptr<const LrfData> data) {
                if (data) {
                    m_lrfModel->updateData(*data);
                }
            });

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
