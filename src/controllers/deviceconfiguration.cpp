#include "deviceconfiguration.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// Initialize static members
DeviceConfiguration::VideoConfig DeviceConfiguration::m_video;
DeviceConfiguration::ImuConfig DeviceConfiguration::m_imu;
DeviceConfiguration::LrfConfig DeviceConfiguration::m_lrf;
DeviceConfiguration::PlcConfig DeviceConfiguration::m_plc21;
DeviceConfiguration::PlcConfig DeviceConfiguration::m_plc42;
DeviceConfiguration::ServoConfig DeviceConfiguration::m_servoAz;
DeviceConfiguration::ServoConfig DeviceConfiguration::m_servoEl;
DeviceConfiguration::ActuatorConfig DeviceConfiguration::m_actuator;
DeviceConfiguration::SystemConfig DeviceConfiguration::m_system;

bool DeviceConfiguration::load(const QString& externalPath)
{
    qInfo() << "Loading device configuration...";

    // Try external file first
    if (QFile::exists(externalPath)) {
        qInfo() << "  Loading from external file:" << externalPath;
        if (loadFromFile(externalPath)) {
            qInfo() << "  ✓ Configuration loaded from external file";
            return true;
        }
        qWarning() << "  ⚠ Failed to parse external config, trying embedded resource...";
    }

    // Fall back to embedded resource
    qInfo() << "  Loading from embedded resource: qrc:/config/devices.json";
    if (loadFromFile(":/config/devices.json")) {
        qInfo() << "  ✓ Configuration loaded from embedded resource";
        return true;
    }

    qCritical() << "  ✗ Failed to load configuration from any source!";
    return false;
}

bool DeviceConfiguration::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open config file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "JSON root is not an object";
        return false;
    }

    QJsonObject root = doc.object();

    // Parse System
    if (root.contains("system")) {
        QJsonObject sys = root["system"].toObject();
        m_system.name = sys["name"].toString(m_system.name);
        m_system.version = sys["version"].toString(m_system.version);
        m_system.accentColor = sys["accentColor"].toString(m_system.accentColor);
    }

    // Parse Video
    if (root.contains("video")) {
        QJsonObject video = root["video"].toObject();
        m_video.sourceWidth = video["sourceWidth"].toInt(m_video.sourceWidth);
        m_video.sourceHeight = video["sourceHeight"].toInt(m_video.sourceHeight);

        if (video.contains("dayCamera")) {
            QJsonObject day = video["dayCamera"].toObject();
            m_video.dayDevicePath = day["devicePath"].toString();
            m_video.dayControlPort = day["controlPort"].toString();
        }

        if (video.contains("nightCamera")) {
            QJsonObject night = video["nightCamera"].toObject();
            m_video.nightDevicePath = night["devicePath"].toString();
            m_video.nightControlPort = night["controlPort"].toString();
        }
    }

    // Parse IMU
    if (root.contains("imu")) {
        QJsonObject imu = root["imu"].toObject();
        m_imu.port = imu["port"].toString();
        m_imu.baudRate = imu["baudRate"].toInt(m_imu.baudRate);
        m_imu.slaveId = imu["slaveId"].toInt(m_imu.slaveId);
    }

    // Parse LRF
    if (root.contains("lrf")) {
        QJsonObject lrf = root["lrf"].toObject();
        m_lrf.port = lrf["port"].toString();
        m_lrf.baudRate = lrf["baudRate"].toInt(m_lrf.baudRate);
    }

    // Parse PLCs
    if (root.contains("plc")) {
        QJsonObject plc = root["plc"].toObject();

        if (plc.contains("plc21")) {
            QJsonObject plc21 = plc["plc21"].toObject();
            m_plc21.port = plc21["port"].toString();
            m_plc21.baudRate = plc21["baudRate"].toInt(m_plc21.baudRate);
            m_plc21.slaveId = plc21["slaveId"].toInt(m_plc21.slaveId);
            m_plc21.parity = parseParity(plc21["parity"].toString());
        }

        if (plc.contains("plc42")) {
            QJsonObject plc42 = plc["plc42"].toObject();
            m_plc42.port = plc42["port"].toString();
            m_plc42.baudRate = plc42["baudRate"].toInt(m_plc42.baudRate);
            m_plc42.slaveId = plc42["slaveId"].toInt(m_plc42.slaveId);
            m_plc42.parity = parseParity(plc42["parity"].toString());
        }
    }

    // Parse Servos
    if (root.contains("servo")) {
        QJsonObject servo = root["servo"].toObject();

        if (servo.contains("azimuth")) {
            QJsonObject az = servo["azimuth"].toObject();
            m_servoAz.name = az["name"].toString();
            m_servoAz.port = az["port"].toString();
            m_servoAz.baudRate = az["baudRate"].toInt(m_servoAz.baudRate);
            m_servoAz.slaveId = az["slaveId"].toInt(m_servoAz.slaveId);
            m_servoAz.parity = parseParity(az["parity"].toString());
        }

        if (servo.contains("elevation")) {
            QJsonObject el = servo["elevation"].toObject();
            m_servoEl.name = el["name"].toString();
            m_servoEl.port = el["port"].toString();
            m_servoEl.baudRate = el["baudRate"].toInt(m_servoEl.baudRate);
            m_servoEl.slaveId = el["slaveId"].toInt(m_servoEl.slaveId);
            m_servoEl.parity = parseParity(el["parity"].toString());
        }
    }

    // Parse Actuator
    if (root.contains("actuator")) {
        QJsonObject act = root["actuator"].toObject();
        m_actuator.port = act["port"].toString();
        m_actuator.baudRate = act["baudRate"].toInt(m_actuator.baudRate);
    }

    return true;
}

QSerialPort::Parity DeviceConfiguration::parseParity(const QString& parityStr)
{
    QString lower = parityStr.toLower();
    if (lower == "even") return QSerialPort::EvenParity;
    if (lower == "odd") return QSerialPort::OddParity;
    if (lower == "space") return QSerialPort::SpaceParity;
    if (lower == "mark") return QSerialPort::MarkParity;
    return QSerialPort::NoParity;
}
