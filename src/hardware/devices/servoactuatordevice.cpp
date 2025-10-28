#include "hardware/devices/servoactuatordevice.h"
#include "hardware/interfaces/Transport.h"
#include "hardware/protocols/ServoActuatorProtocolParser.h"
#include "hardware/messages/ServoActuatorMessage.h"
#include <QDebug>

ServoActuatorDevice::ServoActuatorDevice(const QString& identifier, QObject* parent)
    : TemplatedDevice<ServoActuatorData>(parent),
      m_identifier(identifier),
      m_commandTimeoutTimer(new QTimer(this))
{
    m_commandTimeoutTimer->setSingleShot(true);
    connect(m_commandTimeoutTimer, &QTimer::timeout, 
            this, &ServoActuatorDevice::handleCommandTimeout);
}

ServoActuatorDevice::~ServoActuatorDevice() {
    m_commandTimeoutTimer->stop();
}

void ServoActuatorDevice::setDependencies(Transport* transport, 
                                           ServoActuatorProtocolParser* parser) {
    m_transport = transport;
    m_parser = parser;
    
    // Parent them to this device for lifetime management
    m_transport->setParent(this);
    m_parser->setParent(this);
    
    // Connect transport signals
    connect(m_transport, &Transport::frameReceived, 
            this, &ServoActuatorDevice::onFrameReceived);
    
    connect(m_transport, &Transport::connectionStateChanged, 
            this, [this](bool connected) {
        auto newData = std::make_shared<ServoActuatorData>(*data());
        newData->isConnected = connected;
        updateData(newData);
        emit actuatorDataChanged(*newData);
    });
}

bool ServoActuatorDevice::initialize() {
    setState(DeviceState::Initializing);
    
    if (!m_transport || !m_parser) {
        qCritical() << m_identifier << "missing dependencies!";
        setState(DeviceState::Error);
        return false;
    }

    // Get configuration from device property
    QJsonObject config = property("config").toJsonObject();
    config["baudRate"] = 115200; // Fixed baud rate for actuator
    
    qDebug() << m_identifier << "initializing...";
    
    // Open transport
    if (m_transport->open(config)) {
        setState(DeviceState::Online);
        qDebug() << m_identifier << "initialized successfully";
        return true;
    }
    
    qCritical() << m_identifier << "failed to initialize transport";
    setState(DeviceState::Error);
    return false;
}

void ServoActuatorDevice::shutdown() {
    m_commandTimeoutTimer->stop();
    m_commandQueue.clear();
    m_pendingCommand.clear();
    
    if (m_transport) {
        QMetaObject::invokeMethod(m_transport, "close", Qt::QueuedConnection);
    }
    
    setState(DeviceState::Offline);
}

void ServoActuatorDevice::onFrameReceived(const QByteArray& frame) {
    if (!m_parser) return;
    
    // Parse frame into messages
    auto messages = m_parser->parse(frame);
    
    // Process each message
    for (const auto& msg : messages) {
        if (msg) {
            processMessage(*msg);
        }
    }
}

void ServoActuatorDevice::processMessage(const Message& message) {
    if (message.typeId() == Message::Type::ServoActuatorDataType) {
        auto const* dataMsg = static_cast<const ServoActuatorDataMessage*>(&message);
        mergePartialData(dataMsg->data());
        
    } else if (message.typeId() == Message::Type::ServoActuatorAckType) {
        auto const* ackMsg = static_cast<const ServoActuatorAckMessage*>(&message);
        
        // Stop timeout timer
        m_commandTimeoutTimer->stop();
        m_pendingCommand.clear();
        
        emit commandAcknowledged(ackMsg->command());
        
        // Process next command if any
        if (!m_commandQueue.isEmpty()) {
            QTimer::singleShot(INTER_COMMAND_DELAY_MS, this, 
                               &ServoActuatorDevice::processNextCommand);
        }
        
    } else if (message.typeId() == Message::Type::ServoActuatorNackType) {
        auto const* nackMsg = static_cast<const ServoActuatorNackMessage*>(&message);
        
        m_commandTimeoutTimer->stop();
        m_pendingCommand.clear();
        
        emit commandError(QString("Command '%1' rejected: %2")
                          .arg(nackMsg->command(), nackMsg->errorDetails()));
        
        // Process next command if any
        if (!m_commandQueue.isEmpty()) {
            QTimer::singleShot(INTER_COMMAND_DELAY_MS, this, 
                               &ServoActuatorDevice::processNextCommand);
        }
        
    } else if (message.typeId() == Message::Type::ServoActuatorCriticalFaultType) {
        auto const* faultMsg = static_cast<const ServoActuatorCriticalFaultMessage*>(&message);
        emit criticalFaultOccurred(faultMsg->criticalFaults());
    }
}

