#include "telemetryapiservice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QMutexLocker>

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

TelemetryApiService::TelemetryApiService(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_dataLogger(nullptr)
    , m_stateModel(nullptr)
    , m_authService(nullptr)
    , m_isRunning(false)
{
    qWarning() << "TelemetryApiService: Created with null dependencies. Call start() with valid pointers.";
}

TelemetryApiService::TelemetryApiService(const TelemetryConfig& config,
                                       SystemDataLogger* logger,
                                       SystemStateModel* stateModel,
                                       TelemetryAuthService* authService,
                                       QObject *parent)
    : QObject(parent)
    , m_server(new QHttpServer(this))
    , m_config(config)
    , m_dataLogger(logger)
    , m_stateModel(stateModel)
    , m_authService(authService)
    , m_isRunning(false)
{
    qInfo() << "TelemetryApiService: Initialized";

    registerEndpoints();
}

TelemetryApiService::~TelemetryApiService()
{
    stop();
}

// ============================================================================
// LIFECYCLE MANAGEMENT
// ============================================================================

bool TelemetryApiService::start()
{
    if (!m_config.httpApi.enabled) {
        qInfo() << "TelemetryApiService: HTTP API disabled in configuration";
        return true;
    }

    if (m_isRunning) {
        qWarning() << "TelemetryApiService: Server already running";
        return true;
    }

    if (!m_dataLogger || !m_stateModel || !m_authService) {
        qCritical() << "TelemetryApiService: Cannot start - missing dependencies";
        return false;
    }

    // Validate configuration
    QString validationError = m_config.validate();
    if (!validationError.isEmpty()) {
        qCritical() << "TelemetryApiService: Invalid configuration:" << validationError;
        return false;
    }

    // Start HTTP server
    QHostAddress bindAddress(m_config.httpApi.bindAddress);
    if (!m_server->listen(bindAddress, m_config.httpApi.port)) {
        qCritical() << "TelemetryApiService: Failed to bind to"
                    << m_config.httpApi.bindAddress << ":" << m_config.httpApi.port;
        return false;
    }

    m_isRunning = true;

    QString url = QString("http://%1:%2")
                     .arg(m_config.httpApi.bindAddress)
                     .arg(m_config.httpApi.port);

    qInfo() << "=== TelemetryApiService Started ===";
    qInfo() << "  URL:" << url;
    qInfo() << "  TLS:" << (m_config.tls.enabled ? "Enabled" : "Disabled");
    qInfo() << "  CORS:" << (m_config.httpApi.enableCors ? "Enabled" : "Disabled");
    qInfo() << "  Max Connections:" << m_config.httpApi.maxConnections;

    emit serverStarted(url);

    return true;
}

void TelemetryApiService::stop()
{
    if (!m_isRunning) {
        return;
    }

    m_isRunning = false;
    qInfo() << "TelemetryApiService: Server stopped";
    emit serverStopped();
}

QString TelemetryApiService::getServerUrl() const
{
    if (!m_isRunning) {
        return QString();
    }

    QString protocol = m_config.tls.enabled ? "https" : "http";
    return QString("%1://%2:%3")
              .arg(protocol)
              .arg(m_config.httpApi.bindAddress)
              .arg(m_config.httpApi.port);
}

// ============================================================================
// ENDPOINT REGISTRATION
// ============================================================================

void TelemetryApiService::registerEndpoints()
{
    qInfo() << "TelemetryApiService: Registering API endpoints...";

    registerAuthEndpoints();
    registerTelemetryEndpoints();
    registerStatisticsEndpoints();
    registerExportEndpoints();
    registerSystemEndpoints();
    registerUserEndpoints();

    qInfo() << "TelemetryApiService: All endpoints registered";
}

void TelemetryApiService::registerAuthEndpoints()
{
    // POST /api/auth/login
    m_server->route("/api/auth/login", QHttpServerRequest::Method::Post,
                   [this](const QHttpServerRequest &request) {
        return handleLogin(request);
    });

    // POST /api/auth/refresh
    m_server->route("/api/auth/refresh", QHttpServerRequest::Method::Post,
                   [this](const QHttpServerRequest &request) {
        return handleRefresh(request);
    });

    // POST /api/auth/logout
    m_server->route("/api/auth/logout", QHttpServerRequest::Method::Post,
                   [this](const QHttpServerRequest &request) {
        return handleLogout(request);
    });
}

void TelemetryApiService::registerTelemetryEndpoints()
{
    // GET /api/telemetry/current
    m_server->route("/api/telemetry/current", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetCurrent(request);
    });

    // GET /api/status (legacy endpoint)
    m_server->route("/api/status", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetStatus(request);
    });

    // Historical data endpoints
    m_server->route("/api/telemetry/history/gimbal", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetGimbalHistory(request);
    });

    m_server->route("/api/telemetry/history/imu", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetImuHistory(request);
    });

    m_server->route("/api/telemetry/history/tracking", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetTrackingHistory(request);
    });

    m_server->route("/api/telemetry/history/weapon", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetWeaponHistory(request);
    });

    m_server->route("/api/telemetry/history/camera", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetCameraHistory(request);
    });

    m_server->route("/api/telemetry/history/sensor", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetSensorHistory(request);
    });

    m_server->route("/api/telemetry/history/ballistic", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetBallisticHistory(request);
    });

    m_server->route("/api/telemetry/history/device", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetDeviceHistory(request);
    });
}

