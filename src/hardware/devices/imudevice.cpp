#include "imudevice.h"
#include "../interfaces/Transport.h"
#include "../protocols/Imu3DMGX3ProtocolParser.h"
#include "../messages/ImuMessage.h"
#include <QDebug>

ImuDevice::ImuDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<ImuData>(parent),
      m_identifier(identifier),
      m_communicationWatchdog(new QTimer(this)),
      m_initializationTimer(new QTimer(this)),
      m_temperatureQueryTimer(new QTimer(this))
{
    // Communication watchdog - detects loss of data stream
    m_communicationWatchdog->setSingleShot(false);
    m_communicationWatchdog->setInterval(COMMUNICATION_TIMEOUT_MS);
    connect(m_communicationWatchdog, &QTimer::timeout,
            this, &ImuDevice::onCommunicationWatchdogTimeout);

    // Initialization timeout - handles long operations like gyro bias capture
    m_initializationTimer->setSingleShot(true);
    connect(m_initializationTimer, &QTimer::timeout,
            this, &ImuDevice::onInitializationTimeout);

    // Temperature query timer - periodic status monitoring
    m_temperatureQueryTimer->setSingleShot(false);
    m_temperatureQueryTimer->setInterval(TEMPERATURE_QUERY_INTERVAL_MS);
    connect(m_temperatureQueryTimer, &QTimer::timeout,
            this, &ImuDevice::onTemperatureQueryTimeout);
}

ImuDevice::~ImuDevice() {
    if (m_communicationWatchdog) {
        m_communicationWatchdog->stop();
    }
    if (m_initializationTimer) {
        m_initializationTimer->stop();
    }
    if (m_temperatureQueryTimer) {
        m_temperatureQueryTimer->stop();
    }
}

void ImuDevice::setDependencies(Transport* transport,
                                 Imu3DMGX3ProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;

    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);

    // Connect to transport's frameReceived signal
    connect(m_transport, SIGNAL(frameReceived(QByteArray)),
            this, SLOT(onFrameReceived(QByteArray)));
}

bool ImuDevice::initialize() {
    setState(DeviceState::Initializing);

    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Transport should already be opened by SystemController
    qDebug() << m_identifier << "initializing 3DM-GX3-25...";

    // Get sampling rate from config (default 100Hz)
    QJsonObject config = property("config").toJsonObject();
    m_samplingRateHz = config["samplingRateHz"].toInt(100);

    // Start initialization sequence
    startInitializationSequence();

    return true;
}

void ImuDevice::shutdown() {
    if (m_communicationWatchdog) {
        m_communicationWatchdog->stop();
    }
    if (m_initializationTimer) {
        m_initializationTimer->stop();
    }
    if (m_temperatureQueryTimer) {
        m_temperatureQueryTimer->stop();
    }

    // Stop continuous mode
    if (m_transport && m_parser) {
        QByteArray stopCmd = Imu3DMGX3ProtocolParser::createStopContinuousModeCommand();
        QMetaObject::invokeMethod(m_transport, "sendFrame",
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, stopCmd));
    }

    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }

    setState(DeviceState::Offline);
}

void ImuDevice::startInitializationSequence() {
    m_initState = InitState::WaitingForGyroBias;

    qDebug() << m_identifier << "Step 1: Capturing gyro bias (device must be stationary)...";
    sendCaptureGyroBiasCommand();

    // Set timeout for gyro bias capture (takes ~10 seconds)
    m_initializationTimer->setInterval(GYRO_BIAS_TIMEOUT_MS);
    m_initializationTimer->start();
}

void ImuDevice::sendCaptureGyroBiasCommand() {
    if (!m_transport || !m_parser) return;

    QByteArray cmd = Imu3DMGX3ProtocolParser::createCaptureGyroBiasCommand();
    QMetaObject::invokeMethod(m_transport, "sendFrame",
                              Qt::DirectConnection,
                              Q_ARG(QByteArray, cmd));
}

