#include "NightCameraProtocolParser.h"
#include "../messages/NightCameraMessage.h"
#include <QDebug>

NightCameraProtocolParser::NightCameraProtocolParser(QObject* parent)
    : ProtocolParser(parent) {}

std::vector<MessagePtr> NightCameraProtocolParser::parse(const QByteArray& rawData) {
    std::vector<MessagePtr> messages;
    m_buffer.append(rawData);

    while (m_buffer.size() >= 10) {
        if (static_cast<quint8>(m_buffer.at(0)) != 0x6E) {
            m_buffer.remove(0, 1);
            continue;
        }

        if (m_buffer.size() < 6) break;

        quint16 byteCount = (static_cast<quint8>(m_buffer.at(4)) << 8) |
                            static_cast<quint8>(m_buffer.at(5));
        int totalSize = 6 + byteCount + 2 + 2;

        if (m_buffer.size() < totalSize) break;

        QByteArray packet = m_buffer.left(totalSize);
        m_buffer.remove(0, totalSize);

        if (verifyCRC(packet)) {
            auto msg = parsePacket(packet);
            if (msg) messages.push_back(std::move(msg));
        }
    }
    return messages;
}

quint16 NightCameraProtocolParser::calculateCRC(const QByteArray& data, int length) {
    quint16 crc = 0x0000;
    for (int i = 0; i < length; ++i) {
        crc ^= static_cast<quint8>(data[i]) << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool NightCameraProtocolParser::verifyCRC(const QByteArray& packet) {
    if (packet.size() < 10) return false;

    quint16 receivedCRC1 = (static_cast<quint8>(packet[6]) << 8) |
                           static_cast<quint8>(packet[7]);
    quint16 receivedCRC2 = (static_cast<quint8>(packet[packet.size() - 2]) << 8) |
                           static_cast<quint8>(packet[packet.size() - 1]);

    quint16 calculatedCRC1 = calculateCRC(packet, 6);
    quint16 calculatedCRC2 = calculateCRC(packet, packet.size() - 2);

    return (calculatedCRC1 == receivedCRC1) && (calculatedCRC2 == receivedCRC2);
}

MessagePtr NightCameraProtocolParser::parsePacket(const QByteArray& packet) {
    NightCameraData data;
    data.isConnected = true;

    quint8 statusByte = static_cast<quint8>(packet.at(1));
    data.errorState = statusByte;

    quint8 functionCode = static_cast<quint8>(packet.at(3));
    quint16 byteCount = (static_cast<quint8>(packet.at(4)) << 8) |
                        static_cast<quint8>(packet.at(5));
    QByteArray payloadData = packet.mid(8, byteCount);

    // Parse based on function code
    if (functionCode == 0x06 && !payloadData.isEmpty()) {
        data.cameraStatus = static_cast<quint8>(payloadData[0]);
    } else if (functionCode == 0x0B) {
        data.ffcInProgress = false;
    }

    return std::make_unique<NightCameraDataMessage>(data);
}

QByteArray NightCameraProtocolParser::buildCommand(quint8 function, const QByteArray& data) {
    QByteArray packet;
    packet.append(static_cast<char>(0x6E));
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));
    packet.append(function);

    quint16 byteCount = static_cast<quint16>(data.size());
    packet.append(static_cast<quint8>((byteCount >> 8) & 0xFF));
    packet.append(static_cast<quint8>(byteCount & 0xFF));

    quint16 crc1 = calculateCRC(packet, 6);
    packet.append(static_cast<quint8>((crc1 >> 8) & 0xFF));
    packet.append(static_cast<quint8>(crc1 & 0xFF));

    packet.append(data);

    quint16 crc2 = calculateCRC(packet, packet.size());
    packet.append(static_cast<quint8>((crc2 >> 8) & 0xFF));
    packet.append(static_cast<quint8>(crc2 & 0xFF));

    return packet;
}
