# Hardware Devices Architecture - MIL-STD Pattern

## Table of Contents
1. [Overview](#overview)
2. [Architecture Principles](#architecture-principles)
3. [Three-Layer Architecture](#three-layer-architecture)
4. [Device Implementation](#device-implementation)
5. [Refactored Devices](#refactored-devices)
6. [Integration Guide](#integration-guide)
7. [Best Practices](#best-practices)

---

## Overview

The RCWS hardware subsystem has been refactored to follow **MIL-STD** (Military Standard) architecture patterns, providing a robust, maintainable, and testable foundation for hardware device communication.

### Key Benefits
- ✅ **Separation of Concerns**: Device logic, transport, and protocol are independent
- ✅ **Testability**: Mock transports and parsers for unit testing
- ✅ **Maintainability**: Protocol changes isolated in dedicated parsers
- ✅ **Consistency**: All devices follow the same pattern
- ✅ **Thread Safety**: Built-in thread-safe data access via TemplatedDevice

---

## Architecture Principles

### 1. Dependency Injection
Devices don't create their own communication channels. Instead, dependencies are injected:

```cpp
// Device doesn't own transport or parser
class MyDevice : public TemplatedDevice<MyData> {
    void setDependencies(Transport* transport, MyProtocolParser* parser);
};

// In SystemController
m_transport = new ModbusTransport(this);
m_parser = new MyProtocolParser(this);
m_device = new MyDevice("deviceId", this);
m_device->setDependencies(m_transport, m_parser);
```

### 2. Interface-Based Design
All components implement well-defined interfaces:
- **IDevice**: Common device interface
- **Transport**: Communication abstraction
- **ProtocolParser**: Protocol interpretation

### 3. Thread-Safe Data Access
Using `TemplatedDevice<TData>`:

```cpp
class MyDevice : public TemplatedDevice<MyData> {
    // Thread-safe read
    auto currentData = data();  // Returns shared_ptr<const MyData>

    // Thread-safe write
    auto newData = std::make_shared<MyData>(*data());
    newData->value = 42;
    updateData(newData);  // Protected by QReadWriteLock
};
```

---

## Three-Layer Architecture

```
┌──────────────────────────────────────────────────────────┐
│                    Device Layer                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │  Device Logic (ImuDevice, Plc21Device, etc.)      │  │
│  │  - Business logic                                   │  │
│  │  - Data aggregation                                 │  │
│  │  - Command generation                               │  │
│  │  - Inherits: TemplatedDevice<TData>                │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────┐
│                  Protocol Layer                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │  Protocol Parser (ImuProtocolParser, etc.)         │  │
│  │  - Parses raw bytes into Messages                  │  │
│  │  - Builds commands/requests                         │  │
│  │  - Protocol-specific logic (CRC, checksums, etc.)  │  │
│  │  - Implements: ProtocolParser interface            │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────┐
│                  Transport Layer                         │
│  ┌────────────────────────────────────────────────────┐  │
│  │  Transport (ModbusTransport, SerialPortTransport)  │  │
│  │  - Raw I/O operations                               │  │
│  │  - Connection management                            │  │
│  │  - Reconnection logic                               │  │
│  │  - Implements: Transport interface                  │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
```

### Layer Responsibilities

#### **Device Layer** - What to do
- State management
- Data validation
- Command sequencing
- Error recovery
- Emit signals for UI/controllers

#### **Protocol Layer** - How to communicate
- Packet framing
- CRC/checksum calculation
- Data encoding/decoding
- Protocol state machines
- Message parsing

#### **Transport Layer** - Physical communication
- Serial port / Modbus client
- Connection setup
- Raw byte transmission
- Error detection
- Reconnection

---

## Device Implementation

### Step 1: Define Data Structure

In `src/hardware/data/DataTypes.h`:

```cpp
struct ImuData {
    bool isConnected = false;
    double rollDeg = 0.0;
    double pitchDeg = 0.0;
    double yawDeg = 0.0;
    double temperature = 0.0;
    double accelX_g = 0.0;
    double accelY_g = 0.0;
    double accelZ_g = 0.0;
    double angRateX_dps = 0.0;
    double angRateY_dps = 0.0;
    double angRateZ_dps = 0.0;

    bool operator!=(const ImuData &other) const {
        return (isConnected != other.isConnected ||
                rollDeg != other.rollDeg ||
                pitchDeg != other.pitchDeg ||
                // ... compare all fields
        );
    }
};
```

**Design Guidelines:**
- Always include `isConnected` flag
- Implement `operator!=` for change detection
- Use appropriate data types (avoid raw integers when floats make sense)
- Add units in field names (e.g., `_dps` for degrees per second)

### Step 2: Define Message Types

In `src/hardware/messages/ImuMessage.h`:

```cpp
#pragma once
#include "../interfaces/Message.h"
#include "../data/DataTypes.h"

class ImuDataMessage : public Message {
public:
    explicit ImuDataMessage(const ImuData& data) : m_data(data) {}

    Type typeId() const override {
        return Type::ImuDataType;  // Add to Message::Type enum
    }

    const ImuData& data() const { return m_data; }

private:
    ImuData m_data;
};
```

**Don't forget:** Add `ImuDataType` to `Message::Type` enum in `src/hardware/interfaces/Message.h`

### Step 3: Implement Protocol Parser

In `src/hardware/protocols/ImuProtocolParser.h`:

```cpp
#pragma once
#include "../interfaces/ProtocolParser.h"
#include <QModbusDataUnit>
#include <QModbusReply>

class ImuProtocolParser : public ProtocolParser {
    Q_OBJECT
public:
    explicit ImuProtocolParser(QObject* parent = nullptr);

    // For serial protocols
    std::vector<MessagePtr> parse(const QByteArray& rawData) override;

    // For Modbus protocols
    std::vector<MessagePtr> parse(QModbusReply* reply) override;

    // Build read request (for Modbus/command-response protocols)
    QModbusDataUnit buildReadRequest() const;

private:
    float parseFloat(const QModbusDataUnit& unit, int index) const;
    // ... protocol-specific helpers
};
```

In `src/hardware/protocols/ImuProtocolParser.cpp`:

```cpp
#include "ImuProtocolParser.h"
#include "../messages/ImuMessage.h"
#include <cstring>  // for memcpy

std::vector<MessagePtr> ImuProtocolParser::parse(QModbusReply* reply) {
    std::vector<MessagePtr> messages;

    if (!reply || reply->error() != QModbusDevice::NoError) {
        return messages;
    }

    QModbusDataUnit unit = reply->result();
    if (unit.valueCount() < 18) {  // Need 18 registers for 9 floats
        return messages;
    }

    // Parse data
    ImuData data;
    data.isConnected = true;
    data.rollDeg = parseFloat(unit, 0);
    data.pitchDeg = parseFloat(unit, 2);
    // ... parse all fields

    messages.push_back(std::make_unique<ImuDataMessage>(data));
    return messages;
}

float ImuProtocolParser::parseFloat(const QModbusDataUnit& unit, int index) const {
    // Parse 32-bit float from two 16-bit registers (big-endian)
    quint16 high = unit.value(index);
    quint16 low = unit.value(index + 1);
    quint32 combined = (static_cast<quint32>(high) << 16) | low;
    float value;
    std::memcpy(&value, &combined, sizeof(value));
    return value;
}
```

### Step 4: Implement Device Class

In `src/hardware/devices/imudevice.h`:

```cpp
#pragma once
#include "TemplatedDevice.h"
#include "../data/DataTypes.h"

class Transport;
class ImuProtocolParser;

class ImuDevice : public TemplatedDevice<ImuData> {
    Q_OBJECT
public:
    explicit ImuDevice(const QString& identifier, QObject* parent = nullptr);
    ~ImuDevice() override;

    // IDevice interface
    bool initialize() override;
    void shutdown() override;
    DeviceType type() const override { return DeviceType::Imu; }

    // Dependency injection
    Q_INVOKABLE void setDependencies(Transport* transport, ImuProtocolParser* parser);

    // Device-specific methods
    void setPollInterval(int ms);

signals:
    void imuDataChanged(const ImuData& data);

private slots:
    void onTransportConnected();
    void onTransportDisconnected();
    void onFrameReceived(const QByteArray& frame);
    void pollDevice();

private:
    void processMessage(const Message& message);

    Transport* m_transport = nullptr;
    ImuProtocolParser* m_parser = nullptr;
    QTimer* m_pollTimer = nullptr;
    int m_pollInterval = 50;  // 50ms = 20Hz
};
```

In `src/hardware/devices/imudevice.cpp`:

```cpp
#include "imudevice.h"
#include "../interfaces/Transport.h"
#include "../protocols/ImuProtocolParser.h"
#include "../messages/ImuMessage.h"

ImuDevice::ImuDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<ImuData>(identifier, parent)
    , m_pollTimer(new QTimer(this))
{
    // Initialize with default disconnected state
    auto initialData = std::make_shared<ImuData>();
    initialData->isConnected = false;
    updateData(initialData);

    connect(m_pollTimer, &QTimer::timeout, this, &ImuDevice::pollDevice);
}

void ImuDevice::setDependencies(Transport* transport, ImuProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    if (m_transport) {
        connect(m_transport, &Transport::connectionStateChanged,
                this, [this](bool connected) {
            if (connected) onTransportConnected();
            else onTransportDisconnected();
        });

        connect(m_transport, &Transport::frameReceived,
                this, &ImuDevice::onFrameReceived);
    }
}

bool ImuDevice::initialize() {
    if (!m_transport || !m_parser) {
        qWarning() << "ImuDevice: Missing dependencies";
        return false;
    }

    setState(DeviceState::Initializing);

    // Transport should already be opened by SystemController
    // Just start polling
    m_pollTimer->start(m_pollInterval);

    setState(DeviceState::Online);
    return true;
}

void ImuDevice::pollDevice() {
    if (!m_transport || !m_parser) return;

    // For Modbus, send read request
    // (This is a simplified example - actual implementation uses ModbusTransport)
    QModbusDataUnit request = m_parser->buildReadRequest();
    // m_transport->sendReadRequest(request);
}

void ImuDevice::onFrameReceived(const QByteArray& frame) {
    if (!m_parser) return;

    auto messages = m_parser->parse(frame);
    for (const auto& msg : messages) {
        processMessage(*msg);
    }
}

void ImuDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::ImuDataType) {
        auto* imuMsg = static_cast<const ImuDataMessage*>(&message);
        const ImuData& newData = imuMsg->data();

        // Update internal state
        auto currentData = data();
        if (newData != *currentData) {
            auto updatedData = std::make_shared<ImuData>(newData);
            updateData(updatedData);

            // Emit signal for models
            emit imuDataChanged(newData);
        }
    }
}
```

### Step 5: Register in SystemController

In `src/controllers/systemcontroller.h`:

```cpp
// Add forward declarations
class ImuDevice;
class ImuProtocolParser;

// Add member variables
private:
    ModbusTransport* m_imuTransport = nullptr;
    ImuProtocolParser* m_imuParser = nullptr;
    ImuDevice* m_imuDevice = nullptr;
```

In `src/controllers/systemcontroller.cpp`:

```cpp
void SystemController::initializeHardware() {
    // 1. Create transport
    m_imuTransport = new ModbusTransport(this);

    // 2. Create parser
    m_imuParser = new ImuProtocolParser(this);

    // 3. Create device
    m_imuDevice = new ImuDevice("imu", this);
    m_imuDevice->setDependencies(m_imuTransport, m_imuParser);
}

void SystemController::startSystem() {
    // Configure and open transport
    QJsonObject imuConfig;
    imuConfig["port"] = "/dev/ttyUSB0";
    imuConfig["baudRate"] = 115200;
    imuConfig["parity"] = static_cast<int>(QSerialPort::NoParity);
    imuConfig["slaveId"] = 1;
    m_imuTransport->open(imuConfig);

    // Initialize device
    m_imuDevice->initialize();
}
```

---

## Refactored Devices

### Summary Table

| Device | Protocol | Transport | Status | Notes |
|--------|----------|-----------|--------|-------|
| **IMU** | Modbus RTU | ModbusTransport | ✅ Complete | SST810 inclinometer, 32-bit float parsing |
| **PLC21** | Modbus RTU | ModbusTransport | ✅ Complete | Panel controls, 13 digital + 6 analog inputs |
| **PLC42** | Modbus RTU | ModbusTransport | ✅ Complete | System interlocks, 8 digital inputs + 8 registers |
| **Day Camera** | Pelco-D | SerialPortTransport | ✅ Complete | 7-byte frames, checksum validation |
| **Night Camera** | TAU2 | SerialPortTransport | ✅ Complete | Dual CRC-16 validation |
| **Joystick** | SDL2 Events | N/A | ✅ Complete | Event-based, no transport layer |
| **Servo Driver (Az)** | Modbus RTU | ModbusTransport | ✅ Complete | Position, speed, torque monitoring |
| **Servo Driver (El)** | Modbus RTU | ModbusTransport | ✅ Complete | Same as Az |
| **Servo Actuator** | ASCII Commands | SerialPortTransport | ✅ Complete | Gun trigger control |
| **LRF** | Binary Protocol | SerialPortTransport | ✅ Complete | Laser rangefinder, 50m-4000m |
| **Lens** | Custom | BaseSerialDevice | ⏳ Legacy | Not yet refactored |
| **Radar** | NMEA 0183 | SerialPortTransport | ✅ Complete | RATTM sentence parsing |

### Device Details

#### IMU Device (SST810 Inclinometer)
- **Protocol**: Modbus RTU with 32-bit float registers
- **Data**: Roll, pitch, yaw, accelerometer (X/Y/Z), gyroscope (X/Y/Z), temperature
- **Special Features**: Big-endian float parsing (2 registers per value)
- **Poll Rate**: 50ms (20Hz)
- **Register Start**: 0x03E8 (1000 decimal)

#### PLC21 Device (Panel Controls)
- **Protocol**: Modbus RTU
- **Inputs**: 13 discrete inputs (switches/buttons)
- **Analog**: 6 analog inputs (speed selector, fire mode, temperature)
- **Poll Rate**: 50ms
- **Special**: Digital output control (LEDs, indicators)

#### PLC42 Device (System Interlocks)
- **Protocol**: Modbus RTU
- **Inputs**: 8 discrete inputs (sensors, emergency stop)
- **Registers**: 8 holding registers (control modes, speeds, directions)
- **Special**: 32-bit speed values (split across 2 registers)
- **Poll Rate**: 50ms

#### Day Camera (Sony FCB-EV7520A)
- **Protocol**: Pelco-D
- **Frame**: 7 bytes (sync, address, command1, command2, data1, data2, checksum)
- **Checksum**: Sum of bytes 2-6, masked to 8-bit
- **Features**: Zoom, focus, pan, tilt control
- **HFOV**: Calculated from zoom position (63.7° wide to 2.3° tele)
- **Baud Rate**: 9600

#### Night Camera (FLIR TAU2)
- **Protocol**: TAU2 (custom binary)
- **CRC**: Dual CRC-16 CCITT (header + full packet)
- **Polynomial**: 0x1021
- **Features**: FFC calibration, digital zoom, video mode/LUT
- **Status Polling**: 5000ms interval
- **Baud Rate**: 57600

#### Joystick (Thrustmaster HOTAS Warthog)
- **Interface**: SDL2 library
- **Events**: Axis motion, button press/release, hat motion
- **Normalization**: int16 (-32768 to 32767) → float (-1.0 to 1.0)
- **Deadzone**: 9% (3000 units)
- **Poll Rate**: 16ms (~60Hz)
- **GUID**: `030000004f0400000204000011010000`

#### Servo Driver (Azimuth/Elevation)
- **Protocol**: Modbus RTU
- **Data**: Position (encoder counts), RPM, torque, motor/driver temps
- **Alarms**: Real-time alarm detection and history
- **Control**: Position setpoint, speed, mode selection
- **Baud Rate**: 230400 (high-speed)

#### Servo Actuator (Gun Trigger)
- **Protocol**: ASCII commands over serial
- **Command Format**: `{COMMAND PARAM1 PARAM2...}\r\n`
- **Response**: ACK/NACK with status hex value
- **Features**: Position control, velocity, critical fault detection
- **Timeout**: 1000ms per command
- **Status Parsing**: 32-bit hex value decoded to messages

#### LRF (Laser Rangefinder)
- **Protocol**: Binary (custom)
- **Frame**: Header + Device Code + Command + Data + CRC
- **Range**: 50m to 4000m
- **Data**: Distance, pulse count, temperature, laser count
- **Fault Detection**: No echo, laser fault, over-temperature

#### Radar (Marine Radar)
- **Protocol**: NMEA 0183 (RATTM sentences)
- **Data**: Target ID, azimuth, range, course, speed
- **Validation**: XOR checksum
- **Multi-Target**: Track multiple targets by ID
- **Conversions**: Nautical miles → meters, knots → m/s
- **Baud Rate**: 4800

---

## Integration Guide

### Adding a New Device

**Step 1**: Define data structure in `DataTypes.h`
```cpp
struct MyDeviceData {
    bool isConnected = false;
    // ... fields
    bool operator!=(const MyDeviceData &other) const;
};
```

**Step 2**: Add message type to `Message.h`
```cpp
enum class Type {
    // ...
    MyDeviceDataType,
};
```

**Step 3**: Create message class in `messages/MyDeviceMessage.h`

**Step 4**: Create protocol parser in `protocols/MyDeviceProtocolParser.h/.cpp`

**Step 5**: Create device class in `devices/mydevice.h/.cpp`

**Step 6**: Update `IDevice.h` enum:
```cpp
enum class DeviceType { ..., MyDevice };
```

**Step 7**: Register in SystemController

**Step 8**: Create data model if needed for UI

**Step 9**: Connect signals in `SystemController::connectDevicesToModels()`

**Step 10**: Test!

---

## Best Practices

### 1. Error Handling
```cpp
// Always check dependencies
if (!m_transport || !m_parser) {
    qWarning() << "Device: Missing dependencies";
    return false;
}

// Handle transport errors
connect(m_transport, &Transport::linkError, this, [](const QString& error) {
    qWarning() << "Transport error:" << error;
});

// Validate parsed data
if (data.isValid()) {
    updateData(data);
} else {
    qWarning() << "Invalid data received";
}
```

### 2. Thread Safety
```cpp
// CORRECT: Use shared_ptr for thread-safe access
auto currentData = data();  // Returns shared_ptr<const MyData>
qDebug() << "Value:" << currentData->value;

// INCORRECT: Don't store references
const MyData& ref = *data();  // DANGEROUS! Pointer may change
```

### 3. State Management
```cpp
// Use proper state transitions
setState(DeviceState::Initializing);
if (initSuccess) {
    setState(DeviceState::Online);
} else {
    setState(DeviceState::Error);
}

// Emit state changes
emit stateChanged(m_state);
```

### 4. Signal Emissions
```cpp
// Only emit when data actually changes
if (newData != *data()) {
    updateData(newData);
    emit dataChanged(newData);
}

// Don't emit in hot paths without checks
```

### 5. Configuration
```cpp
// Use JSON for transport configuration
QJsonObject config;
config["port"] = "/dev/ttyUSB0";
config["baudRate"] = 115200;
config["parity"] = static_cast<int>(QSerialPort::NoParity);
config["slaveId"] = 1;  // For Modbus
m_transport->open(config);
```

### 6. Polling vs Event-Driven
```cpp
// Polling (for Modbus, etc.)
m_pollTimer = new QTimer(this);
connect(m_pollTimer, &QTimer::timeout, this, &Device::poll);
m_pollTimer->start(50);  // 50ms = 20Hz

// Event-driven (for serial protocols)
connect(m_transport, &Transport::frameReceived,
        this, &Device::onFrameReceived);
```

### 7. Resource Cleanup
```cpp
void MyDevice::shutdown() {
    // Stop timers
    if (m_pollTimer) {
        m_pollTimer->stop();
    }

    // Disconnect signals (Qt does this automatically on delete, but be explicit)
    if (m_transport) {
        disconnect(m_transport, nullptr, this, nullptr);
    }

    // Update state
    auto disconnectedData = std::make_shared<MyData>();
    disconnectedData->isConnected = false;
    updateData(disconnectedData);

    setState(DeviceState::Offline);
}
```

---

## Testing Strategy

### Unit Testing
```cpp
// Mock transport
class MockTransport : public Transport {
    // ... implement interface with test data
};

// Test device
TEST_F(MyDeviceTest, ParsesDataCorrectly) {
    auto mockTransport = new MockTransport();
    auto parser = new MyDeviceProtocolParser();
    auto device = new MyDevice("test");
    device->setDependencies(mockTransport, parser);

    // Inject test data
    mockTransport->injectData(testPacket);

    // Verify
    EXPECT_EQ(device->data()->value, expectedValue);
}
```

### Integration Testing
- Use virtual serial ports (`socat`) for protocol testing
- Modbus simulator for Modbus devices
- SDL2 test joystick for joystick device

### Hardware-in-the-Loop Testing
- Connect actual hardware
- Verify real-time performance
- Check error recovery
- Test timeout scenarios

---

*Last Updated: January 2025*
*Document Version: 1.0*
