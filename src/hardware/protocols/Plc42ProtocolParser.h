#pragma once
#include "../interfaces/ProtocolParser.h"
#include <QModbusReply>

//================================================================================
// PLC42 PROTOCOL PARSER (Modbus RTU)
//================================================================================

/**
 * @brief Register addresses for PLC42 Modbus communication
 */
namespace Plc42Registers {
    constexpr int DIGITAL_INPUTS_START_ADDR = 0;
    constexpr int DIGITAL_INPUTS_COUNT = 13;
    constexpr int HOLDING_REGISTERS_START_ADDR = 0;
    constexpr int HOLDING_REGISTERS_COUNT = 10;
}

/**
 * @brief Parser for Modbus RTU PLC42 protocol
 *
 * Converts QModbusReply objects into typed Message objects.
 * Handles digital inputs (discrete inputs) and holding registers.
 */
class Plc42ProtocolParser : public ProtocolParser {
    Q_OBJECT
public:
    explicit Plc42ProtocolParser(QObject* parent = nullptr);
    ~Plc42ProtocolParser() override = default;

    // This parser does not use raw byte streaming
    std::vector<MessagePtr> parse(const QByteArray& /*rawData*/) override { return {}; }

    // Primary parsing method for Modbus replies
    std::vector<MessagePtr> parse(QModbusReply* reply) override;

private:
    // Helper methods to create specific messages from a reply
    MessagePtr parseDigitalInputsReply(const QModbusDataUnit& unit);
    MessagePtr parseHoldingRegistersReply(const QModbusDataUnit& unit);
};
