/**
 * @file imudevice.h
 * @brief 3DM-GX3-25 MicroStrain AHRS device following MIL-STD architecture
 *
 * This class represents ONLY the device logic - no transport or protocol handling.
 * Transport and protocol parsing are injected as dependencies.
 *
 * @section Architecture
 * - Device: Pure business logic (this class)
 * - Transport: SerialTransport (injected)
 * - Parser: Imu3DMGX3ProtocolParser (injected)
 * - Data: ImuData (in DataTypes.h)
 *
 * @section Protocol
 * - 3DM-GX3-25 uses single-byte serial binary commands (not Modbus)
 * - Primary command: 0xCF (Euler Angles + Angular Rates)
 * - Operates in continuous streaming mode
 * - Data rate: up to 1000Hz (configurable)
 *
 * @section Benefits
 * - Real magnetic heading (yaw) from magnetometer
 * - Higher resolution (17-bit ADC)
 * - Built-in Kalman filtering
 * - Gyro-stabilized outputs
 *
 * @author refactored_to_milstd
 * @date 2025-11-03
 * @version 3.0 - Migrated to 3DM-GX3-25
 */

#ifndef IMUDEVICE_H
#define IMUDEVICE_H

#include "../devices/TemplatedDevice.h"
#include "../data/DataTypes.h"
#include <QTimer>

class Transport;
class Imu3DMGX3ProtocolParser;
class Message;

/**
 * @brief 3DM-GX3-25 MicroStrain AHRS device
 *
 * Manages a 3DM-GX3-25 AHRS via serial binary protocol. This class contains
 * ONLY device-specific logic - all transport and protocol handling
 * is delegated to injected dependencies.
 *
 * Features:
 * - Continuous streaming mode (0xCF: Euler angles + angular rates)
 * - Automatic gyro bias calibration (0xCD)
 * - Configurable sampling rate (0xDB: up to 1000Hz)
 * - Real-time orientation and angular rates for gimbal stabilization
 */
class ImuDevice : public TemplatedDevice<ImuData> {
    Q_OBJECT
public:
    explicit ImuDevice(const QString& identifier, QObject* parent = nullptr);
    ~ImuDevice() override;

    // Device identification
    QString identifier() const { return m_identifier; }

    // Dependency injection (called before initialize)
    Q_INVOKABLE void setDependencies(Transport* transport,
                                      Imu3DMGX3ProtocolParser* parser);

    // IDevice interface (device lifecycle)
    Q_INVOKABLE bool initialize() override;
    void shutdown() override;
    DeviceType type() const override { return DeviceType::Imu; }

    // Configuration
    Q_INVOKABLE void setSamplingRate(int rateHz);  // 100-1000Hz

signals:
    void imuDataChanged(const ImuData& data);

private slots:
    void onSerialDataReceived(const QByteArray& data);
    void processMessage(const Message& message);
    void onCommunicationWatchdogTimeout();
    void onInitializationTimeout();

private:
    void startInitializationSequence();
    void sendCaptureGyroBiasCommand();
    void sendSamplingSettingsCommand();
    void startContinuousMode();
    void resetCommunicationWatchdog();
    void setConnectionState(bool connected);

    QString m_identifier;
    Transport* m_transport = nullptr;
    Imu3DMGX3ProtocolParser* m_parser = nullptr;

    QTimer* m_communicationWatchdog = nullptr;
    QTimer* m_initializationTimer = nullptr;

    enum class InitState {
        NotStarted,
        WaitingForGyroBias,      // After sending 0xCD
        WaitingForSamplingRate,  // After sending 0xDB
        StartingContinuousMode,  // After sending 0xC4
        Running                  // Continuous mode active
    };
    InitState m_initState = InitState::NotStarted;

    int m_samplingRateHz = 100;  // Default 100Hz

    static constexpr int COMMUNICATION_TIMEOUT_MS = 3000;  // 3 seconds without data = disconnected
    static constexpr int GYRO_BIAS_TIMEOUT_MS = 15000;     // 15 seconds for gyro bias capture
};

#endif // IMUDEVICE_H
