#pragma once
#include "../interfaces/ProtocolParser.h"
#include <QModbusReply>
#include <QMap>

//================================================================================
// SERVO DRIVER PROTOCOL PARSER (Modbus RTU)
//================================================================================

/**
 * @brief Register addresses for servo driver Modbus communication
 */
namespace ServoDriverRegisters {
    constexpr int POSITION_START_ADDR = 204;
    constexpr int POSITION_REG_COUNT = 2;
    constexpr int TEMPERATURE_START_ADDR = 248;
    constexpr int TEMPERATURE_REG_COUNT = 4;
    constexpr int ALARM_STATUS_ADDR = 172;
    constexpr int ALARM_STATUS_REG_COUNT = 20;
    constexpr int ALARM_HISTORY_ADDR = 130;
    constexpr int ALARM_HISTORY_REG_COUNT = 20;
    constexpr int ALARM_RESET_ADDR = 388;
    constexpr int ALARM_HISTORY_CLEAR_ADDR = 386;
}

/**
 * @brief Parser for Modbus RTU servo driver protocol
 * 
 * Converts QModbusReply objects into typed Message objects.
 * Handles position, temperature, and alarm data parsing.
 */
class ServoDriverProtocolParser : public ProtocolParser {
    Q_OBJECT
public:
    explicit ServoDriverProtocolParser(QObject* parent = nullptr);
    ~ServoDriverProtocolParser() override = default;

    // This parser does not use raw byte streaming
    std::vector<MessagePtr> parse(const QByteArray& /*rawData*/) override { return {}; }

    // Primary parsing method for Modbus replies
    std::vector<MessagePtr> parse(QModbusReply* reply) override;

private:
    // Helper methods to create specific messages from a reply
    MessagePtr parsePositionReply(const QModbusDataUnit& unit);
    MessagePtr parseTemperatureReply(const QModbusDataUnit& unit);
    MessagePtr parseAlarmReply(const QModbusDataUnit& unit);
    MessagePtr parseAlarmHistoryReply(const QModbusDataUnit& unit);

    // Alarm description lookup
    void initializeAlarmMap();
    QString getAlarmDescription(uint16_t alarmCode);
    
    QMap<uint16_t, QString> m_alarmMap;
};
