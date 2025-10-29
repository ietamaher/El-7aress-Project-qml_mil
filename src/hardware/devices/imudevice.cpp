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
      m_pollTimer(new QTimer(this)),
      m_communicationWatchdog(new QTimer(this))
{
    connect(m_pollTimer, &QTimer::timeout, this, &ImuDevice::pollTimerTimeout);

    m_communicationWatchdog->setSingleShot(false);
    m_communicationWatchdog->setInterval(COMMUNICATION_TIMEOUT_MS);
    connect(m_communicationWatchdog, &QTimer::timeout,
            this, &ImuDevice::onCommunicationWatchdogTimeout);
}

ImuDevice::~ImuDevice() {
    m_pollTimer->stop();
    m_communicationWatchdog->stop();
}

void ImuDevice::setDependencies(Transport* transport,
                                 ImuProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Don't listen to transport connectionStateChanged - we manage connection via watchdog
}

bool ImuDevice::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Transport should already be opened by SystemController
    qDebug() << m_identifier << "initializing...";

    // Get poll interval from config (default 50ms)
    QJsonObject config = property("config").toJsonObject();
    int pollInterval = config["pollIntervalMs"].toInt(50);

    setState(DeviceState::Online);

    // Start polling and watchdog
    m_pollTimer->start(pollInterval);
    m_communicationWatchdog->start();

    qDebug() << m_identifier << "initialized successfully with poll interval:" << pollInterval << "ms";
    return true;
}

void ImuDevice::shutdown() {
    m_pollTimer->stop();
    m_communicationWatchdog->stop();

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
        setConnectionState(false);
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

        // We received valid data - device is connected and communicating
        setConnectionState(true);
        resetCommunicationWatchdog();

        // Update with new data
        auto newData = std::make_shared<ImuData>(dataMsg->data());
        updateData(newData);
        emit imuDataChanged(*newData);
    }
}

void ImuDevice::setPollInterval(int intervalMs) {
    m_pollTimer->setInterval(intervalMs);
}

void ImuDevice::resetCommunicationWatchdog() {
    m_communicationWatchdog->start();
}

void ImuDevice::setConnectionState(bool connected) {
    auto currentData = data();
    if (currentData->isConnected != connected) {
        auto newData = std::make_shared<ImuData>(*currentData);
        newData->isConnected = connected;
        updateData(newData);
        emit imuDataChanged(*newData);

        if (connected) {
            qDebug() << m_identifier << "connected";
        } else {
            qWarning() << m_identifier << "disconnected";
        }
    }
}

void ImuDevice::onCommunicationWatchdogTimeout() {
    qWarning() << m_identifier << "Communication timeout - no data received for"
               << COMMUNICATION_TIMEOUT_MS << "ms";
    setConnectionState(false);
}
