#include "nightcameracontroldevice.h"
#include "../interfaces/Transport.h"
#include "../protocols/NightCameraProtocolParser.h"
#include "../messages/NightCameraMessage.h"
#include <QJsonObject>
#include <QDebug>

NightCameraControlDevice::NightCameraControlDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<NightCameraData>(parent),
      m_identifier(identifier),
      m_statusCheckTimer(new QTimer(this))
{
    connect(m_statusCheckTimer, &QTimer::timeout, this, &NightCameraControlDevice::checkCameraStatus);
}

NightCameraControlDevice::~NightCameraControlDevice() {
    shutdown();
}

void NightCameraControlDevice::setDependencies(Transport* transport, NightCameraProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    m_transport->setParent(this);
    m_parser->setParent(this);

    connect(m_transport, &Transport::frameReceived, this, &NightCameraControlDevice::processFrame);
    connect(m_transport, &Transport::connectionStateChanged, this, [this](bool connected) {
        auto newData = std::make_shared<NightCameraData>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit nightCameraDataChanged(*newData);

        if (connected) {
            m_statusCheckTimer->start(5000);
        } else {
            m_statusCheckTimer->stop();
        }
    });
}

bool NightCameraControlDevice::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Transport should already be opened by SystemController
    qDebug() << m_identifier << "initialized successfully";

    setState(DeviceState::Online);
    m_statusCheckTimer->start(5000);
    getCameraStatus();
    return true;
}

void NightCameraControlDevice::shutdown() {
    m_statusCheckTimer->stop();

    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }

    setState(DeviceState::Offline);
}

void NightCameraControlDevice::processFrame(const QByteArray& frame) {
    if (!m_parser) return;

    auto messages = m_parser->parse(frame);
    for (const auto& msg : messages) {
        if (msg) processMessage(*msg);
    }
}

void NightCameraControlDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::NightCameraDataType) {
        auto const* dataMsg = static_cast<const NightCameraDataMessage*>(&message);

        // Merge with current data
        auto currentData = data();
        auto newData = std::make_shared<NightCameraData>(*currentData);

        const NightCameraData& partial = dataMsg->data();
        newData->cameraStatus = partial.cameraStatus;
        newData->errorState = partial.errorState;
        if (!partial.ffcInProgress) {
            newData->ffcInProgress = false;
        }

        updateData(newData);
        emit nightCameraDataChanged(*newData);
    }
}

void NightCameraControlDevice::sendCommand(quint8 function, const QByteArray& cmdData) {
    if (state() != DeviceState::Online || !m_transport || !m_parser) return;

    QByteArray command = m_parser->buildCommand(function, cmdData);
    m_transport->sendFrame(command);
}

void NightCameraControlDevice::performFFC() {
    auto newData = std::make_shared<NightCameraData>(*data());
    newData->ffcInProgress = true;
    updateData(newData);
    emit nightCameraDataChanged(*newData);

    sendCommand(0x0B, QByteArray::fromHex("0001"));
}

void NightCameraControlDevice::setDigitalZoom(quint8 zoomLevel) {
    auto newData = std::make_shared<NightCameraData>(*data());
    newData->digitalZoomEnabled = (zoomLevel > 0);
    newData->digitalZoomLevel = zoomLevel;
    newData->currentHFOV = (zoomLevel > 0) ? 5.2 : 10.4;
    updateData(newData);
    emit nightCameraDataChanged(*newData);

    QByteArray zoomArg = (zoomLevel > 0) ? QByteArray::fromHex("0004") : QByteArray::fromHex("0000");
    sendCommand(0x0F, zoomArg);
}

void NightCameraControlDevice::setVideoModeLUT(quint16 mode) {
    auto newData = std::make_shared<NightCameraData>(*data());
    newData->videoMode = mode;
    updateData(newData);
    emit nightCameraDataChanged(*newData);

    if (mode > 12) mode = 12;
    QByteArray modeArg = QByteArray::fromHex(QByteArray::number(mode, 16).rightJustified(4, '0'));
    sendCommand(0x10, modeArg);
}

void NightCameraControlDevice::getCameraStatus() {
    sendCommand(0x06, QByteArray::fromHex("0000"));
}

void NightCameraControlDevice::checkCameraStatus() {
    getCameraStatus();
}
