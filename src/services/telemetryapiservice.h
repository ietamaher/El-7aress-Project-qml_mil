#ifndef TELEMETRYAPISERVICE_H
#define TELEMETRYAPISERVICE_H

/**
 * @file TelemetryApiService.h
 * @brief Comprehensive REST API service for RCWS telemetry system
 *
 * This service provides a complete RESTful API for accessing real-time and historical
 * telemetry data from the RCWS system. It integrates with SystemDataLogger and
 * SystemStateModel to expose all system parameters via HTTP endpoints.
 *
 * FEATURES:
 * • JWT authentication and authorization
 * • Complete REST API for all telemetry categories
 * • Historical data queries with time-range filtering
 * • CSV export endpoints
 * • System health and statistics
 * • CORS support for web clients
 * • Rate limiting and IP whitelisting
 * • Audit logging for all requests
 * • Optional TLS/SSL encryption
 *
 * API ENDPOINTS:
 * Authentication:
 *   POST   /api/auth/login         - Authenticate and get JWT token
 *   POST   /api/auth/refresh       - Refresh JWT token
 *   POST   /api/auth/logout        - Logout (revoke token)
 *
 * Current State:
 *   GET    /api/telemetry/current  - All current telemetry data
 *   GET    /api/status             - System status summary
 *
 * Historical Data:
 *   GET    /api/telemetry/history/gimbal    - Gimbal motion history
 *   GET    /api/telemetry/history/imu       - IMU data history
 *   GET    /api/telemetry/history/tracking  - Tracking data history
 *   GET    /api/telemetry/history/weapon    - Weapon status history
 *   GET    /api/telemetry/history/camera    - Camera status history
 *   GET    /api/telemetry/history/sensor    - Sensor data history
 *   GET    /api/telemetry/history/ballistic - Ballistic data history
 *   GET    /api/telemetry/history/device    - Device status history
 *
 * Statistics:
 *   GET    /api/telemetry/stats/memory      - Memory usage statistics
 *   GET    /api/telemetry/stats/samples     - Sample counts per category
 *   GET    /api/telemetry/stats/timerange   - Available time ranges
 *
 * Export:
 *   GET    /api/telemetry/export/csv        - Export category data to CSV
 *
 * System:
 *   GET    /api/health                      - System health check
 *   GET    /api/version                     - API version info
 *
 * User Management (Admin only):
 *   GET    /api/users                       - List all users
 *   POST   /api/users                       - Create new user
 *   DELETE /api/users/:username             - Delete user
 *   PUT    /api/users/:username/password    - Change password
 *
 * @author RCWS Development Team
 * @date 2025
 */

#include <QObject>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QDateTime>
#include "telemetryconfig.h"
#include "telemetryauthservice.h"
#include "logger/systemdatalogger.h"
#include "models/domain/systemstatemodel.h"

/**
 * @brief Rate limiting tracker per IP address
 */
struct RateLimitInfo {
    int requestCount;
    QDateTime windowStart;

    RateLimitInfo() : requestCount(0) {}
};

/**
 * @brief Telemetry API Service - Complete REST API server
 */
class TelemetryApiService : public QObject
{
    Q_OBJECT

public:
    explicit TelemetryApiService(QObject *parent = nullptr);
    explicit TelemetryApiService(const TelemetryConfig& config,
                                SystemDataLogger* logger,
                                SystemStateModel* stateModel,
                                TelemetryAuthService* authService,
                                QObject *parent = nullptr);
    ~TelemetryApiService();

    // ========================================================================
    // Lifecycle Management
    // ========================================================================

    /**
     * @brief Start the HTTP server
     * @return true if server started successfully
     */
    bool start();

    /**
     * @brief Stop the HTTP server
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

    // ========================================================================
    // Configuration
    // ========================================================================

    void setConfig(const TelemetryConfig& config);
    TelemetryConfig getConfig() const { return m_config; }

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
     * @brief Emitted on API request
     */
    void requestReceived(const QString& method, const QString& path, const QString& clientIp);

    /**
     * @brief Emitted on rate limit exceeded
     */
    void rateLimitExceeded(const QString& clientIp, const QString& path);

private:
    // ========================================================================
    // Endpoint Registration
    // ========================================================================

    void registerEndpoints();

    // Authentication endpoints
    void registerAuthEndpoints();

    // Telemetry endpoints
    void registerTelemetryEndpoints();

    // Statistics endpoints
    void registerStatisticsEndpoints();

    // Export endpoints
    void registerExportEndpoints();

    // System endpoints
    void registerSystemEndpoints();

