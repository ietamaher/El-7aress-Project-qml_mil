#ifndef TELEMETRYCONFIG_H
#define TELEMETRYCONFIG_H

/**
 * @file TelemetryConfig.h
 * @brief Configuration structures for telemetry system
 *
 * This file contains all configuration structures for the RCWS telemetry system
 * including HTTP server, WebSocket server, TLS/SSL, and general telemetry settings.
 *
 * @author RCWS Development Team
 * @date 2025
 */

#include <QString>
#include <QStringList>
#include <QHostAddress>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslConfiguration>

/**
 * @brief TLS/SSL security configuration
 */
struct TlsConfig {
    bool enabled;                   ///< Enable TLS/SSL encryption
    QString certificatePath;        ///< Path to SSL certificate file (.crt)
    QString privateKeyPath;         ///< Path to private key file (.key)
    QString caPath;                 ///< Path to CA certificate (for client verification)
    bool requireClientCert;         ///< Require client certificate authentication
    QSsl::SslProtocol protocol;     ///< TLS protocol version
    QString cipherSuites;           ///< Allowed cipher suites (comma-separated)

    TlsConfig()
        : enabled(false)
        , requireClientCert(false)
        , protocol(QSsl::TlsV1_2OrLater)
        , cipherSuites("ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256")
    {}

    /**
     * @brief Load SSL configuration from files
     */
    bool loadSslConfiguration(QSslConfiguration& sslConfig) const;
};

/**
 * @brief HTTP API server configuration
 */
struct HttpApiConfig {
    bool enabled;                   ///< Enable HTTP API server
    QString bindAddress;            ///< IP address to bind to (0.0.0.0 = all interfaces)
    quint16 port;                   ///< TCP port (default: 8080, HTTPS: 8443)
    int maxConnections;             ///< Maximum concurrent connections
    int requestTimeoutSec;          ///< Request timeout in seconds
    bool enableCors;                ///< Enable CORS headers for web clients
    QStringList corsOrigins;        ///< Allowed CORS origins
    int rateLimitPerMinute;         ///< Rate limit (requests per minute per IP)

    HttpApiConfig()
        : enabled(true)
        , bindAddress("0.0.0.0")
        , port(8080)
        , maxConnections(100)
        , requestTimeoutSec(30)
        , enableCors(true)
        , corsOrigins({"*"})  // Allow all origins (change in production!)
        , rateLimitPerMinute(60)
    {}
};

/**
 * @brief WebSocket server configuration
 */
struct WebSocketConfig {
    bool enabled;                   ///< Enable WebSocket server
    QString bindAddress;            ///< IP address to bind to
    quint16 port;                   ///< TCP port (default: 8081, WSS: 8444)
    int maxConnections;             ///< Maximum concurrent WebSocket connections
    int heartbeatIntervalSec;       ///< Send ping every N seconds
    int maxMessageSizeKB;           ///< Maximum message size in KB
    int updateRateHz;               ///< Telemetry update rate (default: 10 Hz)
    bool enableCompression;         ///< Enable WebSocket compression

    WebSocketConfig()
        : enabled(true)
        , bindAddress("0.0.0.0")
        , port(8081)
        , maxConnections(50)
        , heartbeatIntervalSec(30)
        , maxMessageSizeKB(1024)
        , updateRateHz(10)
        , enableCompression(true)
    {}
};

/**
 * @brief Data export configuration
 */
struct ExportConfig {
    bool enableCsvExport;           ///< Allow CSV export
    QString exportDirectory;        ///< Directory for exported files
    int maxExportRangeDays;         ///< Maximum time range for single export (days)
    int maxExportSizeMB;            ///< Maximum export file size (MB)
    bool requireAuthentication;     ///< Require authentication for exports

    ExportConfig()
        : enableCsvExport(true)
        , exportDirectory("./exports")
        , maxExportRangeDays(30)
        , maxExportSizeMB(100)
        , requireAuthentication(true)
    {}
};

/**
 * @brief Complete telemetry system configuration
 */
struct TelemetryConfig {
    HttpApiConfig httpApi;          ///< HTTP REST API settings
    WebSocketConfig webSocket;      ///< WebSocket server settings
    TlsConfig tls;                  ///< TLS/SSL security settings
    ExportConfig exportSettings;    ///< Data export settings

    /**
     * @brief Load configuration from JSON file
     */
    bool loadFromFile(const QString& filePath);

    /**
     * @brief Save configuration to JSON file
     */
    bool saveToFile(const QString& filePath) const;

    /**
     * @brief Validate configuration
     * @return Error message if invalid, empty string if valid
     */
    QString validate() const;
};

#endif // TELEMETRYCONFIG_H
