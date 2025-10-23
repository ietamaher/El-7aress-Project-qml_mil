#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include <QObject>
#include <QString>
#include <QSerialPort>

class DeviceConfiguration
{
public:
    struct VideoConfig {
        int sourceWidth = 1280;
        int sourceHeight = 720;
        QString dayDevicePath;
        QString dayControlPort;
        QString nightDevicePath;
        QString nightControlPort;
    };

    struct ImuConfig {
        QString port;
        int baudRate = 115200;
        int slaveId = 1;
    };

    struct LrfConfig {
        QString port;
        int baudRate = 115200;
    };

    struct PlcConfig {
        QString port;
        int baudRate = 115200;
        int slaveId = 31;
        QSerialPort::Parity parity = QSerialPort::EvenParity;
    };

    struct ServoConfig {
        QString name;
        QString port;
        int baudRate = 230400;
        int slaveId = 1;
        QSerialPort::Parity parity = QSerialPort::NoParity;
    };

    struct ActuatorConfig {
        QString port;
        int baudRate = 115200;
    };

    struct SystemConfig {
        QString name = "El 7arress RCWS";
        QString version = "4.5";
        QString accentColor = "#46E2A5";
    };

    // Load configuration from file (tries external first, then embedded resource)
    static bool load(const QString& externalPath = "./config/devices.json");

    // Getters
    static const VideoConfig& video() { return m_video; }
    static const ImuConfig& imu() { return m_imu; }
    static const LrfConfig& lrf() { return m_lrf; }
    static const PlcConfig& plc21() { return m_plc21; }
    static const PlcConfig& plc42() { return m_plc42; }
    static const ServoConfig& servoAz() { return m_servoAz; }
    static const ServoConfig& servoEl() { return m_servoEl; }
    static const ActuatorConfig& actuator() { return m_actuator; }
    static const SystemConfig& system() { return m_system; }

private:
    static bool loadFromFile(const QString& filePath);
    static QSerialPort::Parity parseParity(const QString& parityStr);

    static VideoConfig m_video;
    static ImuConfig m_imu;
    static LrfConfig m_lrf;
    static PlcConfig m_plc21;
    static PlcConfig m_plc42;
    static ServoConfig m_servoAz;
    static ServoConfig m_servoEl;
    static ActuatorConfig m_actuator;
    static SystemConfig m_system;
};

#endif // DEVICECONFIGURATION_H