void ImuDevice::sendSamplingSettingsCommand() {
    if (!m_transport || !m_parser) return;

    // Calculate decimation value from desired sampling rate
    // 0 = 1000Hz, 1 = 500Hz, 4 = 200Hz, 9 = 100Hz, 19 = 50Hz
    quint16 decimation;
    if (m_samplingRateHz >= 1000) {
        decimation = 0;
    } else if (m_samplingRateHz >= 500) {
        decimation = 1;
    } else if (m_samplingRateHz >= 200) {
        decimation = 4;
    } else if (m_samplingRateHz >= 100) {
        decimation = 9;
    } else {
        decimation = 19; // 50Hz
    }

    qDebug() << m_identifier << "Step 2: Setting sampling rate to"
             << (1000 / (decimation + 1)) << "Hz (decimation =" << decimation << ")";

    QByteArray cmd = Imu3DMGX3ProtocolParser::createSamplingSettingsCommand(decimation);
    QMetaObject::invokeMethod(m_transport, "sendFrame",
                              Qt::DirectConnection,
                              Q_ARG(QByteArray, cmd));
}

void ImuDevice::startContinuousMode() {
    if (!m_transport || !m_parser) return;

    qDebug() << m_identifier << "Step 3: Starting continuous mode (0xCF: Euler + Rates)...";

    QByteArray cmd = Imu3DMGX3ProtocolParser::createContinuousModeCommand();
    QMetaObject::invokeMethod(m_transport, "sendFrame",
                              Qt::DirectConnection,
                              Q_ARG(QByteArray, cmd));

    m_initState = InitState::Running;
    setState(DeviceState::Online);

    // Start communication watchdog
    m_communicationWatchdog->start();

    // Start periodic temperature monitoring
    m_temperatureQueryTimer->start();

    qDebug() << m_identifier << "Initialization complete! Streaming orientation data...";
}

void ImuDevice::onFrameReceived(const QByteArray& frame) {
    if (!m_parser) return;

    // Parse incoming data stream
    auto messages = m_parser->parse(frame);

    // Process each message
    for (const auto& msg : messages) {
        if (msg) {
            processMessage(*msg);
        }
    }

    // Handle initialization state machine
    if (m_initState == InitState::WaitingForGyroBias) {
        // Gyro bias capture completed (parser detected 0xCD response)
        m_initializationTimer->stop();
        m_initState = InitState::WaitingForSamplingRate;
        sendSamplingSettingsCommand();
    } else if (m_initState == InitState::WaitingForSamplingRate) {
        // Sampling rate configured (parser detected 0xDB response)
        m_initState = InitState::StartingContinuousMode;
        startContinuousMode();
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

void ImuDevice::setSamplingRate(int rateHz) {
    m_samplingRateHz = qBound(50, rateHz, 1000);
    qDebug() << m_identifier << "Sampling rate set to" << m_samplingRateHz << "Hz";
}

void ImuDevice::onInitializationTimeout() {
    qWarning() << m_identifier << "Initialization timeout in state" << static_cast<int>(m_initState);

    if (m_initState == InitState::WaitingForGyroBias) {
        qWarning() << m_identifier << "Gyro bias capture timed out - proceeding anyway";
        m_initState = InitState::WaitingForSamplingRate;
        sendSamplingSettingsCommand();
    } else {
        qCritical() << m_identifier << "Initialization failed!";
        setState(DeviceState::Error);
    }
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

void ImuDevice::sendReadTemperaturesCommand() {
    if (!m_transport || !m_parser) return;

    QByteArray cmd = Imu3DMGX3ProtocolParser::createReadTemperaturesCommand();
    QMetaObject::invokeMethod(m_transport, "sendFrame",
                              Qt::DirectConnection,
                              Q_ARG(QByteArray, cmd));
}

void ImuDevice::onTemperatureQueryTimeout() {
    // Only query temperature if device is running normally
    if (m_initState == InitState::Running && state() == DeviceState::Online) {
        sendReadTemperaturesCommand();
    }
}
