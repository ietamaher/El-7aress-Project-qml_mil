#include "Imu3DMGX3ProtocolParser.h"
#include "../messages/ImuMessage.h"
#include <QDebug>
#include <cstring>
#include <QtEndian>

Imu3DMGX3ProtocolParser::Imu3DMGX3ProtocolParser(QObject* parent)
    : ProtocolParser(parent)
{
}

std::vector<MessagePtr> Imu3DMGX3ProtocolParser::parse(const QByteArray& rawData) {
    std::vector<MessagePtr> messages;

    // Append new data to buffer
    m_buffer.append(rawData);

    // Process all complete packets in buffer
    while (!m_buffer.isEmpty()) {
        // Need at least 1 byte to identify command
        if (m_buffer.size() < 1) {
            break;
        }

        quint8 command = static_cast<quint8>(m_buffer.at(0));

        // Determine expected packet size based on command byte
        int expectedSize = 0;
        switch (command) {
            case GX3Commands::EULER_ANGLES_AND_RATES:
                expectedSize = PACKET_SIZE_0xCF;
                break;
            case GX3Commands::CAPTURE_GYRO_BIAS:
                expectedSize = PACKET_SIZE_0xCD;
                break;
            case GX3Commands::SAMPLING_SETTINGS:
                expectedSize = PACKET_SIZE_0xDB;
                break;
            default:
                // Unknown command - try to find next valid command byte
                qWarning() << "Imu3DMGX3Parser: Unknown command byte" << Qt::hex << command;
                m_buffer.remove(0, 1); // Discard invalid byte
                continue;
        }

        // Wait for complete packet
        if (m_buffer.size() < expectedSize) {
            break; // Need more data
        }

        // Extract packet
        QByteArray packet = m_buffer.left(expectedSize);
        m_buffer.remove(0, expectedSize);

        // Validate checksum (last 2 bytes)
        QByteArray dataWithoutChecksum = packet.left(packet.size() - 2);
        quint16 receivedChecksum = extractUInt16(packet, packet.size() - 2);
        quint16 calculatedChecksum = calculateChecksum(dataWithoutChecksum);

        if (receivedChecksum != calculatedChecksum) {
            qWarning() << "Imu3DMGX3Parser: Checksum mismatch! Expected"
                       << Qt::hex << calculatedChecksum << "got" << receivedChecksum;
            continue; // Discard corrupted packet
        }

        // Parse packet based on command
        MessagePtr msg = nullptr;
        switch (command) {
            case GX3Commands::EULER_ANGLES_AND_RATES:
                msg = parse0xCFPacket(packet);
                break;
            case GX3Commands::CAPTURE_GYRO_BIAS:
                qDebug() << "Imu3DMGX3Parser: Gyro bias capture completed successfully";
                // No data message generated, just log
                break;
            case GX3Commands::SAMPLING_SETTINGS:
                qDebug() << "Imu3DMGX3Parser: Sampling settings configured successfully";
                break;
        }

        if (msg) {
            messages.push_back(std::move(msg));
        }
    }

    return messages;
}

MessagePtr Imu3DMGX3ProtocolParser::parse0xCFPacket(const QByteArray& packet) {
    if (packet.size() != PACKET_SIZE_0xCF) {
        qWarning() << "Imu3DMGX3Parser: Invalid 0xCF packet size:" << packet.size();
        return nullptr;
    }

    // Verify echo byte
    if (static_cast<quint8>(packet.at(0)) != GX3Commands::EULER_ANGLES_AND_RATES) {
        qWarning() << "Imu3DMGX3Parser: Invalid echo byte in 0xCF packet";
        return nullptr;
    }

    // Parse data fields (all big-endian IEEE 754 floats)
    ImuData data;
    data.isConnected = true;

    // Offset 1: Roll (degrees)
    data.rollDeg = extractFloat(packet, 1);

    // Offset 5: Pitch (degrees)
    data.pitchDeg = extractFloat(packet, 5);

    // Offset 9: Yaw (degrees) - FINALLY A REAL HEADING!
    data.yawDeg = extractFloat(packet, 9);

    // Offset 13: Roll Rate (deg/s)
    data.angRateX_dps = extractFloat(packet, 13);

    // Offset 17: Pitch Rate (deg/s)
    data.angRateY_dps = extractFloat(packet, 17);

    // Offset 21: Yaw Rate (deg/s)
    data.angRateZ_dps = extractFloat(packet, 21);

    // Offset 25: Timer (32-bit unsigned, ticks at 62.5µs)
    // quint32 timerTicks = extractUInt32(packet, 25);
    // double timeSeconds = timerTicks * 62.5e-6; // Optional: convert to seconds

    // Acceleration data not provided by 0xCF command
    // If needed, use 0xCC or 0xD2 commands instead
    data.accelX_g = 0.0;
    data.accelY_g = 0.0;
    data.accelZ_g = 0.0;

    // Temperature not provided by 0xCF
    data.temperature = 0.0;

    // Validate data ranges (sanity checks)
    if (std::isnan(data.rollDeg) || std::isnan(data.pitchDeg) || std::isnan(data.yawDeg) ||
        std::isnan(data.angRateX_dps) || std::isnan(data.angRateY_dps) || std::isnan(data.angRateZ_dps)) {
        qWarning() << "Imu3DMGX3Parser: NaN detected in 0xCF data!";
        return nullptr;
    }

    return std::make_unique<ImuDataMessage>(data);
}

