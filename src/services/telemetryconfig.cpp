#include "telemetryconfig.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSslCertificate>
#include <QSslKey>

// ============================================================================
// TLS CONFIG
// ============================================================================

bool TlsConfig::loadSslConfiguration(QSslConfiguration& sslConfig) const
{
    if (!enabled) {
        return true;  // TLS disabled, no configuration needed
    }

    // Load certificate
    QFile certFile(certificatePath);
    if (!certFile.open(QIODevice::ReadOnly)) {
        qCritical() << "TelemetryConfig: Failed to open certificate file:" << certificatePath;
        return false;
    }

    QSslCertificate certificate(&certFile, QSsl::Pem);
    certFile.close();

    if (certificate.isNull()) {
        qCritical() << "TelemetryConfig: Invalid certificate:" << certificatePath;
        return false;
    }

    // Load private key
    QFile keyFile(privateKeyPath);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qCritical() << "TelemetryConfig: Failed to open private key file:" << privateKeyPath;
        return false;
    }

    QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    keyFile.close();

    if (privateKey.isNull()) {
        qCritical() << "TelemetryConfig: Invalid private key:" << privateKeyPath;
        return false;
    }

    // Configure SSL
    sslConfig.setLocalCertificate(certificate);
    sslConfig.setPrivateKey(privateKey);
    sslConfig.setProtocol(protocol);

    // Load CA certificate if client verification is required
    if (requireClientCert && !caPath.isEmpty()) {
        QFile caFile(caPath);
        if (caFile.open(QIODevice::ReadOnly)) {
            QList<QSslCertificate> caCerts = QSslCertificate::fromDevice(&caFile, QSsl::Pem);
            sslConfig.setCaCertificates(caCerts);
            sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
            caFile.close();
        } else {
            qWarning() << "TelemetryConfig: Failed to load CA certificate:" << caPath;
        }
    } else {
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    }

    qInfo() << "TelemetryConfig: SSL configuration loaded successfully";
    qInfo() << "  Protocol:" << protocol;
    qInfo() << "  Client verification:" << (requireClientCert ? "Required" : "None");

    return true;
}

// ============================================================================
// TELEMETRY CONFIG
// ============================================================================

bool TelemetryConfig::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "TelemetryConfig: Failed to open configuration file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "TelemetryConfig: Invalid JSON format in configuration file";
        return false;
    }

    QJsonObject root = doc.object();

    // Load HTTP API config
    if (root.contains("httpApi")) {
        QJsonObject httpObj = root["httpApi"].toObject();
        httpApi.enabled = httpObj["enabled"].toBool(true);
        httpApi.bindAddress = httpObj["bindAddress"].toString("0.0.0.0");
        httpApi.port = httpObj["port"].toInt(8080);
        httpApi.maxConnections = httpObj["maxConnections"].toInt(100);
        httpApi.requestTimeoutSec = httpObj["requestTimeoutSec"].toInt(30);
        httpApi.enableCors = httpObj["enableCors"].toBool(true);
        httpApi.rateLimitPerMinute = httpObj["rateLimitPerMinute"].toInt(60);

        if (httpObj.contains("corsOrigins")) {
            httpApi.corsOrigins.clear();
            QJsonArray originsArray = httpObj["corsOrigins"].toArray();
            for (const QJsonValue& val : originsArray) {
                httpApi.corsOrigins.append(val.toString());
            }
        }
    }

    // Load WebSocket config
    if (root.contains("webSocket")) {
        QJsonObject wsObj = root["webSocket"].toObject();
        webSocket.enabled = wsObj["enabled"].toBool(true);
        webSocket.bindAddress = wsObj["bindAddress"].toString("0.0.0.0");
        webSocket.port = wsObj["port"].toInt(8081);
        webSocket.maxConnections = wsObj["maxConnections"].toInt(50);
        webSocket.heartbeatIntervalSec = wsObj["heartbeatIntervalSec"].toInt(30);
        webSocket.maxMessageSizeKB = wsObj["maxMessageSizeKB"].toInt(1024);
        webSocket.updateRateHz = wsObj["updateRateHz"].toInt(10);
        webSocket.enableCompression = wsObj["enableCompression"].toBool(true);
    }

    // Load TLS config
    if (root.contains("tls")) {
        QJsonObject tlsObj = root["tls"].toObject();
        tls.enabled = tlsObj["enabled"].toBool(false);
        tls.certificatePath = tlsObj["certificatePath"].toString();
        tls.privateKeyPath = tlsObj["privateKeyPath"].toString();
        tls.caPath = tlsObj["caPath"].toString();
        tls.requireClientCert = tlsObj["requireClientCert"].toBool(false);
        tls.cipherSuites = tlsObj["cipherSuites"].toString();

        QString protocolStr = tlsObj["protocol"].toString("TlsV1_2OrLater");
        if (protocolStr == "TlsV1_3OrLater") {
            tls.protocol = QSsl::TlsV1_3OrLater;
        } else {
            tls.protocol = QSsl::TlsV1_2OrLater;
        }
    }

    // Load Export config
    if (root.contains("export")) {
        QJsonObject exportObj = root["export"].toObject();
        exportSettings.enableCsvExport = exportObj["enableCsvExport"].toBool(true);
        exportSettings.exportDirectory = exportObj["exportDirectory"].toString("./exports");
        exportSettings.maxExportRangeDays = exportObj["maxExportRangeDays"].toInt(30);
        exportSettings.maxExportSizeMB = exportObj["maxExportSizeMB"].toInt(100);
        exportSettings.requireAuthentication = exportObj["requireAuthentication"].toBool(true);
    }

    qInfo() << "TelemetryConfig: Configuration loaded from" << filePath;
    return true;
}

