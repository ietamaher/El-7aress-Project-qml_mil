#pragma once
#include "../interfaces/ProtocolParser.h"
#include <QByteArray>

//================================================================================
// 3DM-GX3-25 MICROSTRAIN AHRS PROTOCOL PARSER (Serial Binary)
//================================================================================

/**
 * @brief Command bytes for 3DM-GX3-25 AHRS
 */
namespace GX3Commands {
    constexpr quint8 EULER_ANGLES_AND_RATES = 0xCF;  // Primary command for gimbal control
    constexpr quint8 CAPTURE_GYRO_BIAS = 0xCD;       // Gyro bias calibration
    constexpr quint8 SET_CONTINUOUS_MODE = 0xC4;     // Enable continuous streaming
    constexpr quint8 STOP_CONTINUOUS_MODE = 0xFA;    // Stop streaming (no reply)
    constexpr quint8 SAMPLING_SETTINGS = 0xDB;       // Configure sampling rate
    constexpr quint8 TEMPERATURES = 0xD1;            // Read sensor temperatures (status)
    constexpr quint8 MODE_COMMAND = 0xD4;            // Set device mode
    constexpr quint8 DEVICE_RESET = 0xFE;            // Reset device (no reply)
    constexpr quint8 READ_FIRMWARE_VERSION = 0xE9;   // Get firmware version
    constexpr quint8 GYRO_STABILIZED_DATA = 0xD2;    // Alternative: gyro-stabilized outputs
}

/**
 * @brief Parser for 3DM-GX3-25 MicroStrain AHRS serial binary protocol
 *
 * This parser handles the single-byte command protocol used by the 3DM-GX3-25.
 * Primary command: 0xCF (Euler Angles and Angular Rates)
 *
 * Packet format for 0xCF:
 * - Echo (1 byte): 0xCF
 * - Roll (4 bytes): IEEE 754 float, degrees
 * - Pitch (4 bytes): IEEE 754 float, degrees
 * - Yaw (4 bytes): IEEE 754 float, degrees (magnetic heading)
 * - Roll Rate (4 bytes): IEEE 754 float, deg/s
 * - Pitch Rate (4 bytes): IEEE 754 float, deg/s
 * - Yaw Rate (4 bytes): IEEE 754 float, deg/s
 * - Timer (4 bytes): 32-bit unsigned, ticks (62.5 µs each)
 * - Checksum (2 bytes): Big-endian sum of all bytes
 * Total: 31 bytes
 */
class Imu3DMGX3ProtocolParser : public ProtocolParser {
    Q_OBJECT
public:
    explicit Imu3DMGX3ProtocolParser(QObject* parent = nullptr);
    ~Imu3DMGX3ProtocolParser() override = default;

    /**
     * @brief Parses incoming serial data stream into typed Message objects
     * @param rawData Byte stream from serial port
     * @return Vector of parsed messages (ImuDataMessage)
     */
    std::vector<MessagePtr> parse(const QByteArray& rawData) override;

    /**
     * @brief Not used for serial protocol (Modbus-specific)
     */
    std::vector<MessagePtr> parse(QModbusReply* /*reply*/) override { return {}; }

    /**
     * @brief Creates command to enter continuous mode with 0xCF data
     * @return 2-byte command: {0xC4, 0xCF}
     */
    static QByteArray createContinuousModeCommand();

    /**
     * @brief Creates command to stop continuous mode
     * @return 1-byte command: {0xFA}
     */
    static QByteArray createStopContinuousModeCommand();

    /**
     * @brief Creates command to capture gyro bias (device must be stationary)
     * @return 1-byte command: {0xCD}
     */
    static QByteArray createCaptureGyroBiasCommand();

    /**
     * @brief Creates command to set sampling rate
     * @param decimation Decimation value (0 = 1000Hz, 1 = 500Hz, 4 = 200Hz, 9 = 100Hz)
     * @param flags Sampling flags (0x0000 for default)
     * @param reserved Reserved bytes (0x0000)
     * @return 7-byte command with checksum
     */
    static QByteArray createSamplingSettingsCommand(quint16 decimation = 9,
                                                     quint16 flags = 0x0000,
                                                     quint16 reserved = 0x0000);

    /**
     * @brief Creates command to read sensor temperatures
     * @return 1-byte command: {0xD1}
     */
    static QByteArray createReadTemperaturesCommand();

    /**
     * @brief Calculates 16-bit checksum for packet validation
     * @param data Packet data (excluding checksum bytes)
     * @return 16-bit checksum (sum of all bytes)
     */
    static quint16 calculateChecksum(const QByteArray& data);

private:
    /**
     * @brief Parses a complete 0xCF packet (31 bytes)
     * @param packet Complete packet data
     * @return ImuDataMessage or nullptr if invalid
     */
    MessagePtr parse0xCFPacket(const QByteArray& packet);

    /**
     * @brief Parses a complete 0xD1 temperature packet (27 bytes)
     * @param packet Complete packet data
     * @return Temperature data (stored in m_lastTemperature)
     */
    void parse0xD1Packet(const QByteArray& packet);

    /**
     * @brief Extracts IEEE 754 float from byte array (big-endian)
     * @param data Byte array
     * @param offset Starting offset
     * @return Extracted float value
     */
    float extractFloat(const QByteArray& data, int offset) const;

    /**
     * @brief Extracts 32-bit unsigned integer (big-endian)
     * @param data Byte array
     * @param offset Starting offset
     * @return Extracted uint32 value
     */
    quint32 extractUInt32(const QByteArray& data, int offset) const;

    /**
     * @brief Extracts 16-bit unsigned integer (big-endian)
     * @param data Byte array
     * @param offset Starting offset
     * @return Extracted uint16 value
     */
    quint16 extractUInt16(const QByteArray& data, int offset) const;

    // Buffer for accumulating partial packets
    QByteArray m_buffer;

    // Temperature cache (updated periodically from 0xD1 queries)
    double m_lastTemperature = 0.0;  // Average of all sensor temps

    // Expected packet sizes for different commands
    static constexpr int PACKET_SIZE_0xCF = 31;  // Euler Angles + Rates
    static constexpr int PACKET_SIZE_0xCD = 79;  // Gyro Bias Capture response
    static constexpr int PACKET_SIZE_0xDB = 7;   // Sampling Settings response
    static constexpr int PACKET_SIZE_0xD1 = 27;  // Temperatures (5 floats + timer)

public:
    /**
     * @brief Gets the last read temperature (averaged across sensors)
     * @return Temperature in degrees Celsius
     */
    double lastTemperature() const { return m_lastTemperature; }
};
