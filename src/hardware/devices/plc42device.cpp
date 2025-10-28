#include "plc42device.h"
#include "../interfaces/Transport.h"
#include "../protocols/Plc42ProtocolParser.h"
#include "../messages/Plc42Message.h"
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QDebug>

Plc42Device::Plc42Device(const QString& identifier, QObject* parent)
    : TemplatedDevice<Plc42Data>(parent),
      m_identifier(identifier),
      m_pollTimer(new QTimer(this))
{
    connect(m_pollTimer, &QTimer::timeout, this, &Plc42Device::pollTimerTimeout);
}

Plc42Device::~Plc42Device() {
    m_pollTimer->stop();
}

void Plc42Device::setDependencies(Transport* transport,
                                   Plc42ProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Connect transport signals
    connect(m_transport, &Transport::connectionStateChanged,
            this, [this](bool connected) {
        auto newData = std::make_shared<Plc42Data>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit plc42DataChanged(*newData);
    });
}

bool Plc42Device::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Get configuration from device property
    QJsonObject config = property("config").toJsonObject();
    int pollInterval = config["pollIntervalMs"].toInt(50);

    qDebug() << m_identifier << "initializing with poll interval:" << pollInterval << "ms";

    // Open transport
    if (m_transport->open(config)) {
        setState(DeviceState::Online);

        // Start polling
        m_pollTimer->start(pollInterval);

        qDebug() << m_identifier << "initialized successfully";
        return true;
    }

    qCritical() << m_identifier << "failed to initialize transport";
    setState(DeviceState::Error);
    return false;
}

void Plc42Device::shutdown() {
    m_pollTimer->stop();

    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }

    setState(DeviceState::Offline);
}

void Plc42Device::pollTimerTimeout() {
    // Read digital inputs (discrete inputs)
    sendReadRequest(Plc42Registers::DIGITAL_INPUTS_START_ADDR,
                    7,  // Read 7 discrete inputs
                    true);

    // Read holding registers
    sendReadRequest(Plc42Registers::HOLDING_REGISTERS_START_ADDR,
                    Plc42Registers::HOLDING_REGISTERS_COUNT,
                    false);
}

void Plc42Device::sendReadRequest(int startAddress, int count, bool isDiscreteInputs) {
    if (state() != DeviceState::Online || !m_transport) return;

    // Cast to ModbusTransport to access Modbus-specific methods
    auto modbusTransport = qobject_cast<QModbusRtuSerialClient*>(
        m_transport->property("client").value<QObject*>());

    if (!modbusTransport) return;

    QModbusDataUnit::RegisterType regType = isDiscreteInputs ?
        QModbusDataUnit::DiscreteInputs : QModbusDataUnit::HoldingRegisters;

    QModbusDataUnit readUnit(regType, startAddress, count);

    QModbusReply* reply = nullptr;
    QMetaObject::invokeMethod(m_transport, "sendReadRequest",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QModbusReply*, reply),
                              Q_ARG(QModbusDataUnit, readUnit));

    if (reply) {
        connect(reply, &QModbusReply::finished, this, [this, reply]() {
            onModbusReplyReady(reply);
        });
    }
}

void Plc42Device::onModbusReplyReady(QModbusReply* reply) {
    if (!reply || !m_parser) {
        if (reply) reply->deleteLater();
        return;
    }

    if (reply->error() != QModbusDevice::NoError) {
        qWarning() << m_identifier << "Modbus error:" << reply->errorString();
        auto newData = std::make_shared<Plc42Data>(*data());
        newData->isConnected = false;
        updateData(newData);
        emit plc42DataChanged(*newData);
        reply->deleteLater();
        return;
    }

    // Parse the reply into messages
    auto messages = m_parser->parse(reply);
    reply->deleteLater();

    // Process each message
    for (const auto& msg : messages) {
        if (msg) {
            processMessage(*msg);
        }
    }
}

void Plc42Device::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::Plc42DataType) {
        auto const* dataMsg = static_cast<const Plc42DataMessage*>(&message);
        mergePartialData(dataMsg->data());
    }
}