float Imu3DMGX3ProtocolParser::extractFloat(const QByteArray& data, int offset) const {
    if (offset + 4 > data.size()) {
        return 0.0f;
    }

    // Extract 4 bytes as big-endian
    quint32 rawBits = qFromBigEndian<quint32>(
        reinterpret_cast<const uchar*>(data.constData() + offset)
    );

    // Reinterpret bits as IEEE 754 float
    float value;
    std::memcpy(&value, &rawBits, sizeof(float));

    return value;
}

quint32 Imu3DMGX3ProtocolParser::extractUInt32(const QByteArray& data, int offset) const {
    if (offset + 4 > data.size()) {
        return 0;
    }

    return qFromBigEndian<quint32>(
        reinterpret_cast<const uchar*>(data.constData() + offset)
    );
}

quint16 Imu3DMGX3ProtocolParser::extractUInt16(const QByteArray& data, int offset) const {
    if (offset + 2 > data.size()) {
        return 0;
    }

    return qFromBigEndian<quint16>(
        reinterpret_cast<const uchar*>(data.constData() + offset)
    );
}

quint16 Imu3DMGX3ProtocolParser::calculateChecksum(const QByteArray& data) {
    quint16 checksum = 0;
    for (char byte : data) {
        checksum += static_cast<quint8>(byte);
    }
    return checksum;
}

QByteArray Imu3DMGX3ProtocolParser::createContinuousModeCommand() {
    // Command format: 0xC4 <command_byte>
    // This sets continuous mode to output the specified command continuously
    QByteArray cmd;
    cmd.append(static_cast<char>(GX3Commands::SET_CONTINUOUS_MODE));
    cmd.append(static_cast<char>(GX3Commands::EULER_ANGLES_AND_RATES));
    return cmd;
}

QByteArray Imu3DMGX3ProtocolParser::createStopContinuousModeCommand() {
    QByteArray cmd;
    cmd.append(static_cast<char>(GX3Commands::STOP_CONTINUOUS_MODE));
    return cmd;
}

QByteArray Imu3DMGX3ProtocolParser::createCaptureGyroBiasCommand() {
    // 0xCD - Device must be stationary for ~10 seconds
    QByteArray cmd;
    cmd.append(static_cast<char>(GX3Commands::CAPTURE_GYRO_BIAS));
    return cmd;
}

QByteArray Imu3DMGX3ProtocolParser::createSamplingSettingsCommand(quint16 decimation,
                                                                   quint16 flags,
                                                                   quint16 reserved) {
    // Command format for 0xDB (7 bytes total):
    // Byte 0: 0xDB (command)
    // Bytes 1-2: Decimation (big-endian)
    // Bytes 3-4: Flags (big-endian)
    // Bytes 5-6: Reserved (big-endian)

    QByteArray cmd;
    cmd.append(static_cast<char>(GX3Commands::SAMPLING_SETTINGS));

    // Decimation (big-endian)
    cmd.append(static_cast<char>((decimation >> 8) & 0xFF));
    cmd.append(static_cast<char>(decimation & 0xFF));

    // Flags (big-endian)
    cmd.append(static_cast<char>((flags >> 8) & 0xFF));
    cmd.append(static_cast<char>(flags & 0xFF));

    // Reserved (big-endian)
    cmd.append(static_cast<char>((reserved >> 8) & 0xFF));
    cmd.append(static_cast<char>(reserved & 0xFF));

    // Note: The device will send back a 7-byte response with checksum
    // This command doesn't need a checksum when sending

    return cmd;
}
