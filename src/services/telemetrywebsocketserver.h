#ifndef TELEMETRYWEBSOCKETSERVER_H
#define TELEMETRYWEBSOCKETSERVER_H

/**
 * @file TelemetryWebSocketServer.h
 * @brief WebSocket server for real-time telemetry streaming
 *
 * This service provides WebSocket-based real-time streaming of telemetry data
 * to web clients. It supports client subscriptions to specific data categories,
 * JWT authentication, and configurable update rates.
 *
 * FEATURES:
 * • WebSocket server for bidirectional communication
 * • JWT token authentication
 * • Selective data subscription (subscribe to specific categories)
 * • Configurable update rate (default: 10 Hz)
 * • Heartbeat/ping mechanism
 * • JSON message format
 * • Optional binary compression
 * • Connection management
 * • Bandwidth throttling
 *
 * CLIENT PROTOCOL:
 * 1. Connect to ws://host:8444/telemetry
 * 2. Send authentication message:
 *    {"type": "auth", "token": "JWT_TOKEN_HERE"}
 * 3. Subscribe to categories:
 *    {"type": "subscribe", "categories": ["gimbal", "imu", "tracking"]}
 *    or {"type": "subscribe", "categories": ["all"]}
 * 4. Receive telemetry updates at configured rate
 * 5. Send ping to keep connection alive (optional)
 *
 * MESSAGE TYPES:
 * • Client → Server:
 *   - auth: Authenticate with JWT token
 *   - subscribe: Subscribe to data categories
 *   - unsubscribe: Unsubscribe from categories
 *   - ping: Keep-alive ping
 *
 * • Server → Client:
 *   - auth_success: Authentication successful
 *   - auth_failed: Authentication failed
 *   - telemetry: Telemetry data update
 *   - pong: Response to ping
 *   - error: Error message
 *
 * @author RCWS Development Team
 * @date 2025
 */

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QMutex>
#include "telemetryconfig.h"
#include "telemetryauthservice.h"
#include "models/domain/systemstatedata.h"

// Forward declaration
class SystemStateModel;

/**
 * @brief Client connection information
 */
struct WebSocketClient {
    QWebSocket* socket;
    QString username;
    UserRole role;
    bool authenticated;
    QSet<QString> subscribedCategories;  // "all", "gimbal", "imu", "tracking", etc.
    QDateTime connectedAt;
    QDateTime lastPing;
    int messagesSent;
    QString clientIp;

    WebSocketClient()
        : socket(nullptr)
        , role(UserRole::Viewer)
        , authenticated(false)
        , messagesSent(0)
    {}
};

/**
 * @brief WebSocket server for real-time telemetry streaming
 */
class TelemetryWebSocketServer : public QObject
{
    Q_OBJECT

public:
    explicit TelemetryWebSocketServer(QObject *parent = nullptr);
    explicit TelemetryWebSocketServer(const WebSocketConfig& config,
                                      TelemetryAuthService* authService,
                                      SystemStateModel* stateModel,
                                      QObject *parent = nullptr);
    ~TelemetryWebSocketServer();

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    /**
     * @brief Start the WebSocket server
     * @return true if server started successfully
     */
    bool start();

    /**
     * @brief Stop the WebSocket server
     */
    void stop();

    /**
     * @brief Check if server is running
     */
    bool isRunning() const { return m_isRunning; }

    /**
     * @brief Get server URL
     */
    QString getServerUrl() const;

    /**
     * @brief Get number of connected clients
     */
    int getClientCount() const;

    /**
     * @brief Get number of authenticated clients
     */
    int getAuthenticatedClientCount() const;

    // ========================================================================
    // Configuration
    // ========================================================================

    void setConfig(const WebSocketConfig& config);
    WebSocketConfig getConfig() const { return m_config; }

    /**
     * @brief Set update rate in Hz (how often to send telemetry updates)
     */
    void setUpdateRate(int hz);