void Plc42Device::mergePartialData(const Plc42Data& partialData) {
    auto currentData = data();
    auto newData = std::make_shared<Plc42Data>(*currentData);
    newData->isConnected = true;

    bool dataChanged = false;

    // Merge discrete inputs
    if (partialData.stationUpperSensor != currentData->stationUpperSensor ||
        partialData.stationLowerSensor != currentData->stationLowerSensor ||
        partialData.emergencyStopActive != currentData->emergencyStopActive ||
        partialData.ammunitionLevel != currentData->ammunitionLevel ||
        partialData.stationInput1 != currentData->stationInput1 ||
        partialData.stationInput2 != currentData->stationInput2 ||
        partialData.stationInput3 != currentData->stationInput3 ||
        partialData.solenoidActive != currentData->solenoidActive) {

        newData->stationUpperSensor = partialData.stationUpperSensor;
        newData->stationLowerSensor = partialData.stationLowerSensor;
        newData->emergencyStopActive = partialData.emergencyStopActive;
        newData->ammunitionLevel = partialData.ammunitionLevel;
        newData->stationInput1 = partialData.stationInput1;
        newData->stationInput2 = partialData.stationInput2;
        newData->stationInput3 = partialData.stationInput3;
        newData->solenoidActive = partialData.solenoidActive;
        dataChanged = true;
    }

    // Merge holding registers
    if (partialData.solenoidMode != currentData->solenoidMode ||
        partialData.gimbalOpMode != currentData->gimbalOpMode ||
        partialData.azimuthSpeed != currentData->azimuthSpeed ||
        partialData.elevationSpeed != currentData->elevationSpeed ||
        partialData.azimuthDirection != currentData->azimuthDirection ||
        partialData.elevationDirection != currentData->elevationDirection ||
        partialData.solenoidState != currentData->solenoidState ||
        partialData.resetAlarm != currentData->resetAlarm) {

        newData->solenoidMode = partialData.solenoidMode;
        newData->gimbalOpMode = partialData.gimbalOpMode;
        newData->azimuthSpeed = partialData.azimuthSpeed;
        newData->elevationSpeed = partialData.elevationSpeed;
        newData->azimuthDirection = partialData.azimuthDirection;
        newData->elevationDirection = partialData.elevationDirection;
        newData->solenoidState = partialData.solenoidState;
        newData->resetAlarm = partialData.resetAlarm;
        dataChanged = true;
    }

    if (dataChanged) {
        updateData(newData);
        emit plc42DataChanged(*newData);
    }
}

//================================================================================
// PUBLIC API - CONTROL METHODS
//================================================================================

void Plc42Device::setSolenoidMode(uint16_t mode) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->solenoidMode = mode;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setGimbalMotionMode(uint16_t mode) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->gimbalOpMode = mode;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setAzimuthSpeedHolding(uint32_t speed) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->azimuthSpeed = speed;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setElevationSpeedHolding(uint32_t speed) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->elevationSpeed = speed;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setAzimuthDirection(uint16_t direction) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->azimuthDirection = direction;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setElevationDirection(uint16_t direction) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->elevationDirection = direction;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setSolenoidState(uint16_t state) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->solenoidState = state;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::setResetAlarm(uint16_t alarm) {
    auto newData = std::make_shared<Plc42Data>(*data());
    newData->resetAlarm = alarm;
    updateData(newData);
    m_hasPendingWrites = true;
    sendWriteHoldingRegisters();
}

void Plc42Device::sendWriteHoldingRegisters() {
    if (state() != DeviceState::Online || !m_transport) return;

    auto currentData = data();

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,
                              Plc42Registers::HOLDING_REGISTERS_START_ADDR,
                              Plc42Registers::HOLDING_REGISTERS_COUNT);

    writeUnit.setValue(0, currentData->solenoidMode);
    writeUnit.setValue(1, currentData->gimbalOpMode);

    // Split 32-bit azimuth speed into two 16-bit registers
    uint16_t azLow  = static_cast<uint16_t>(currentData->azimuthSpeed & 0xFFFF);
    uint16_t azHigh = static_cast<uint16_t>((currentData->azimuthSpeed >> 16) & 0xFFFF);
    writeUnit.setValue(2, azLow);
    writeUnit.setValue(3, azHigh);

    // Split 32-bit elevation speed into two 16-bit registers
    uint16_t elLow  = static_cast<uint16_t>(currentData->elevationSpeed & 0xFFFF);
    uint16_t elHigh = static_cast<uint16_t>((currentData->elevationSpeed >> 16) & 0xFFFF);
    writeUnit.setValue(4, elLow);
    writeUnit.setValue(5, elHigh);

    writeUnit.setValue(6, currentData->azimuthDirection);
    writeUnit.setValue(7, currentData->elevationDirection);
    writeUnit.setValue(8, currentData->solenoidState);
    writeUnit.setValue(9, currentData->resetAlarm);

    QModbusReply* reply = nullptr;
    QMetaObject::invokeMethod(m_transport, "sendWriteRequest",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QModbusReply*, reply),
                              Q_ARG(QModbusDataUnit, writeUnit));

    if (reply) {
        connect(reply, &QModbusReply::finished, this, [this, reply]() {
            bool success = (reply->error() == QModbusDevice::NoError);
            if (!success) {
                qWarning() << m_identifier << "Write error:" << reply->errorString();
            }
            m_hasPendingWrites = false;
            emit registerWritten(success);
            reply->deleteLater();
        });
    }
}

void Plc42Device::setPollInterval(int intervalMs) {
    m_pollTimer->setInterval(intervalMs);
}
