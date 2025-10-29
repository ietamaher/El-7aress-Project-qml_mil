// devices/LRFDevice.cpp
#include "hardware/devices/lrfdevice.h"
#include "hardware/interfaces/Transport.h"
#include "hardware/protocols/LrfProtocolParser.h"
#include "hardware/protocols/LrfMessage.h"
#include <QTimer>
#include <QJsonObject>
#include <QDebug>

LRFDevice::LRFDevice(QObject* parent)
    : TemplatedDevice<LrfData>(parent),
      m_transport(nullptr),
      m_parser(nullptr),
      m_commandResponseTimer(new QTimer(this))
{
    m_commandResponseTimer->setSingleShot(true);
    connect(m_commandResponseTimer, &QTimer::timeout, 
            this, &LRFDevice::handleCommandResponseTimeout);
}

LRFDevice::~LRFDevice() {
    shutdown();
}

IDevice::DeviceType LRFDevice::type() const { 
    return DeviceType::LRF; 
}

void LRFDevice::setDependencies(Transport* transport, LrfProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Connect transport signals
    connect(m_transport, &Transport::frameReceived, 
            this, &LRFDevice::processFrame);
}

bool LRFDevice::initialize() {
    setState(DeviceState::Initializing);

    // Safety check
    if (!m_transport || !m_parser) {
        qCritical() << "LRF missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Transport should already be opened by SystemController
    qDebug() << "LRF initialized successfully";

    setState(DeviceState::Online);
    sendSelfCheck(); // Initial status check
    return true;
}

void LRFDevice::shutdown() {
    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }
    setState(DeviceState::Offline);
}

// Command API implementations
void LRFDevice::sendSelfCheck() { 
    sendCommand(0x01); 
}

void LRFDevice::sendSingleRanging() { 
    sendCommand(0x0B); 
}

void LRFDevice::sendContinuousRanging1Hz() { 
    sendCommand(0x0C); 
}

void LRFDevice::sendContinuousRanging5Hz() { 
    sendCommand(0x02); 
}

void LRFDevice::sendContinuousRanging10Hz() { 
    sendCommand(0x04); 
}

void LRFDevice::stopRanging() { 
    sendCommand(0x05); 
}

void LRFDevice::queryAccumulatedLaserCount() { 
    sendCommand(0x0A); 
}

void LRFDevice::queryProductInfo() { 
    sendCommand(0x10); 
}

void LRFDevice::queryTemperature() { 
    sendCommand(0x06); 
}

void LRFDevice::sendCommand(quint8 commandCode) {
    if (state() != DeviceState::Online) return;
    if (!m_parser || !m_transport) return;

    QByteArray packet = m_parser->buildCommand(commandCode);
    m_commandResponseTimer->start(600); // 600ms timeout
    m_transport->sendFrame(packet);
}

void LRFDevice::processFrame(const QByteArray& frame) {
    if (!m_parser) return;
    
    const auto messages = m_parser->parse(frame);
    if (!messages.empty()) {
        m_commandResponseTimer->stop();
    }

    for (const auto& msgPtr : messages) {
        if (msgPtr) {
            processMessage(*msgPtr);
        }
    }
}

void LRFDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::LrfDataType) {
        auto const* lrfMsg = static_cast<const LrfDataMessage*>(&message);
        auto newData = std::make_shared<LrfData>(lrfMsg->data());
        newData->isConnected = true;
        updateData(newData);
        emit lrfDataChanged(newData);
    } 
    else if (message.typeId() == Message::Type::LrfInfoType) {
        auto const* info = static_cast<const LrfInfoMessage*>(&message);
        emit productInfoReceived(info->productId(), info->softwareVersion());
    }
}

void LRFDevice::handleCommandResponseTimeout() {
    qWarning() << "LRF command response timeout!";
    
    auto currentData = data();
    auto newData = std::make_shared<LrfData>(*currentData);
    newData->isFault = true;
    updateData(newData);
    emit lrfDataChanged(newData);
    emit responseTimeout();
}
