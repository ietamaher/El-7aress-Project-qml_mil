/**
 * @file imudevice.h
 * @brief Refactored IMU device following MIL-STD architecture
 *
 * This class represents ONLY the device logic - no transport or protocol handling.
 * Transport and protocol parsing are injected as dependencies.
 *
 * @section Architecture
 * - Device: Pure business logic (this class)
 * - Transport: ModbusTransport (injected)
 * - Parser: ImuProtocolParser (injected)
 * - Data: ImuData (in DataTypes.h)
 *
 * @section Benefits
 * - Clean separation of concerns
 * - Thread-safe data access (automatic via TemplatedDevice)
 * - Easy unit testing (mock transport/parser)
 * - Protocol changes isolated to parser
 *
 * @author refactored_to_milstd
 * @date 2025-10-28
 * @version 2.0
 */

#ifndef IMUDEVICE_H
#define IMUDEVICE_H

#include "../devices/TemplatedDevice.h"
#include "../data/DataTypes.h"
#include <QTimer>

class Transport;
class ImuProtocolParser;
class QModbusReply;
class Message;

/**
 * @brief Modbus-based IMU/Inclinometer device (SST810)
 *
 * Manages an SST810 inclinometer via Modbus RTU protocol. This class contains
 * ONLY device-specific logic - all transport and protocol handling
 * is delegated to injected dependencies.
 *
 * Reads 18 Input Registers containing 9 float values (angles, accelerations, gyro rates).
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
                                      ImuProtocolParser* parser);

    // IDevice interface (device lifecycle)
    Q_INVOKABLE bool initialize() override;
    void shutdown() override;
    DeviceType type() const override { return DeviceType::Imu; }

    // Configuration
    Q_INVOKABLE void setPollInterval(int intervalMs);

signals:
    void imuDataChanged(const ImuData& data);

private slots:
    void pollTimerTimeout();
    void onModbusReplyReady(QModbusReply* reply);
    void processMessage(const Message& message);
    void onCommunicationWatchdogTimeout();

private:
    void sendReadRequest();
    void resetCommunicationWatchdog();
    void setConnectionState(bool connected);

    QString m_identifier;
    Transport* m_transport = nullptr;
    ImuProtocolParser* m_parser = nullptr;

    QTimer* m_pollTimer;
    QTimer* m_communicationWatchdog = nullptr;

    static constexpr int COMMUNICATION_TIMEOUT_MS = 3000;  // 3 seconds without data = disconnected
};

#endif // IMUDEVICE_H
