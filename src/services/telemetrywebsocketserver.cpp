#include "telemetrywebsocketserver.h"
#include "models/domain/systemstatemodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QMutexLocker>

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

TelemetryWebSocketServer::TelemetryWebSocketServer(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_authService(nullptr)
    , m_stateModel(nullptr)
    , m_isRunning(false)
    , m_updateTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_totalMessagesSent(0)
    , m_totalBytesSent(0)
{
    qWarning() << "TelemetryWebSocketServer: Created with null dependencies";
}

TelemetryWebSocketServer::TelemetryWebSocketServer(const WebSocketConfig& config,
                                                   TelemetryAuthService* authService,
                                                   SystemStateModel* stateModel,
                                                   QObject *parent)
    : QObject(parent)
    , m_server(new QWebSocketServer("RCWS Telemetry Server", QWebSocketServer::NonSecureMode, this))
    , m_config(config)
    , m_authService(authService)
    , m_stateModel(stateModel)
    , m_isRunning(false)
    , m_updateTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_totalMessagesSent(0)
    , m_totalBytesSent(0)
{
    qInfo() << "TelemetryWebSocketServer: Initialized";

    // Connect server signals
    connect(m_server, &QWebSocketServer::newConnection,
            this, &TelemetryWebSocketServer::onNewConnection);

    // Setup update timer (broadcasts telemetry periodically)
    connect(m_updateTimer, &QTimer::timeout,
            this, &TelemetryWebSocketServer::onUpdateTimerTimeout);

    // Setup heartbeat timer (checks for inactive clients)
    connect(m_heartbeatTimer, &QTimer::timeout,
            this, &TelemetryWebSocketServer::onHeartbeatTimerTimeout);
}

TelemetryWebSocketServer::~TelemetryWebSocketServer()
{
    stop();
}

// ============================================================================
// LIFECYCLE MANAGEMENT
// ============================================================================

bool TelemetryWebSocketServer::start()
{
    if (!m_config.enabled) {
        qInfo() << "TelemetryWebSocketServer: WebSocket disabled in configuration";
        return true;
    }

    if (m_isRunning) {
        qWarning() << "TelemetryWebSocketServer: Server already running";
        return true;
    }

    if (!m_authService || !m_stateModel) {
        qCritical() << "TelemetryWebSocketServer: Cannot start - missing dependencies";
        return false;
    }

    // Start WebSocket server
    QHostAddress bindAddress(m_config.bindAddress);
    if (!m_server->listen(bindAddress, m_config.port)) {
        qCritical() << "TelemetryWebSocketServer: Failed to bind to"
                    << m_config.bindAddress << ":" << m_config.port;
        return false;
    }

    m_isRunning = true;

    // Start update timer
    int updateIntervalMs = 1000 / m_config.updateRateHz;
    m_updateTimer->start(updateIntervalMs);

    // Start heartbeat timer (check every 10 seconds)
    m_heartbeatTimer->start(10000);

    QString url = QString("ws://%1:%2/telemetry")
                     .arg(m_config.bindAddress)
                     .arg(m_config.port);

    qInfo() << "=== TelemetryWebSocketServer Started ===";
    qInfo() << "  URL:" << url;
    qInfo() << "  Update Rate:" << m_config.updateRateHz << "Hz";
    qInfo() << "  Max Connections:" << m_config.maxConnections;
    qInfo() << "  Heartbeat Interval:" << m_config.heartbeatIntervalSec << "seconds";

    emit serverStarted(url);

    return true;
}

void TelemetryWebSocketServer::stop()
{
    if (!m_isRunning) {
        return;
    }

    m_updateTimer->stop();
    m_heartbeatTimer->stop();

    // Disconnect all clients
    QMutexLocker locker(&m_clientsMutex);
    for (auto* socket : m_clients.keys()) {
        socket->close(QWebSocketProtocol::CloseCodeNormal, "Server shutting down");
    }
    m_clients.clear();

    m_server->close();
    m_isRunning = false;

    qInfo() << "TelemetryWebSocketServer: Server stopped";
    qInfo() << "  Total messages sent:" << m_totalMessagesSent;
    qInfo() << "  Total bytes sent:" << m_totalBytesSent;

    emit serverStopped();
}

QString TelemetryWebSocketServer::getServerUrl() const
{
    if (!m_isRunning) {
        return QString();
    }

    return QString("ws://%1:%2/telemetry")
              .arg(m_config.bindAddress)
              .arg(m_config.port);
}

int TelemetryWebSocketServer::getClientCount() const
{
    QMutexLocker locker(&m_clientsMutex);
    return m_clients.size();
}

int TelemetryWebSocketServer::getAuthenticatedClientCount() const
{
    QMutexLocker locker(&m_clientsMutex);
    int count = 0;
    for (const auto& client : m_clients) {
        if (client.authenticated) {
            count++;
        }
    }
    return count;
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void TelemetryWebSocketServer::setConfig(const WebSocketConfig& config)
{
    m_config = config;

    if (m_isRunning) {
        // Update timers
        int updateIntervalMs = 1000 / m_config.updateRateHz;
        m_updateTimer->setInterval(updateIntervalMs);
    }
}

void TelemetryWebSocketServer::setUpdateRate(int hz)
{
    if (hz < 1 || hz > 100) {
        qWarning() << "TelemetryWebSocketServer: Invalid update rate:" << hz << "(must be 1-100 Hz)";
        return;
    }

    m_config.updateRateHz = hz;

    if (m_isRunning) {
        int updateIntervalMs = 1000 / hz;
        m_updateTimer->setInterval(updateIntervalMs);
        qInfo() << "TelemetryWebSocketServer: Update rate changed to" << hz << "Hz";
    }
}

// ============================================================================
// CONNECTION HANDLING
// ============================================================================

void TelemetryWebSocketServer::onNewConnection()
{
    QWebSocket* socket = m_server->nextPendingConnection();

    if (!socket) {
        return;
    }

    QString clientIp = getClientIp(socket);

    QMutexLocker locker(&m_clientsMutex);

    // Check max connections
    if (m_clients.size() >= m_config.maxConnections) {
        qWarning() << "TelemetryWebSocketServer: Max connections reached, rejecting" << clientIp;
        socket->close(QWebSocketProtocol::CloseCodePolicyViolated, "Max connections reached");
        socket->deleteLater();
        return;
    }

    // Create client info
    WebSocketClient client;
    client.socket = socket;
    client.authenticated = false;
    client.connectedAt = QDateTime::currentDateTime();
    client.lastPing = QDateTime::currentDateTime();
    client.clientIp = clientIp;

    m_clients[socket] = client;

    locker.unlock();

    // Connect socket signals
    connect(socket, &QWebSocket::textMessageReceived,
            this, &TelemetryWebSocketServer::onTextMessageReceived);
    connect(socket, &QWebSocket::binaryMessageReceived,
            this, &TelemetryWebSocketServer::onBinaryMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &TelemetryWebSocketServer::onClientDisconnected);

    qInfo() << "TelemetryWebSocketServer: New connection from" << clientIp
            << "- Total clients:" << m_clients.size();

    emit clientConnected(clientIp);

    // Send welcome message
    QJsonObject welcome;
    welcome["type"] = "welcome";
    welcome["message"] = "RCWS Telemetry Server";
    welcome["version"] = "1.0.0";
    welcome["requiresAuth"] = true;
    sendToClient(socket, welcome);
}

void TelemetryWebSocketServer::onClientDisconnected()
{
    QWebSocket* socket = qobject_cast<QWebSocket*>(sender());
    if (!socket) {
        return;
    }

    QMutexLocker locker(&m_clientsMutex);

    if (m_clients.contains(socket)) {
        WebSocketClient client = m_clients[socket];
        m_clients.remove(socket);

        locker.unlock();

        qInfo() << "TelemetryWebSocketServer: Client disconnected:" << client.username
                << "from" << client.clientIp
                << "- Messages sent:" << client.messagesSent;

        emit clientDisconnected(client.username, client.clientIp);
    }

    socket->deleteLater();
}

// ============================================================================
// MESSAGE HANDLING
// ============================================================================

void TelemetryWebSocketServer::onTextMessageReceived(const QString& message)
{
    QWebSocket* socket = qobject_cast<QWebSocket*>(sender());
    if (!socket) {
        return;
    }

    // Parse JSON message
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        sendError(socket, "Invalid JSON format");
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    // Handle different message types
    if (type == "auth") {
        handleAuthMessage(socket, obj);
    } else if (type == "subscribe") {
        handleSubscribeMessage(socket, obj);
    } else if (type == "unsubscribe") {
        handleUnsubscribeMessage(socket, obj);
    } else if (type == "ping") {
        handlePingMessage(socket, obj);
    } else {
        sendError(socket, "Unknown message type: " + type);
    }
}

void TelemetryWebSocketServer::onBinaryMessageReceived(const QByteArray& message)
{
    // Binary messages not implemented yet
    QWebSocket* socket = qobject_cast<QWebSocket*>(sender());
    if (socket) {
        sendError(socket, "Binary messages not supported");
    }
}

// ============================================================================
// MESSAGE HANDLERS
// ============================================================================

void TelemetryWebSocketServer::handleAuthMessage(QWebSocket* socket, const QJsonObject& message)
{
    WebSocketClient* client = getClient(socket);
    if (!client) {
        return;
    }

    QString token = message["token"].toString();

    if (token.isEmpty()) {
        sendError(socket, "Token required");
        return;
    }

    // Validate token
    if (!m_authService->isTokenValid(token)) {
        sendError(socket, "Invalid or expired token");

        QJsonObject response;
        response["type"] = "auth_failed";
        response["reason"] = "Invalid or expired token";
        sendToClient(socket, response);

        // Disconnect after failed auth
        socket->close(QWebSocketProtocol::CloseCodePolicyViolated, "Authentication failed");
        return;
    }

    // Get user info from token
    TokenPayload payload = m_authService->validateToken(token);

    // Update client info
    QMutexLocker locker(&m_clientsMutex);
    client->authenticated = true;
    client->username = payload.username;
    client->role = payload.role;
    locker.unlock();

    qInfo() << "TelemetryWebSocketServer: Client authenticated:" << client->username
            << "Role:" << static_cast<int>(client->role)
            << "from" << client->clientIp;

    emit clientAuthenticated(client->username, client->clientIp);

    // Send success response
    QJsonObject response;
    response["type"] = "auth_success";
    response["username"] = payload.username;
    response["role"] = static_cast<int>(payload.role);
    response["message"] = "Authentication successful";
    sendToClient(socket, response);
}

void TelemetryWebSocketServer::handleSubscribeMessage(QWebSocket* socket, const QJsonObject& message)
{
    WebSocketClient* client = getClient(socket);
    if (!client) {
        return;
    }

    if (!client->authenticated) {
        sendError(socket, "Authentication required");
        return;
    }

    QJsonArray categoriesArray = message["categories"].toArray();
    if (categoriesArray.isEmpty()) {
        sendError(socket, "Categories array required");
        return;
    }

    QMutexLocker locker(&m_clientsMutex);

    // Add categories to subscription
    for (const QJsonValue& val : categoriesArray) {
        QString category = val.toString().toLower();
        client->subscribedCategories.insert(category);
    }

    locker.unlock();

    qInfo() << "TelemetryWebSocketServer: Client" << client->username
            << "subscribed to:" << client->subscribedCategories;

    // Send confirmation
    QJsonObject response;
    response["type"] = "subscribe_success";
    QJsonArray subscribed;
    for (const QString& cat : client->subscribedCategories) {
        subscribed.append(cat);
    }
    response["categories"] = subscribed;
    sendToClient(socket, response);
}

void TelemetryWebSocketServer::handleUnsubscribeMessage(QWebSocket* socket, const QJsonObject& message)
{
    WebSocketClient* client = getClient(socket);
    if (!client) {
        return;
    }

    if (!client->authenticated) {
        sendError(socket, "Authentication required");
        return;
    }

    QJsonArray categoriesArray = message["categories"].toArray();

    QMutexLocker locker(&m_clientsMutex);

    if (categoriesArray.isEmpty()) {
        // Unsubscribe from all
        client->subscribedCategories.clear();
    } else {
        // Unsubscribe from specific categories
        for (const QJsonValue& val : categoriesArray) {
            QString category = val.toString().toLower();
            client->subscribedCategories.remove(category);
        }
    }

    locker.unlock();

    // Send confirmation
    QJsonObject response;
    response["type"] = "unsubscribe_success";
    sendToClient(socket, response);
}

void TelemetryWebSocketServer::handlePingMessage(QWebSocket* socket, const QJsonObject& message)
{
    WebSocketClient* client = getClient(socket);
    if (!client) {
        return;
    }

    QMutexLocker locker(&m_clientsMutex);
    client->lastPing = QDateTime::currentDateTime();
    locker.unlock();

    // Send pong
    QJsonObject response;
    response["type"] = "pong";
    response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    sendToClient(socket, response);
}

// ============================================================================
// BROADCASTING
// ============================================================================

void TelemetryWebSocketServer::onUpdateTimerTimeout()
{
    broadcastTelemetryUpdate();
}

void TelemetryWebSocketServer::broadcastTelemetryUpdate()
{
    if (!m_stateModel) {
        return;
    }

    const SystemStateData& state = m_stateModel->data();

    QMutexLocker locker(&m_clientsMutex);

    int broadcastCount = 0;
    int totalBytes = 0;

    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        WebSocketClient& client = it.value();

        // Only send to authenticated clients with subscriptions
        if (!client.authenticated || client.subscribedCategories.isEmpty()) {
            continue;
        }

        // Create telemetry message filtered by subscriptions
        QJsonObject telemetryData = stateToJson(state, client.subscribedCategories);

        QJsonObject message;
        message["type"] = "telemetry";
        message["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        message["data"] = telemetryData;

        QJsonDocument doc(message);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        client.socket->sendTextMessage(QString::fromUtf8(jsonData));
        client.messagesSent++;

        broadcastCount++;
        totalBytes += jsonData.size();
    }

    locker.unlock();

    if (broadcastCount > 0) {
        m_totalMessagesSent += broadcastCount;
        m_totalBytesSent += totalBytes;

        emit telemetryBroadcast(broadcastCount, totalBytes);
    }
}

void TelemetryWebSocketServer::onHeartbeatTimerTimeout()
{
    disconnectInactiveClients();
}

void TelemetryWebSocketServer::disconnectInactiveClients()
{
    QMutexLocker locker(&m_clientsMutex);

    QDateTime now = QDateTime::currentDateTime();
    int timeoutSec = m_config.heartbeatIntervalSec * 3;  // 3x heartbeat interval

    QList<QWebSocket*> toDisconnect;

    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        const WebSocketClient& client = it.value();

        if (client.authenticated) {
            qint64 secondsSinceLastPing = client.lastPing.secsTo(now);

            if (secondsSinceLastPing > timeoutSec) {
                qWarning() << "TelemetryWebSocketServer: Client" << client.username
                          << "inactive for" << secondsSinceLastPing << "seconds, disconnecting";
                toDisconnect.append(it.key());
            }
        }
    }

    locker.unlock();

    // Disconnect inactive clients
    for (QWebSocket* socket : toDisconnect) {
        socket->close(QWebSocketProtocol::CloseCodeGoingAway, "Inactive timeout");
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

void TelemetryWebSocketServer::sendToClient(QWebSocket* socket, const QJsonObject& message)
{
    if (!socket) {
        return;
    }

    QJsonDocument doc(message);
    socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void TelemetryWebSocketServer::broadcastToAll(const QJsonObject& message)
{
    QJsonDocument doc(message);
    QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    QMutexLocker locker(&m_clientsMutex);

    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        if (it.value().authenticated) {
            it.value().socket->sendTextMessage(jsonStr);
        }
    }
}

WebSocketClient* TelemetryWebSocketServer::getClient(QWebSocket* socket)
{
    QMutexLocker locker(&m_clientsMutex);

    if (m_clients.contains(socket)) {
        return &m_clients[socket];
    }

    return nullptr;
}

void TelemetryWebSocketServer::sendError(QWebSocket* socket, const QString& error)
{
    QJsonObject errorObj;
    errorObj["type"] = "error";
    errorObj["message"] = error;
    errorObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    sendToClient(socket, errorObj);
}

QString TelemetryWebSocketServer::getClientIp(QWebSocket* socket) const
{
    if (!socket) {
        return "unknown";
    }

    return socket->peerAddress().toString();
}

QJsonObject TelemetryWebSocketServer::stateToJson(const SystemStateData& state,
                                                   const QSet<QString>& categories) const
{
    QJsonObject data;

    bool sendAll = categories.contains("all");

    // Gimbal data
    if (sendAll || categories.contains("gimbal")) {
        QJsonObject gimbal;
        gimbal["azimuth"] = state.gimbalAz;
        gimbal["elevation"] = state.gimbalEl;
        gimbal["azimuthSpeed"] = static_cast<double>(state.azimuthSpeed);
        gimbal["elevationSpeed"] = static_cast<double>(state.elevationSpeed);
        gimbal["opMode"] = static_cast<int>(state.opMode);
        gimbal["motionMode"] = static_cast<int>(state.motionMode);
        data["gimbal"] = gimbal;
    }

    // IMU data
    if (sendAll || categories.contains("imu")) {
        QJsonObject imu;
        imu["roll"] = state.imuRollDeg;
        imu["pitch"] = state.imuPitchDeg;
        imu["yaw"] = state.imuYawDeg;
        imu["temperature"] = state.temperature;
        data["imu"] = imu;
    }

    // Tracking data
    if (sendAll || categories.contains("tracking")) {
        QJsonObject tracking;
        tracking["active"] = state.trackingActive;
        tracking["phase"] = static_cast<int>(state.currentTrackingPhase);
        tracking["hasTarget"] = state.trackerHasValidTarget;
        tracking["targetAz"] = state.targetAz;
        tracking["targetEl"] = state.targetEl;
        data["tracking"] = tracking;
    }

    // Weapon status
    if (sendAll || categories.contains("weapon")) {
        QJsonObject weapon;
        weapon["armed"] = state.gunArmed;
        weapon["ready"] = state.isReady();
        weapon["ammoLoaded"] = state.ammoLoaded;
        weapon["fireMode"] = static_cast<int>(state.fireMode);
        weapon["ammunitionLevel"] = state.stationAmmunitionLevel;
        data["weapon"] = weapon;
    }

    // Camera status
    if (sendAll || categories.contains("camera")) {
        QJsonObject camera;
        camera["activeCamera"] = state.activeCameraIsDay ? "day" : "night";
        camera["dayZoom"] = state.dayZoomPosition;
        camera["nightZoom"] = state.nightZoomPosition;
        camera["dayHFOV"] = state.dayCurrentHFOV;
        camera["nightHFOV"] = state.nightCurrentHFOV;
        data["camera"] = camera;
    }

    // Sensor data
    if (sendAll || categories.contains("sensor")) {
        QJsonObject sensor;
        sensor["lrfDistance"] = state.lrfDistance;
        sensor["lrfStatus"] = state.lrfSystemStatus;
        data["sensor"] = sensor;
    }

    // Ballistic data
    if (sendAll || categories.contains("ballistic")) {
        QJsonObject ballistic;
        ballistic["zeroingActive"] = state.zeroingModeActive;
        ballistic["zeroingAzOffset"] = state.zeroingAzimuthOffset;
        ballistic["zeroingElOffset"] = state.zeroingElevationOffset;
        ballistic["windageActive"] = state.windageModeActive;
        ballistic["windSpeed"] = state.windageSpeedKnots;
        data["ballistic"] = ballistic;
    }

    // Device status
    if (sendAll || categories.contains("device")) {
        QJsonObject device;
        device["azMotorTemp"] = state.azMotorTemp;
        device["azDriverTemp"] = state.azDriverTemp;
        device["elMotorTemp"] = state.elMotorTemp;
        device["elDriverTemp"] = state.elDriverTemp;
        device["stationTemp"] = state.stationTemperature;
        device["emergencyStop"] = state.emergencyStopActive;
        data["device"] = device;
    }

    return data;
}

bool TelemetryWebSocketServer::shouldSendCategory(const WebSocketClient& client,
                                                  const QString& category) const
{
    return client.subscribedCategories.contains("all") ||
           client.subscribedCategories.contains(category);
}