    // ========================================================================
    // Broadcasting
    // ========================================================================

    /**
     * @brief Broadcast telemetry update to all subscribed clients
     * This is called automatically by the update timer
     */
    void broadcastTelemetryUpdate();

    /**
     * @brief Broadcast message to specific client
     */
    void sendToClient(QWebSocket* socket, const QJsonObject& message);

    /**
     * @brief Broadcast message to all authenticated clients
     */
    void broadcastToAll(const QJsonObject& message);

signals:
    /**
     * @brief Emitted when server starts
     */
    void serverStarted(const QString& url);

    /**
     * @brief Emitted when server stops
     */
    void serverStopped();

    /**
     * @brief Emitted when new client connects
     */
    void clientConnected(const QString& clientIp);

    /**
     * @brief Emitted when client authenticates
     */
    void clientAuthenticated(const QString& username, const QString& clientIp);

    /**
     * @brief Emitted when client disconnects
     */
    void clientDisconnected(const QString& username, const QString& clientIp);

    /**
     * @brief Emitted on telemetry update broadcast
     */
    void telemetryBroadcast(int clientCount, int messageSize);

private slots:
    /**
     * @brief Handle new WebSocket connection
     */
    void onNewConnection();

    /**
     * @brief Handle client disconnection
     */
    void onClientDisconnected();

    /**
     * @brief Handle text message from client
     */
    void onTextMessageReceived(const QString& message);

    /**
     * @brief Handle binary message from client
     */
    void onBinaryMessageReceived(const QByteArray& message);

    /**
     * @brief Periodic telemetry update broadcast
     */
    void onUpdateTimerTimeout();

    /**
     * @brief Periodic heartbeat check
     */
    void onHeartbeatTimerTimeout();

private:
    // ========================================================================
    // Message Handlers
    // ========================================================================

    void handleAuthMessage(QWebSocket* socket, const QJsonObject& message);
    void handleSubscribeMessage(QWebSocket* socket, const QJsonObject& message);
    void handleUnsubscribeMessage(QWebSocket* socket, const QJsonObject& message);
    void handlePingMessage(QWebSocket* socket, const QJsonObject& message);

    // ========================================================================
    // Helper Methods
    // ========================================================================

    /**
     * @brief Get client info by socket
     */
    WebSocketClient* getClient(QWebSocket* socket);

    /**
     * @brief Remove client from tracking
     */
    void removeClient(QWebSocket* socket);

    /**
     * @brief Send error message to client
     */
    void sendError(QWebSocket* socket, const QString& error);

    /**
     * @brief Get client IP address
     */
    QString getClientIp(QWebSocket* socket) const;

    /**
     * @brief Create telemetry JSON for subscribed categories
     */
    QJsonObject createTelemetryMessage(const WebSocketClient& client) const;

    /**
     * @brief Check if client should receive category data
     */
    bool shouldSendCategory(const WebSocketClient& client, const QString& category) const;

    /**
     * @brief Convert SystemStateData to JSON (filtered by subscription)
     */
    QJsonObject stateToJson(const SystemStateData& state, const QSet<QString>& categories) const;

    /**
     * @brief Disconnect inactive clients (no ping for N seconds)
     */
    void disconnectInactiveClients();

    // ========================================================================
    // Member Variables
    // ========================================================================

    QWebSocketServer* m_server;
    WebSocketConfig m_config;
    TelemetryAuthService* m_authService;
    SystemStateModel* m_stateModel;

    bool m_isRunning;

    // Client management
    QMap<QWebSocket*, WebSocketClient> m_clients;
    mutable QMutex m_clientsMutex;

    // Update timer (broadcasts telemetry at configured rate)
    QTimer* m_updateTimer;

    // Heartbeat timer (checks for inactive clients)
    QTimer* m_heartbeatTimer;

    // Statistics
    qint64 m_totalMessagesSent;
    qint64 m_totalBytesSent;
};

#endif // TELEMETRYWEBSOCKETSERVER_H