void TelemetryApiService::registerStatisticsEndpoints()
{
    m_server->route("/api/telemetry/stats/memory", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetMemoryStats(request);
    });

    m_server->route("/api/telemetry/stats/samples", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetSampleStats(request);
    });

    m_server->route("/api/telemetry/stats/timerange", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetTimeRangeStats(request);
    });
}

void TelemetryApiService::registerExportEndpoints()
{
    m_server->route("/api/telemetry/export/csv", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleExportCsv(request);
    });
}

void TelemetryApiService::registerSystemEndpoints()
{
    m_server->route("/api/health", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleHealthCheck(request);
    });

    m_server->route("/api/version", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleGetVersion(request);
    });
}

void TelemetryApiService::registerUserEndpoints()
{
    m_server->route("/api/users", QHttpServerRequest::Method::Get,
                   [this](const QHttpServerRequest &request) {
        return handleListUsers(request);
    });

    m_server->route("/api/users", QHttpServerRequest::Method::Post,
                   [this](const QHttpServerRequest &request) {
        return handleCreateUser(request);
    });
}

// ============================================================================
// AUTHENTICATION HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleLogin(const QHttpServerRequest &request)
{
    QString clientIp = getClientIp(request);

    // Check rate limit
    if (!checkRateLimit(clientIp)) {
        emit rateLimitExceeded(clientIp, "/api/auth/login");
        return createErrorResponse("Rate limit exceeded", 429);
    }

    // Parse request body
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        return createErrorResponse("Invalid JSON");
    }

    QJsonObject obj = doc.object();
    QString username = obj["username"].toString();
    QString password = obj["password"].toString();

    if (username.isEmpty() || password.isEmpty()) {
        return createErrorResponse("Username and password required");
    }

    // Authenticate
    AuthResult result = m_authService->authenticate(username, password, clientIp);

    logRequest("POST", "/api/auth/login", clientIp, username, result.success ? 200 : 401);

    if (!result.success) {
        return createErrorResponse(result.errorMessage, 401);
    }

    QJsonObject response;
    response["token"] = result.token;
    response["expiresAt"] = result.expiresAt.toString(Qt::ISODate);
    response["role"] = static_cast<int>(result.role);

    return createJsonResponse(response);
}

QHttpServerResponse TelemetryApiService::handleRefresh(const QHttpServerRequest &request)
{
    QString token = extractToken(request);
    QString clientIp = getClientIp(request);

    if (token.isEmpty()) {
        return createErrorResponse("Token required", 401);
    }

    QString newToken = m_authService->refreshToken(token);

    if (newToken.isEmpty()) {
        logRequest("POST", "/api/auth/refresh", clientIp, "", 401);
        return createErrorResponse("Invalid or expired token", 401);
    }

    TokenPayload payload = m_authService->validateToken(newToken);

    QJsonObject response;
    response["token"] = newToken;
    response["expiresAt"] = payload.expiresAt.toString(Qt::ISODate);

    logRequest("POST", "/api/auth/refresh", clientIp, payload.username, 200);

    return createJsonResponse(response);
}

QHttpServerResponse TelemetryApiService::handleLogout(const QHttpServerRequest &request)
{
    QString token = extractToken(request);
    QString clientIp = getClientIp(request);

    if (token.isEmpty()) {
        return createErrorResponse("Token required", 401);
    }

    TokenPayload payload = m_authService->validateToken(token);
    m_authService->revokeToken(token);

    logRequest("POST", "/api/auth/logout", clientIp, payload.username, 200);

    QJsonObject response;
    response["message"] = "Logged out successfully";

    return createJsonResponse(response);
}

