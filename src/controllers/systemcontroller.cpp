#include "systemcontroller.h"

// Managers
#include "managers/HardwareManager.h"
#include "managers/ViewModelRegistry.h"
#include "managers/ControllerRegistry.h"

// Controllers (needed for direct access)
#include "gimbalcontroller.h"

// Configuration & Validation
#include "deviceconfiguration.h"
#include "config/ConfigurationValidator.h"

// Models & Services
#include "models/domain/systemstatemodel.h"
#include "logger/systemdatalogger.h"
#include "video/videoimageprovider.h"

// Hardware Devices (for video connection)
#include "hardware/devices/cameravideostreamdevice.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

SystemController::SystemController(QObject *parent)
    : QObject(parent)
{
    qInfo() << "SystemController: Created";
}

SystemController::~SystemController()
{
    qInfo() << "SystemController: Shutting down...";

    // Managers handle their own cleanup
    // Just ensure proper destruction order

    qInfo() << "SystemController: Shutdown complete.";
}

// ============================================================================
// PHASE 1: INITIALIZE HARDWARE
// ============================================================================

void SystemController::initializeHardware()
{
    qInfo() << "=== PHASE 1: Hardware Initialization ===";

    // 1. Create SystemStateModel (central data hub)
    m_systemStateModel = new SystemStateModel(this);
    qInfo() << "  ✓ SystemStateModel created";

    // 2. Create Data Logger
    createDataLogger();

    // 3. Create managers
    createManagers();

    // 4. Create hardware using HardwareManager
    if (!m_hardwareManager->createHardware()) {
        qCritical() << "Failed to create hardware!";
        return;
    }

    // 5. Connect devices to models
    if (!m_hardwareManager->connectDevicesToModels()) {
        qCritical() << "Failed to connect devices to models!";
        return;
    }

    // 6. Connect models to SystemState
    if (!m_hardwareManager->connectModelsToSystemState()) {
        qCritical() << "Failed to connect models to system state!";
        return;
    }

    // 7. Create hardware controllers
    if (!m_controllerRegistry->createHardwareControllers()) {
        qCritical() << "Failed to create hardware controllers!";
        return;
    }

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

    // 3. Create ViewModels using ViewModelRegistry
    if (!m_viewModelRegistry->createViewModels()) {
        qCritical() << "Failed to create ViewModels!";
        return;
    }

    // 4. Create QML Controllers using ControllerRegistry
    if (!m_controllerRegistry->createQmlControllers()) {
        qCritical() << "Failed to create QML controllers!";
        return;
    }

    // 5. Initialize controllers
    if (!m_controllerRegistry->initializeControllers()) {
        qCritical() << "Failed to initialize controllers!";
        return;
    }

    // 6. Connect video to OSD for frame-synchronized updates
    if (!m_controllerRegistry->connectVideoToOsd()) {
        qCritical() << "Failed to connect video to OSD!";
        return;
    }

    // 7. Register ViewModels with QML
    QQmlContext* rootContext = engine->rootContext();
    if (!m_viewModelRegistry->registerWithQml(rootContext)) {
        qCritical() << "Failed to register ViewModels with QML!";
        return;
    }

    // 8. Register Controllers with QML
    if (!m_controllerRegistry->registerWithQml(rootContext)) {
        qCritical() << "Failed to register Controllers with QML!";
        return;
    }

    qInfo() << "=== PHASE 2 COMPLETE ===\n";
}

// ============================================================================
// PHASE 3: START SYSTEM
// ============================================================================

void SystemController::startSystem()
{
    qInfo() << "=== PHASE 3: System Startup ===";

    // 1. Start the OSD startup sequence (shows professional startup messages)
    if (m_controllerRegistry->osdController()) {
        m_controllerRegistry->osdController()->startStartupSequence();
        qInfo() << "  ✓ OSD startup sequence started";
    }

    // 2. Start hardware (open transports, initialize devices)
    if (!m_hardwareManager->startHardware()) {
        qCritical() << "Failed to start hardware!";
        return;
    }

    // 3. Clear gimbal alarms (via gimbal controller)
    if (m_controllerRegistry->gimbalController()) {
        m_controllerRegistry->gimbalController()->clearAlarms();
        qInfo() << "  ✓ Gimbal alarms cleared";
    }

    // 4. Create API server
    createApiServer();

    qInfo() << "=== PHASE 3 COMPLETE - SYSTEM RUNNING ===\n";
}