bool TelemetryConfig::saveToFile(const QString& filePath) const
{
    QJsonObject root;

    // HTTP API config
    QJsonObject httpObj;
    httpObj["enabled"] = httpApi.enabled;
    httpObj["bindAddress"] = httpApi.bindAddress;
    httpObj["port"] = httpApi.port;
    httpObj["maxConnections"] = httpApi.maxConnections;
    httpObj["requestTimeoutSec"] = httpApi.requestTimeoutSec;
    httpObj["enableCors"] = httpApi.enableCors;
    httpObj["rateLimitPerMinute"] = httpApi.rateLimitPerMinute;

    QJsonArray originsArray;
    for (const QString& origin : httpApi.corsOrigins) {
        originsArray.append(origin);
    }
    httpObj["corsOrigins"] = originsArray;

    root["httpApi"] = httpObj;

    // WebSocket config
    QJsonObject wsObj;
    wsObj["enabled"] = webSocket.enabled;
    wsObj["bindAddress"] = webSocket.bindAddress;
    wsObj["port"] = webSocket.port;
    wsObj["maxConnections"] = webSocket.maxConnections;
    wsObj["heartbeatIntervalSec"] = webSocket.heartbeatIntervalSec;
    wsObj["maxMessageSizeKB"] = webSocket.maxMessageSizeKB;
    wsObj["updateRateHz"] = webSocket.updateRateHz;
    wsObj["enableCompression"] = webSocket.enableCompression;

    root["webSocket"] = wsObj;

    // TLS config
    QJsonObject tlsObj;
    tlsObj["enabled"] = tls.enabled;
    tlsObj["certificatePath"] = tls.certificatePath;
    tlsObj["privateKeyPath"] = tls.privateKeyPath;
    tlsObj["caPath"] = tls.caPath;
    tlsObj["requireClientCert"] = tls.requireClientCert;
    tlsObj["protocol"] = (tls.protocol == QSsl::TlsV1_3OrLater) ? "TlsV1_3OrLater" : "TlsV1_2OrLater";
    tlsObj["cipherSuites"] = tls.cipherSuites;

    root["tls"] = tlsObj;

    // Export config
    QJsonObject exportObj;
    exportObj["enableCsvExport"] = exportSettings.enableCsvExport;
    exportObj["exportDirectory"] = exportSettings.exportDirectory;
    exportObj["maxExportRangeDays"] = exportSettings.maxExportRangeDays;
    exportObj["maxExportSizeMB"] = exportSettings.maxExportSizeMB;
    exportObj["requireAuthentication"] = exportSettings.requireAuthentication;

    root["export"] = exportObj;

    // Write to file
    QJsonDocument doc(root);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "TelemetryConfig: Failed to write configuration file:" << filePath;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qInfo() << "TelemetryConfig: Configuration saved to" << filePath;
    return true;
}

QString TelemetryConfig::validate() const
{
    QStringList errors;

    // Validate HTTP API
    if (httpApi.enabled) {
        if (httpApi.port < 1 || httpApi.port > 65535) {
            errors << "Invalid HTTP API port";
        }
        if (httpApi.maxConnections < 1) {
            errors << "HTTP maxConnections must be > 0";
        }
    }

    // Validate WebSocket
    if (webSocket.enabled) {
        if (webSocket.port < 1 || webSocket.port > 65535) {
            errors << "Invalid WebSocket port";
        }
        if (webSocket.updateRateHz < 1 || webSocket.updateRateHz > 100) {
            errors << "WebSocket updateRateHz must be between 1-100 Hz";
        }
    }

    // Validate TLS
    if (tls.enabled) {
        if (tls.certificatePath.isEmpty()) {
            errors << "TLS certificate path is empty";
        }
        if (tls.privateKeyPath.isEmpty()) {
            errors << "TLS private key path is empty";
        }
    }

    // Validate Export
    if (exportSettings.enableCsvExport) {
        if (exportSettings.maxExportRangeDays < 1) {
            errors << "Export maxExportRangeDays must be > 0";
        }
    }

    if (errors.isEmpty()) {
        return QString();
    }

    return errors.join("; ");
}