// ============================================================================
// TELEMETRY HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleGetCurrent(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadTelemetry);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    const SystemStateData& state = m_stateModel->data();
    QJsonObject jsonData = systemStateToJson(state);

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/current", clientIp, "", 200);

    return createJsonResponse(jsonData);
}

QHttpServerResponse TelemetryApiService::handleGetStatus(const QHttpServerRequest &request)
{
    // Legacy endpoint - reduced data set
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadTelemetry);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    const SystemStateData& state = m_stateModel->data();

    QJsonObject status;
    status["armed"] = state.gunArmed;
    status["ready"] = state.isReady();
    status["azimuth"] = state.gimbalAz;
    status["elevation"] = state.gimbalEl;
    status["tracking"] = state.trackingActive;
    status["camera"] = state.activeCameraIsDay ? "day" : "night";
    status["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/status", clientIp, "", 200);

    return createJsonResponse(status);
}

QHttpServerResponse TelemetryApiService::handleGetGimbalHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getGimbalMotionHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(gimbalMotionToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/gimbal", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetImuHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getImuHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(imuDataToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/imu", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetTrackingHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getTrackingHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(trackingDataToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/tracking", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetWeaponHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getWeaponStatusHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(weaponStatusToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/weapon", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetCameraHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getCameraStatusHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(cameraStatusToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/camera", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetSensorHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getSensorHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(sensorDataToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/sensor", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetBallisticHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getBallisticHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(ballisticDataToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/ballistic", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleGetDeviceHistory(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadHistory);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    auto history = m_dataLogger->getDeviceStatusHistory(startTime, endTime);

    QJsonArray jsonArray;
    for (const auto& point : history) {
        jsonArray.append(deviceStatusToJson(point));
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/history/device", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

// ============================================================================
// STATISTICS HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleGetMemoryStats(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadSystemHealth);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    auto stats = m_dataLogger->getMemoryUsage();

    QJsonObject jsonStats;
    jsonStats["totalBytes"] = static_cast<qint64>(stats.totalBytes);
    jsonStats["totalMB"] = static_cast<double>(stats.totalBytes) / (1024 * 1024);
    jsonStats["deviceStatusBytes"] = static_cast<qint64>(stats.deviceStatusBytes);
    jsonStats["gimbalMotionBytes"] = static_cast<qint64>(stats.gimbalMotionBytes);
    jsonStats["imuDataBytes"] = static_cast<qint64>(stats.imuDataBytes);
    jsonStats["trackingDataBytes"] = static_cast<qint64>(stats.trackingDataBytes);
    jsonStats["weaponStatusBytes"] = static_cast<qint64>(stats.weaponStatusBytes);
    jsonStats["cameraStatusBytes"] = static_cast<qint64>(stats.cameraStatusBytes);
    jsonStats["sensorDataBytes"] = static_cast<qint64>(stats.sensorDataBytes);
    jsonStats["ballisticDataBytes"] = static_cast<qint64>(stats.ballisticDataBytes);
    jsonStats["userInputBytes"] = static_cast<qint64>(stats.userInputBytes);

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/stats/memory", clientIp, "", 200);

    return createJsonResponse(jsonStats);
}

QHttpServerResponse TelemetryApiService::handleGetSampleStats(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadSystemHealth);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QJsonObject jsonStats;
    jsonStats["deviceStatus"] = m_dataLogger->getSampleCount(DataCategory::DeviceStatus);
    jsonStats["gimbalMotion"] = m_dataLogger->getSampleCount(DataCategory::GimbalMotion);
    jsonStats["imuData"] = m_dataLogger->getSampleCount(DataCategory::ImuData);
    jsonStats["trackingData"] = m_dataLogger->getSampleCount(DataCategory::TrackingData);
    jsonStats["weaponStatus"] = m_dataLogger->getSampleCount(DataCategory::WeaponStatus);
    jsonStats["cameraStatus"] = m_dataLogger->getSampleCount(DataCategory::CameraStatus);
    jsonStats["sensorData"] = m_dataLogger->getSampleCount(DataCategory::SensorData);
    jsonStats["ballisticData"] = m_dataLogger->getSampleCount(DataCategory::BallisticData);
    jsonStats["userInput"] = m_dataLogger->getSampleCount(DataCategory::UserInput);

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/stats/samples", clientIp, "", 200);

    return createJsonResponse(jsonStats);
}

QHttpServerResponse TelemetryApiService::handleGetTimeRangeStats(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ReadSystemHealth);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QJsonObject jsonStats;

    auto gimbalRange = m_dataLogger->getDataTimeRange(DataCategory::GimbalMotion);
    QJsonObject gimbalObj;
    gimbalObj["start"] = gimbalRange.first.toString(Qt::ISODate);
    gimbalObj["end"] = gimbalRange.second.toString(Qt::ISODate);
    gimbalObj["durationSec"] = gimbalRange.first.secsTo(gimbalRange.second);
    jsonStats["gimbalMotion"] = gimbalObj;

    auto imuRange = m_dataLogger->getDataTimeRange(DataCategory::ImuData);
    QJsonObject imuObj;
    imuObj["start"] = imuRange.first.toString(Qt::ISODate);
    imuObj["end"] = imuRange.second.toString(Qt::ISODate);
    imuObj["durationSec"] = imuRange.first.secsTo(imuRange.second);
    jsonStats["imuData"] = imuObj;

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/stats/timerange", clientIp, "", 200);

    return createJsonResponse(jsonStats);
}

// ============================================================================
// EXPORT HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleExportCsv(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ExportData);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    if (!m_config.exportSettings.enableCsvExport) {
        return createErrorResponse("CSV export disabled", 403);
    }

    // Parse query parameters
    QUrlQuery query(request.url());
    QString categoryStr = query.queryItemValue("category");

    DataCategory category = DataCategory::GimbalMotion;
    if (categoryStr == "imu") category = DataCategory::ImuData;
    else if (categoryStr == "tracking") category = DataCategory::TrackingData;
    else if (categoryStr == "weapon") category = DataCategory::WeaponStatus;
    else if (categoryStr == "camera") category = DataCategory::CameraStatus;
    else if (categoryStr == "sensor") category = DataCategory::SensorData;
    else if (categoryStr == "ballistic") category = DataCategory::BallisticData;
    else if (categoryStr == "device") category = DataCategory::DeviceStatus;

    QDateTime startTime, endTime;
    QString errorMsg;
    if (!parseTimeRange(request, startTime, endTime, errorMsg)) {
        return createErrorResponse(errorMsg);
    }

    // Generate temporary file path
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString filename = QString("%1_%2.csv").arg(categoryStr).arg(timestamp);
    QString filePath = QString("%1/%2")
                          .arg(m_config.exportSettings.exportDirectory)
                          .arg(filename);

    // Export to CSV
    if (!m_dataLogger->exportToCSV(category, filePath, startTime, endTime)) {
        return createErrorResponse("Export failed", 500);
    }

    QJsonObject response;
    response["message"] = "Export successful";
    response["filename"] = filename;
    response["path"] = filePath;

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/telemetry/export/csv", clientIp, "", 200);

    return createJsonResponse(response);
}