    // User management endpoints
    void registerUserEndpoints();

    // ========================================================================
    // Authentication Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleLogin(const QHttpServerRequest &request);
    QHttpServerResponse handleRefresh(const QHttpServerRequest &request);
    QHttpServerResponse handleLogout(const QHttpServerRequest &request);

    // ========================================================================
    // Telemetry Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleGetCurrent(const QHttpServerRequest &request);
    QHttpServerResponse handleGetStatus(const QHttpServerRequest &request);

    // Historical data handlers
    QHttpServerResponse handleGetGimbalHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetImuHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetTrackingHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetWeaponHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetCameraHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetSensorHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetBallisticHistory(const QHttpServerRequest &request);
    QHttpServerResponse handleGetDeviceHistory(const QHttpServerRequest &request);

    // ========================================================================
    // Statistics Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleGetMemoryStats(const QHttpServerRequest &request);
    QHttpServerResponse handleGetSampleStats(const QHttpServerRequest &request);
    QHttpServerResponse handleGetTimeRangeStats(const QHttpServerRequest &request);

    // ========================================================================
    // Export Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleExportCsv(const QHttpServerRequest &request);

    // ========================================================================
    // System Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleHealthCheck(const QHttpServerRequest &request);
    QHttpServerResponse handleGetVersion(const QHttpServerRequest &request);

    // ========================================================================
    // User Management Endpoint Handlers
    // ========================================================================

    QHttpServerResponse handleListUsers(const QHttpServerRequest &request);
    QHttpServerResponse handleCreateUser(const QHttpServerRequest &request);
    QHttpServerResponse handleDeleteUser(const QHttpServerRequest &request, const QString& username);
    QHttpServerResponse handleChangePassword(const QHttpServerRequest &request, const QString& username);

    // ========================================================================
    // Helper Methods
    // ========================================================================

    /**
     * @brief Extract JWT token from request headers
     */
    QString extractToken(const QHttpServerRequest &request) const;

    /**
     * @brief Check authentication and return error response if invalid
     */
    QHttpServerResponse checkAuthentication(const QHttpServerRequest &request,
                                           Permission requiredPermission = Permission::ReadTelemetry);

    /**
     * @brief Add CORS headers to response
     */
    void addCorsHeaders(QHttpServerResponse& response) const;

    /**
     * @brief Create JSON error response
     */
    QHttpServerResponse createErrorResponse(const QString& error, int statusCode = 400) const;

    /**
     * @brief Create JSON success response
     */
    QHttpServerResponse createJsonResponse(const QJsonObject& data, int statusCode = 200) const;
    QHttpServerResponse createJsonResponse(const QJsonArray& data, int statusCode = 200) const;

    /**
     * @brief Parse time range from query parameters
     */
    bool parseTimeRange(const QHttpServerRequest &request,
                       QDateTime& startTime, QDateTime& endTime,
                       QString& errorMsg) const;

    /**
     * @brief Check rate limit for client IP
     */
    bool checkRateLimit(const QString& clientIp);

    /**
     * @brief Get client IP address from request
     */
    QString getClientIp(const QHttpServerRequest &request) const;

    /**
     * @brief Log API request
     */
    void logRequest(const QString& method, const QString& path,
                   const QString& clientIp, const QString& username,
                   int statusCode);

    // ========================================================================
    // Data Conversion Helpers
    // ========================================================================

    QJsonObject gimbalMotionToJson(const GimbalMotionData& data) const;
    QJsonObject imuDataToJson(const ImuDataPoint& data) const;
    QJsonObject trackingDataToJson(const TrackingDataPoint& data) const;
    QJsonObject weaponStatusToJson(const WeaponStatusData& data) const;
    QJsonObject cameraStatusToJson(const CameraStatusData& data) const;
    QJsonObject sensorDataToJson(const SensorDataPoint& data) const;
    QJsonObject ballisticDataToJson(const BallisticDataPoint& data) const;
    QJsonObject deviceStatusToJson(const DeviceStatusData& data) const;

    QJsonObject systemStateToJson(const SystemStateData& state) const;

    // ========================================================================
    // Member Variables
    // ========================================================================

    QHttpServer* m_server;
    TelemetryConfig m_config;
    SystemDataLogger* m_dataLogger;
    SystemStateModel* m_stateModel;
    TelemetryAuthService* m_authService;

    bool m_isRunning;

    // Rate limiting
    QMap<QString, RateLimitInfo> m_rateLimits;
    QMutex m_rateLimitMutex;
};

#endif // TELEMETRYAPISERVICE_H
