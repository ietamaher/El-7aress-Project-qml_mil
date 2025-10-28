/**
 * @file plc42device.h
 * @brief Refactored PLC42 device following MIL-STD architecture
 *
 * This class represents ONLY the device logic - no transport or protocol handling.
 * Transport and protocol parsing are injected as dependencies.
 *
 * @section Architecture
 * - Device: Pure business logic (this class)
 * - Transport: ModbusTransport (injected)
 * - Parser: Plc42ProtocolParser (injected)
 * - Data: Plc42Data (in DataTypes.h)
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

#ifndef PLC42DEVICE_H
#define PLC42DEVICE_H

#include "../devices/TemplatedDevice.h"
#include "../data/DataTypes.h"
#include <QTimer>

class Transport;
class Plc42ProtocolParser;
class QModbusReply;
class Message;

/**
 * @brief Modbus-based PLC42 device
 *
 * Manages a PLC42 controller via Modbus RTU protocol. This class contains
 * ONLY device-specific logic - all transport and protocol handling
 * is delegated to injected dependencies.
 */
class Plc42Device : public TemplatedDevice<Plc42Data> {
    Q_OBJECT
public:
    explicit Plc42Device(const QString& identifier, QObject* parent = nullptr);
    ~Plc42Device() override;

    // Device identification
    QString identifier() const { return m_identifier; }

    // Dependency injection (called before initialize)
    Q_INVOKABLE void setDependencies(Transport* transport,
                                      Plc42ProtocolParser* parser);

    // IDevice interface (device lifecycle)
    Q_INVOKABLE bool initialize() override;
    void shutdown() override;
    DeviceType type() const override { return DeviceType::Plc42; }

    // Public API - Control methods
    Q_INVOKABLE void setSolenoidMode(uint16_t mode);
    Q_INVOKABLE void setGimbalMotionMode(uint16_t mode);
    Q_INVOKABLE void setAzimuthSpeedHolding(uint32_t speed);
    Q_INVOKABLE void setElevationSpeedHolding(uint32_t speed);
    Q_INVOKABLE void setAzimuthDirection(uint16_t direction);
    Q_INVOKABLE void setElevationDirection(uint16_t direction);
    Q_INVOKABLE void setSolenoidState(uint16_t state);
    Q_INVOKABLE void setResetAlarm(uint16_t alarm);

    // Configuration
    Q_INVOKABLE void setPollInterval(int intervalMs);

signals:
    void plc42DataChanged(const Plc42Data& data);
    void registerWritten(bool success);

private slots:
    void pollTimerTimeout();
    void onModbusReplyReady(QModbusReply* reply);
    void processMessage(const Message& message);

private:
    void sendReadRequest(int startAddress, int count, bool isDiscreteInputs = true);
    void sendWriteHoldingRegisters();
    void mergePartialData(const Plc42Data& partialData);

    QString m_identifier;
    Transport* m_transport = nullptr;
    Plc42ProtocolParser* m_parser = nullptr;

    QTimer* m_pollTimer;
    Plc42Data m_pendingWrites; // Data to be written on next write cycle
    bool m_hasPendingWrites = false;
};

#endif // PLC42DEVICE_H