// ============================================================================
// SYSTEM HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleHealthCheck(const QHttpServerRequest &request)
{
    // No authentication required for health check

    QJsonObject health;
    health["status"] = "healthy";
    health["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    health["uptime"] = 0;  // TODO: track uptime
    health["apiVersion"] = "1.0.0";

    // Component health
    QJsonObject components;
    components["dataLogger"] = (m_dataLogger != nullptr);
    components["stateModel"] = (m_stateModel != nullptr);
    components["authService"] = (m_authService != nullptr);
    health["components"] = components;

    return createJsonResponse(health);
}

QHttpServerResponse TelemetryApiService::handleGetVersion(const QHttpServerRequest &request)
{
    QJsonObject version;
    version["apiVersion"] = "1.0.0";
    version["systemName"] = "El 7arress RCWS Telemetry API";
    version["buildDate"] = __DATE__;
    version["buildTime"] = __TIME__;

    return createJsonResponse(version);
}

// ============================================================================
// USER MANAGEMENT HANDLERS
// ============================================================================

QHttpServerResponse TelemetryApiService::handleListUsers(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ManageUsers);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QList<UserAccount> users = m_authService->getAllUsers();

    QJsonArray jsonArray;
    for (const auto& user : users) {
        QJsonObject userObj;
        userObj["username"] = user.username;
        userObj["role"] = static_cast<int>(user.role);
        userObj["enabled"] = user.enabled;
        userObj["createdAt"] = user.createdAt.toString(Qt::ISODate);
        userObj["lastLogin"] = user.lastLogin.toString(Qt::ISODate);
        userObj["description"] = user.description;
        jsonArray.append(userObj);
    }

    QString clientIp = getClientIp(request);
    logRequest("GET", "/api/users", clientIp, "", 200);

    return createJsonResponse(jsonArray);
}