// ============================================================================
// HELPER METHODS
// ============================================================================

void SystemController::createManagers()
{
    qInfo() << "  Creating managers...";

    // Create HardwareManager
    m_hardwareManager = new HardwareManager(m_systemStateModel, this);

    // Create ViewModelRegistry
    m_viewModelRegistry = new ViewModelRegistry(this);

    // Create ControllerRegistry
    m_controllerRegistry = new ControllerRegistry(
        m_hardwareManager,
        m_viewModelRegistry,
        m_systemStateModel,
        this
    );

    qInfo() << "    ✓ All managers created";
}

void SystemController::createDataLogger()
{
    qInfo() << "  Creating data logger...";

    const auto& perfConf = DeviceConfiguration::performance();
    const auto& sysConf = DeviceConfiguration::system();

    SystemDataLogger::LoggerConfig loggerConfig;
    loggerConfig.gimbalMotionBufferSize = perfConf.gimbalMotionBufferSize;
    loggerConfig.imuDataBufferSize = perfConf.imuDataBufferSize;
    loggerConfig.trackingDataBufferSize = perfConf.trackingDataBufferSize;
    loggerConfig.enableDatabasePersistence = sysConf.enableDataLogger;
    loggerConfig.databasePath = sysConf.databasePath;

    m_dataLogger = new SystemDataLogger(loggerConfig, this);

    // Connect SystemStateModel to DataLogger
    connect(m_systemStateModel, &SystemStateModel::dataChanged,
            m_dataLogger, &SystemDataLogger::onSystemStateChanged);

    qInfo() << "    ✓ DataLogger created and connected";
}

void SystemController::createApiServer()
{
    qInfo() << "  Creating API server...";

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

    // API endpoint: Get system status
    m_apiServer->route("/api/status", [this](const QHttpServerRequest &request) {
        const auto& data = m_systemStateModel->data();

        QJsonObject status;
        status["armed"] = data.gunArmed;
        status["ready"] = data.isReady();
        status["azimuth"] = data.gimbalAz;
        status["elevation"] = data.gimbalEl;
        status["tracking"] = data.trackingActive;
        status["camera"] = data.activeCameraIsDay ? "day" : "night";

        return QHttpServerResponse(status);
    });

    m_apiServer->listen(QHostAddress::Any, 8080);
    qInfo() << "    ✓ API Server listening on port 8080";
}

void SystemController::connectVideoToProvider()
{
    if (!m_videoProvider || !m_hardwareManager) {
        qWarning() << "Cannot connect video: missing components";
        return;
    }

    qInfo() << "  Connecting video streams to provider...";

    // Day camera
    if (m_hardwareManager->dayVideoProcessor()) {
        connect(m_hardwareManager->dayVideoProcessor(), &CameraVideoStreamDevice::frameDataReady,
                this, [this](const FrameData& data) {
                    if (data.cameraIndex == 0 && m_systemStateModel->data().activeCameraIsDay) {
                        m_videoProvider->updateImage(data.baseImage);
                    }
                });
        qInfo() << "    ✓ Day camera connected to video provider";
    }

    // Night camera
    if (m_hardwareManager->nightVideoProcessor()) {
        connect(m_hardwareManager->nightVideoProcessor(), &CameraVideoStreamDevice::frameDataReady,
                this, [this](const FrameData& data) {
                    if (data.cameraIndex == 1 && !m_systemStateModel->data().activeCameraIsDay) {
                        m_videoProvider->updateImage(data.baseImage);
                    }
                });
        qInfo() << "    ✓ Night camera connected to video provider";
    }
}