void ServoActuatorDevice::mergePartialData(const ServoActuatorData& partialData) {
    auto currentData = data();
    auto newData = std::make_shared<ServoActuatorData>(*currentData);
    newData->isConnected = true;
    
    bool dataChanged = false;
    
    // Merge only non-zero/non-default values
    if (partialData.position_mm != 0.0 && 
        !qFuzzyCompare(partialData.position_mm, currentData->position_mm)) {
        newData->position_mm = partialData.position_mm;
        dataChanged = true;
    }
    
    if (partialData.velocity_mm_s != 0.0 && 
        !qFuzzyCompare(partialData.velocity_mm_s, currentData->velocity_mm_s)) {
        newData->velocity_mm_s = partialData.velocity_mm_s;
        dataChanged = true;
    }
    
    if (partialData.temperature_c != 0.0 && 
        !qFuzzyCompare(partialData.temperature_c, currentData->temperature_c)) {
        newData->temperature_c = partialData.temperature_c;
        dataChanged = true;
    }
    
    if (partialData.busVoltage_v != 0.0 && 
        !qFuzzyCompare(partialData.busVoltage_v, currentData->busVoltage_v)) {
        newData->busVoltage_v = partialData.busVoltage_v;
        dataChanged = true;
    }
    
    if (partialData.torque_percent != 0.0 && 
        !qFuzzyCompare(partialData.torque_percent, currentData->torque_percent)) {
        newData->torque_percent = partialData.torque_percent;
        dataChanged = true;
    }
    
    if (partialData.status != currentData->status) {
        newData->status = partialData.status;
        dataChanged = true;
    }
    
    if (dataChanged) {
        updateData(newData);
        emit actuatorDataChanged(*newData);
    }
}

void ServoActuatorDevice::sendCommand(const QString& command) {
    if (state() != DeviceState::Online || !m_transport || !m_parser) {
        qWarning() << m_identifier << "Cannot send command: device not ready";
        return;
    }

    // If a command is already pending, queue this one
    if (!m_pendingCommand.isEmpty()) {
        m_commandQueue.enqueue(command);
        return;
    }

    // Build command with checksum using parser
    QByteArray fullCommand = m_parser->buildCommand(command);
    
    m_pendingCommand = command;
    m_transport->sendFrame(fullCommand);
    m_commandTimeoutTimer->start(COMMAND_TIMEOUT_MS);
    
    // Store pending command in parser for response routing
    m_parser->setProperty("pendingCommand", command);
}

void ServoActuatorDevice::processNextCommand() {
    if (!m_commandQueue.isEmpty() && m_pendingCommand.isEmpty()) {
        sendCommand(m_commandQueue.dequeue());
    }
}

void ServoActuatorDevice::handleCommandTimeout() {
    qWarning() << m_identifier << "Timeout waiting for response to:" << m_pendingCommand;
    emit commandError(QString("Timeout on command: %1").arg(m_pendingCommand));
    
    m_pendingCommand.clear();
    
    // Try next command if any
    if (!m_commandQueue.isEmpty()) {
        QTimer::singleShot(INTER_COMMAND_DELAY_MS, this, 
                           &ServoActuatorDevice::processNextCommand);
    }
}

//================================================================================
// PUBLIC API - MOTION CONTROL
//================================================================================

void ServoActuatorDevice::moveToPosition(double position_mm) {
    // Use parser for unit conversion
    int counts = m_parser->millimetersToSensorCounts(position_mm);
    sendCommand(QString("TA%1").arg(counts));
}

void ServoActuatorDevice::setMaxSpeed(double speed_mm_s) {
    int counts = m_parser->speedToSensorCounts(speed_mm_s);
    sendCommand(QString("SP%1").arg(counts));
}

void ServoActuatorDevice::setAcceleration(double accel_mm_s2) {
    int counts = m_parser->accelToSensorCounts(accel_mm_s2);
    sendCommand(QString("AC%1").arg(counts));
}

void ServoActuatorDevice::setMaxTorque(double percent) {
    int counts = m_parser->torquePercentToSensorCounts(percent);
    sendCommand(QString("MT%1").arg(counts));
}

void ServoActuatorDevice::stopMove() {
    sendCommand("TK");
}

void ServoActuatorDevice::holdCurrentPosition() {
    sendCommand("PC");
}

//================================================================================
// PUBLIC API - STATUS & DIAGNOSTICS
//================================================================================

void ServoActuatorDevice::checkAllStatus() {
    // Queue up all polling commands
    m_commandQueue.enqueue("SR");  // Status register
    m_commandQueue.enqueue("AP");  // Position
    m_commandQueue.enqueue("VL");  // Velocity
    m_commandQueue.enqueue("TQ");  // Torque
    m_commandQueue.enqueue("RT1"); // Temperature
    m_commandQueue.enqueue("BV");  // Bus voltage
    
    // Start processing if no command is pending
    if (m_pendingCommand.isEmpty()) {
        processNextCommand();
    }
}

void ServoActuatorDevice::checkStatusRegister() {
    sendCommand("SR");
}

void ServoActuatorDevice::checkPosition() {
    sendCommand("AP");
}

void ServoActuatorDevice::checkVelocity() {
    sendCommand("VL");
}

void ServoActuatorDevice::checkTorque() {
    sendCommand("TQ");
}

void ServoActuatorDevice::checkTemperature() {
    sendCommand("RT1");
}

void ServoActuatorDevice::checkBusVoltage() {
    sendCommand("BV");
}

//================================================================================
// PUBLIC API - SYSTEM & CONFIGURATION
//================================================================================

void ServoActuatorDevice::saveSettings() {
    sendCommand("CW321");
}

void ServoActuatorDevice::clearFaults() {
    sendCommand("ZF");
}

void ServoActuatorDevice::reboot() {
    sendCommand("ZR321");
}