QHttpServerResponse TelemetryApiService::handleCreateUser(const QHttpServerRequest &request)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ManageUsers);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        return createErrorResponse("Invalid JSON");
    }

    QJsonObject obj = doc.object();
    QString username = obj["username"].toString();
    QString password = obj["password"].toString();
    UserRole role = static_cast<UserRole>(obj["role"].toInt());
    QString description = obj["description"].toString();

    if (m_authService->createUser(username, password, role, description)) {
        QJsonObject response;
        response["message"] = "User created successfully";
        response["username"] = username;

        QString clientIp = getClientIp(request);
        logRequest("POST", "/api/users", clientIp, "", 201);

        return createJsonResponse(response, 201);
    }

    return createErrorResponse("Failed to create user", 400);
}

QHttpServerResponse TelemetryApiService::handleDeleteUser(const QHttpServerRequest &request, const QString& username)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ManageUsers);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    if (m_authService->deleteUser(username)) {
        QJsonObject response;
        response["message"] = "User deleted successfully";
        response["username"] = username;

        QString clientIp = getClientIp(request);
        logRequest("DELETE", "/api/users/" + username, clientIp, "", 200);

        return createJsonResponse(response);
    }

    return createErrorResponse("User not found", 404);
}

QHttpServerResponse TelemetryApiService::handleChangePassword(const QHttpServerRequest &request, const QString& username)
{
    QHttpServerResponse authResponse = checkAuthentication(request, Permission::ManageUsers);
    if (authResponse.statusCode() != 200) {
        return authResponse;
    }

    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        return createErrorResponse("Invalid JSON");
    }

    QJsonObject obj = doc.object();
    QString oldPassword = obj["oldPassword"].toString();
    QString newPassword = obj["newPassword"].toString();

    if (m_authService->changePassword(username, oldPassword, newPassword)) {
        QJsonObject response;
        response["message"] = "Password changed successfully";

        QString clientIp = getClientIp(request);
        logRequest("PUT", "/api/users/" + username + "/password", clientIp, "", 200);

        return createJsonResponse(response);
    }

    return createErrorResponse("Failed to change password", 400);
}

// ============================================================================
// HELPER METHODS
// ============================================================================

QString TelemetryApiService::extractToken(const QHttpServerRequest &request) const
{
    QString authHeader = request.value("Authorization");

    if (authHeader.startsWith("Bearer ")) {
        return authHeader.mid(7);  // Remove "Bearer " prefix
    }

    return QString();
}

