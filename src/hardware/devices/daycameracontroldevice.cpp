#include "daycameracontroldevice.h"
#include "../interfaces/Transport.h"
#include "../protocols/DayCameraProtocolParser.h"
#include "../messages/DayCameraMessage.h"
#include <QJsonObject>
#include <QDebug>

DayCameraControlDevice::DayCameraControlDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<DayCameraData>(parent), m_identifier(identifier) {}

DayCameraControlDevice::~DayCameraControlDevice() {
    shutdown();
}

void DayCameraControlDevice::setDependencies(Transport* transport, DayCameraProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    m_transport->setParent(this);
    m_parser->setParent(this);

    connect(m_transport, &Transport::frameReceived, this, &DayCameraControlDevice::processFrame);
    connect(m_transport, &Transport::connectionStateChanged, this, [this](bool connected) {
        auto newData = std::make_shared<DayCameraData>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit dayCameraDataChanged(*newData);
    });
}

bool DayCameraControlDevice::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    QJsonObject config = property("config").toJsonObject();
    config["baudRate"] = 9600;

    if (m_transport->open(config)) {
        setState(DeviceState::Online);
        getCameraStatus();
        return true;
    }

    setState(DeviceState::Error);
    return false;
}

void DayCameraControlDevice::shutdown() {
    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }
    setState(DeviceState::Offline);
}

void DayCameraControlDevice::processFrame(const QByteArray& frame) {
    if (!m_parser) return;

    auto messages = m_parser->parse(frame);
    for (const auto& msg : messages) {
        if (msg) processMessage(*msg);
    }
}

void DayCameraControlDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::DayCameraDataType) {
        auto const* dataMsg = static_cast<const DayCameraDataMessage*>(&message);

        // Merge with current data
        auto currentData = data();
        auto newData = std::make_shared<DayCameraData>(*currentData);

        const DayCameraData& partial = dataMsg->data();
        if (partial.zoomPosition != 0) {
            newData->zoomPosition = partial.zoomPosition;
            newData->currentHFOV = partial.currentHFOV;
        }
        if (partial.focusPosition != 0) {
            newData->focusPosition = partial.focusPosition;
        }

        updateData(newData);
        emit dayCameraDataChanged(*newData);
    }
}

void DayCameraControlDevice::sendCommand(quint8 cmd1, quint8 cmd2, quint8 data1, quint8 data2) {
    if (state() != DeviceState::Online || !m_transport || !m_parser) return;

    QByteArray command = m_parser->buildCommand(cmd1, cmd2, data1, data2);
    m_transport->sendFrame(command);
}

void DayCameraControlDevice::zoomIn() {
    auto newData = std::make_shared<DayCameraData>(*data());
    newData->zoomMovingIn = true;
    newData->zoomMovingOut = false;
    updateData(newData);
    emit dayCameraDataChanged(*newData);
    sendCommand(0x00, 0x20);
}

void DayCameraControlDevice::zoomOut() {
    auto newData = std::make_shared<DayCameraData>(*data());
    newData->zoomMovingOut = true;
    newData->zoomMovingIn = false;
    updateData(newData);
    emit dayCameraDataChanged(*newData);
    sendCommand(0x00, 0x40);
}

void DayCameraControlDevice::zoomStop() {
    auto newData = std::make_shared<DayCameraData>(*data());
    newData->zoomMovingIn = false;
    newData->zoomMovingOut = false;
    updateData(newData);
    emit dayCameraDataChanged(*newData);
    sendCommand(0x00, 0x00);
}

void DayCameraControlDevice::setZoomPosition(quint16 position) {
    quint8 high = (position >> 8) & 0xFF;
    quint8 low = position & 0xFF;
    sendCommand(0x00, 0xA7, high, low);
}

void DayCameraControlDevice::focusNear() {
    sendCommand(0x01, 0x00);
}

void DayCameraControlDevice::focusFar() {
    sendCommand(0x00, 0x02);
}

void DayCameraControlDevice::focusStop() {
    sendCommand(0x00, 0x00);
}

void DayCameraControlDevice::setFocusAuto(bool enabled) {
    auto newData = std::make_shared<DayCameraData>(*data());
    newData->autofocusEnabled = enabled;
    updateData(newData);
    emit dayCameraDataChanged(*newData);

    sendCommand(0x01, enabled ? 0x63 : 0x64);
}

void DayCameraControlDevice::setFocusPosition(quint16 position) {
    quint8 high = (position >> 8) & 0xFF;
    quint8 low = position & 0xFF;
    sendCommand(0x00, 0x63, high, low);
}

void DayCameraControlDevice::getCameraStatus() {
    sendCommand(0x00, 0xA7);
}
