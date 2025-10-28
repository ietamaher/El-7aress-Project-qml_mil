#include "plc21device.h"
#include "../interfaces/Transport.h"
#include "../protocols/Plc21ProtocolParser.h"
#include "../messages/Plc21Message.h"
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QDebug>

Plc21Device::Plc21Device(const QString& identifier, QObject* parent)
    : TemplatedDevice<Plc21PanelData>(parent),
      m_identifier(identifier),
      m_pollTimer(new QTimer(this))
{
    connect(m_pollTimer, &QTimer::timeout, this, &Plc21Device::pollTimerTimeout);
}

Plc21Device::~Plc21Device() {
    m_pollTimer->stop();
}

void Plc21Device::setDependencies(Transport* transport,
                                   Plc21ProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Connect transport signals
    connect(m_transport, &Transport::connectionStateChanged,
            this, [this](bool connected) {
        auto newData = std::make_shared<Plc21PanelData>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit panelDataChanged(*newData);
    });
}

bool Plc21Device::initialize() {
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

void Plc21Device::shutdown() {
    m_pollTimer->stop();

    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }

    setState(DeviceState::Offline);
}

void Plc21Device::pollTimerTimeout() {
    // Read digital inputs (discrete inputs)
    sendReadRequest(Plc21Registers::DIGITAL_INPUTS_START_ADDR,
                    Plc21Registers::DIGITAL_INPUTS_COUNT,
                    true);

    // Read analog inputs (holding registers)
    sendReadRequest(Plc21Registers::ANALOG_INPUTS_START_ADDR,
                    Plc21Registers::ANALOG_INPUTS_COUNT,
                    false);
}

void Plc21Device::sendReadRequest(int startAddress, int count, bool isDiscreteInputs) {
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

void Plc21Device::onModbusReplyReady(QModbusReply* reply) {
    if (!reply || !m_parser) {
        if (reply) reply->deleteLater();
        return;
    }

    if (reply->error() != QModbusDevice::NoError) {
        qWarning() << m_identifier << "Modbus error:" << reply->errorString();
        auto newData = std::make_shared<Plc21PanelData>(*data());
        newData->isConnected = false;
        updateData(newData);
        emit panelDataChanged(*newData);
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

void Plc21Device::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::Plc21DataType) {
        auto const* dataMsg = static_cast<const Plc21DataMessage*>(&message);
        mergePartialData(dataMsg->data());
    }
}

void Plc21Device::mergePartialData(const Plc21PanelData& partialData) {
    auto currentData = data();
    auto newData = std::make_shared<Plc21PanelData>(*currentData);
    newData->isConnected = true;

    bool dataChanged = false;

    // Merge digital inputs (always update if they're part of the partial data)
    // Since the parser sets default values, we need a smarter merge strategy
    // For now, we'll assume any non-default value in partialData should be merged

    // Digital inputs - always merge (parser provides full set)
    if (partialData.armGunSW != currentData->armGunSW ||
        partialData.loadAmmunitionSW != currentData->loadAmmunitionSW ||
        partialData.enableStationSW != currentData->enableStationSW ||
        partialData.homePositionSW != currentData->homePositionSW ||
        partialData.enableStabilizationSW != currentData->enableStabilizationSW ||
        partialData.authorizeSw != currentData->authorizeSw ||
        partialData.switchCameraSW != currentData->switchCameraSW ||
        partialData.menuUpSW != currentData->menuUpSW ||
        partialData.menuDownSW != currentData->menuDownSW ||
        partialData.menuValSw != currentData->menuValSw) {

        newData->armGunSW = partialData.armGunSW;
        newData->loadAmmunitionSW = partialData.loadAmmunitionSW;
        newData->enableStationSW = partialData.enableStationSW;
        newData->homePositionSW = partialData.homePositionSW;
        newData->enableStabilizationSW = partialData.enableStabilizationSW;
        newData->authorizeSw = partialData.authorizeSw;
        newData->switchCameraSW = partialData.switchCameraSW;
        newData->menuUpSW = partialData.menuUpSW;
        newData->menuDownSW = partialData.menuDownSW;
        newData->menuValSw = partialData.menuValSw;
        dataChanged = true;
    }

    // Analog inputs - merge if different
    if (partialData.speedSW != currentData->speedSW ||
        partialData.fireMode != currentData->fireMode ||
        partialData.panelTemperature != currentData->panelTemperature) {

        newData->speedSW = partialData.speedSW;
        newData->fireMode = partialData.fireMode;
        newData->panelTemperature = partialData.panelTemperature;
        dataChanged = true;
    }

    if (dataChanged) {
        updateData(newData);
        emit panelDataChanged(*newData);
    }
}

//================================================================================
// PUBLIC API - OUTPUT CONTROL
//================================================================================

void Plc21Device::setDigitalOutputs(const QVector<bool>& outputs) {
    m_digitalOutputs = outputs;
    sendWriteRequest(Plc21Registers::DIGITAL_OUTPUTS_START_ADDR, outputs);
}

void Plc21Device::writeDigitalOutput(int index, bool value) {
    if (index < 0 || index >= Plc21Registers::DIGITAL_OUTPUTS_COUNT) {
        qWarning() << m_identifier << "Invalid output index:" << index;
        return;
    }

    // Ensure vector has enough elements
    while (m_digitalOutputs.size() <= index) {
        m_digitalOutputs.append(false);
    }

    m_digitalOutputs[index] = value;
    sendWriteRequest(Plc21Registers::DIGITAL_OUTPUTS_START_ADDR, m_digitalOutputs);
}

void Plc21Device::sendWriteRequest(int startAddress, const QVector<bool>& values) {
    if (state() != DeviceState::Online || !m_transport) return;

    QModbusDataUnit writeUnit(QModbusDataUnit::Coils, startAddress, values.size());
    for (int i = 0; i < values.size(); ++i) {
        writeUnit.setValue(i, values[i] ? 1 : 0);
    }

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
            emit digitalOutputWritten(success);
            reply->deleteLater();
        });
    }
}

void Plc21Device::setPollInterval(int intervalMs) {
    m_pollTimer->setInterval(intervalMs);
}
