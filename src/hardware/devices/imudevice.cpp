#include "imudevice.h"
#include "../interfaces/Transport.h"
#include "../protocols/ImuProtocolParser.h"
#include "../messages/ImuMessage.h"
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QDebug>

ImuDevice::ImuDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<ImuData>(parent),
      m_identifier(identifier),
      m_pollTimer(new QTimer(this))
{
    connect(m_pollTimer, &QTimer::timeout, this, &ImuDevice::pollTimerTimeout);
}

ImuDevice::~ImuDevice() {
    m_pollTimer->stop();
}

void ImuDevice::setDependencies(Transport* transport,
                                 ImuProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Connect transport signals
    connect(m_transport, &Transport::connectionStateChanged,
            this, [this](bool connected) {
        auto newData = std::make_shared<ImuData>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit imuDataChanged(*newData);
    });
}

bool ImuDevice::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Get configuration from device property
    QJsonObject config = property("config").toJsonObject();
    int pollInterval = config["pollIntervalMs"].toInt(50);
    config["parity"] = "none"; // SST810 uses NO parity

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

void ImuDevice::shutdown() {
    m_pollTimer->stop();

    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }

    setState(DeviceState::Offline);
}

void ImuDevice::pollTimerTimeout() {
    // Read all 18 Input Registers in a single request
    sendReadRequest();
}

void ImuDevice::sendReadRequest() {
    if (state() != DeviceState::Online || !m_transport) return;

    // Cast to ModbusTransport to access Modbus-specific methods
    auto modbusTransport = qobject_cast<QModbusRtuSerialClient*>(
        m_transport->property("client").value<QObject*>());

    if (!modbusTransport) return;

    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters,
                             ImuRegisters::ALL_DATA_START_ADDR,
                             ImuRegisters::ALL_DATA_REG_COUNT);

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

void ImuDevice::onModbusReplyReady(QModbusReply* reply) {
    if (!reply || !m_parser) {
        if (reply) reply->deleteLater();
        return;
    }

    if (reply->error() != QModbusDevice::NoError) {
        qWarning() << m_identifier << "Modbus error:" << reply->errorString();
        auto newData = std::make_shared<ImuData>(*data());
        newData->isConnected = false;
        updateData(newData);
        emit imuDataChanged(*newData);
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

void ImuDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::ImuDataType) {
        auto const* dataMsg = static_cast<const ImuDataMessage*>(&message);

        // Update with new data
        auto newData = std::make_shared<ImuData>(dataMsg->data());
        updateData(newData);
        emit imuDataChanged(*newData);
    }
}

void ImuDevice::setPollInterval(int intervalMs) {
    m_pollTimer->setInterval(intervalMs);
}