QHttpServerResponse TelemetryApiService::checkAuthentication(const QHttpServerRequest &request,
                                                            Permission requiredPermission)
{
    QString token = extractToken(request);

    if (token.isEmpty()) {
        return createErrorResponse("Authentication required", 401);
    }

    if (!m_authService->isTokenValid(token)) {
        return createErrorResponse("Invalid or expired token", 401);
    }

    if (!m_authService->hasPermission(token, requiredPermission)) {
        return createErrorResponse("Insufficient permissions", 403);
    }

    // Return success response (will be ignored if authentication passes)
    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

void TelemetryApiService::addCorsHeaders(QHttpServerResponse& response) const
{
    if (!m_config.httpApi.enableCors) {
        return;
    }

    QString allowedOrigins = m_config.httpApi.corsOrigins.join(",");
    // Note: QHttpServerResponse doesn't support custom headers in current Qt version
    // This is a placeholder for future implementation
}

QHttpServerResponse TelemetryApiService::createErrorResponse(const QString& error, int statusCode) const
{
    QJsonObject obj;
    obj["error"] = error;
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    return QHttpServerResponse(obj, static_cast<QHttpServerResponse::StatusCode>(statusCode));
}

QHttpServerResponse TelemetryApiService::createJsonResponse(const QJsonObject& data, int statusCode) const
{
    return QHttpServerResponse(data, static_cast<QHttpServerResponse::StatusCode>(statusCode));
}

QHttpServerResponse TelemetryApiService::createJsonResponse(const QJsonArray& data, int statusCode) const
{
    return QHttpServerResponse(data, static_cast<QHttpServerResponse::StatusCode>(statusCode));
}

bool TelemetryApiService::parseTimeRange(const QHttpServerRequest &request,
                                        QDateTime& startTime, QDateTime& endTime,
                                        QString& errorMsg) const
{
    QUrlQuery query(request.url());

    QString fromStr = query.queryItemValue("from");
    QString toStr = query.queryItemValue("to");

    // Default: last 60 seconds
    if (fromStr.isEmpty() && toStr.isEmpty()) {
        endTime = QDateTime::currentDateTime();
        startTime = endTime.addSecs(-60);
        return true;
    }

    // Parse timestamps
    if (!fromStr.isEmpty()) {
        startTime = QDateTime::fromString(fromStr, Qt::ISODate);
        if (!startTime.isValid()) {
            errorMsg = "Invalid 'from' timestamp format (use ISO 8601)";
            return false;
        }
    }

    if (!toStr.isEmpty()) {
        endTime = QDateTime::fromString(toStr, Qt::ISODate);
        if (!endTime.isValid()) {
            errorMsg = "Invalid 'to' timestamp format (use ISO 8601)";
            return false;
        }
    }

    // Validate range
    if (startTime >= endTime) {
        errorMsg = "'from' must be before 'to'";
        return false;
    }

    qint64 rangeDays = startTime.daysTo(endTime);
    if (rangeDays > m_config.exportSettings.maxExportRangeDays) {
        errorMsg = QString("Time range exceeds maximum of %1 days")
                      .arg(m_config.exportSettings.maxExportRangeDays);
        return false;
    }

    return true;
}

bool TelemetryApiService::checkRateLimit(const QString& clientIp)
{
    QMutexLocker locker(&m_rateLimitMutex);

    QDateTime now = QDateTime::currentDateTime();

    if (!m_rateLimits.contains(clientIp)) {
        m_rateLimits[clientIp] = RateLimitInfo();
        m_rateLimits[clientIp].windowStart = now;
    }

    RateLimitInfo& info = m_rateLimits[clientIp];

    // Reset window if minute has passed
    if (info.windowStart.secsTo(now) >= 60) {
        info.requestCount = 0;
        info.windowStart = now;
    }

    info.requestCount++;

    return info.requestCount <= m_config.httpApi.rateLimitPerMinute;
}

QString TelemetryApiService::getClientIp(const QHttpServerRequest &request) const
{
    // Try X-Forwarded-For header first (if behind reverse proxy)
    QString forwardedFor = request.value("X-Forwarded-For");
    if (!forwardedFor.isEmpty()) {
        return forwardedFor.split(',').first().trimmed();
    }

    // Fallback: use remote address (not available in current Qt HTTP Server API)
    return "unknown";
}

void TelemetryApiService::logRequest(const QString& method, const QString& path,
                                     const QString& clientIp, const QString& username,
                                     int statusCode)
{
    QString logMsg = QString("%1 %2 from %3 [%4] - Status: %5")
                        .arg(method)
                        .arg(path)
                        .arg(clientIp)
                        .arg(username.isEmpty() ? "anonymous" : username)
                        .arg(statusCode);

    qDebug() << "API:" << logMsg;

    emit requestReceived(method, path, clientIp);
}

// ============================================================================
// DATA CONVERSION HELPERS
// ============================================================================

QJsonObject TelemetryApiService::gimbalMotionToJson(const GimbalMotionData& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["gimbalAz"] = data.gimbalAz;
    obj["gimbalEl"] = data.gimbalEl;
    obj["azimuthSpeed"] = data.azimuthSpeed;
    obj["elevationSpeed"] = data.elevationSpeed;
    obj["gimbalSpeed"] = data.gimbalSpeed;
    obj["opMode"] = static_cast<int>(data.opMode);
    obj["motionMode"] = static_cast<int>(data.motionMode);
    return obj;
}

QJsonObject TelemetryApiService::imuDataToJson(const ImuDataPoint& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["roll"] = data.imuRollDeg;
    obj["pitch"] = data.imuPitchDeg;
    obj["yaw"] = data.imuYawDeg;
    obj["gyroX"] = data.gyroX;
    obj["gyroY"] = data.gyroY;
    obj["gyroZ"] = data.gyroZ;
    obj["accelX"] = data.accelX;
    obj["accelY"] = data.accelY;
    obj["accelZ"] = data.accelZ;
    obj["temperature"] = data.temperature;
    return obj;
}

QJsonObject TelemetryApiService::trackingDataToJson(const TrackingDataPoint& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["trackingPhase"] = static_cast<int>(data.trackingPhase);
    obj["trackingActive"] = data.trackingActive;
    obj["hasValidTarget"] = data.trackerHasValidTarget;
    obj["targetAz"] = data.targetAz;
    obj["targetEl"] = data.targetEl;
    obj["targetCenterX"] = data.trackedTargetCenterX_px;
    obj["targetCenterY"] = data.trackedTargetCenterY_px;
    return obj;
}

QJsonObject TelemetryApiService::weaponStatusToJson(const WeaponStatusData& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["armed"] = data.gunArmed;
    obj["ammoLoaded"] = data.ammoLoaded;
    obj["fireMode"] = static_cast<int>(data.fireMode);
    obj["ammunitionLevel"] = data.stationAmmunitionLevel;
    obj["inNoFireZone"] = data.isReticleInNoFireZone;
    return obj;
}

QJsonObject TelemetryApiService::cameraStatusToJson(const CameraStatusData& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["activeCamera"] = data.activeCameraIsDay ? "day" : "night";
    obj["dayZoom"] = data.dayZoomPosition;
    obj["nightZoom"] = data.nightZoomPosition;
    obj["dayHFOV"] = data.dayCurrentHFOV;
    obj["nightHFOV"] = data.nightCurrentHFOV;
    return obj;
}

QJsonObject TelemetryApiService::sensorDataToJson(const SensorDataPoint& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["lrfDistance"] = data.lrfDistance;
    obj["radarPlotCount"] = data.radarPlotCount;
    obj["selectedTrackId"] = data.selectedRadarTrackId;
    return obj;
}

QJsonObject TelemetryApiService::ballisticDataToJson(const BallisticDataPoint& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["zeroingActive"] = data.zeroingModeActive;
    obj["zeroingAzOffset"] = data.zeroingAzimuthOffset;
    obj["zeroingElOffset"] = data.zeroingElevationOffset;
    obj["windageActive"] = data.windageModeActive;
    obj["windSpeed"] = data.windageSpeedKnots;
    obj["windDirection"] = data.windageDirection;
    obj["leadAngleActive"] = data.leadAngleActive;
    return obj;
}

QJsonObject TelemetryApiService::deviceStatusToJson(const DeviceStatusData& data) const
{
    QJsonObject obj;
    obj["timestamp"] = data.timestamp.toString(Qt::ISODate);
    obj["azMotorTemp"] = data.azMotorTemp;
    obj["azDriverTemp"] = data.azDriverTemp;
    obj["elMotorTemp"] = data.elMotorTemp;
    obj["elDriverTemp"] = data.elDriverTemp;
    obj["panelTemp"] = data.panelTemperature;
    obj["stationTemp"] = data.stationTemperature;
    obj["dayCameraConnected"] = data.dayCameraConnected;
    obj["nightCameraConnected"] = data.nightCameraConnected;
    obj["emergencyStop"] = data.emergencyStopActive;
    return obj;
}

QJsonObject TelemetryApiService::systemStateToJson(const SystemStateData& state) const
{
    QJsonObject obj;
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Gimbal
    obj["gimbalAz"] = state.gimbalAz;
    obj["gimbalEl"] = state.gimbalEl;

    // IMU
    obj["roll"] = state.imuRollDeg;
    obj["pitch"] = state.imuPitchDeg;
    obj["yaw"] = state.imuYawDeg;

    // Weapon
    obj["armed"] = state.gunArmed;
    obj["ready"] = state.isReady();

    // Tracking
    obj["trackingActive"] = state.trackingActive;
    obj["trackingPhase"] = static_cast<int>(state.currentTrackingPhase);

    // Camera
    obj["activeCamera"] = state.activeCameraIsDay ? "day" : "night";
    obj["dayZoom"] = state.dayZoomPosition;

    // Sensors
    obj["lrfDistance"] = state.lrfDistance;

    // Operational
    obj["opMode"] = static_cast<int>(state.opMode);
    obj["motionMode"] = static_cast<int>(state.motionMode);

    return obj;
}
