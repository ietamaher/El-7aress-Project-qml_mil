#include "Plc21ProtocolParser.h"
#include "../messages/Plc21Message.h"
#include <QModbusDataUnit>
#include <QDebug>

Plc21ProtocolParser::Plc21ProtocolParser(QObject* parent)
    : ProtocolParser(parent)
{
}

std::vector<MessagePtr> Plc21ProtocolParser::parse(QModbusReply* reply) {
    std::vector<MessagePtr> messages;

    if (!reply || reply->error() != QModbusDevice::NoError) {
        return messages;
    }

    const QModbusDataUnit unit = reply->result();

    // Determine which type of data we received based on register type and address
    if (unit.registerType() == QModbusDataUnit::DiscreteInputs &&
        unit.startAddress() == Plc21Registers::DIGITAL_INPUTS_START_ADDR) {
        messages.push_back(parseDigitalInputsReply(unit));
    }
    else if (unit.registerType() == QModbusDataUnit::HoldingRegisters &&
             unit.startAddress() == Plc21Registers::ANALOG_INPUTS_START_ADDR) {
        messages.push_back(parseAnalogInputsReply(unit));
    }

    return messages;
}

MessagePtr Plc21ProtocolParser::parseDigitalInputsReply(const QModbusDataUnit& unit) {
    Plc21PanelData data;
    data.isConnected = true;

    // Map digital inputs based on their indices
    if (unit.valueCount() > 0) {
        data.authorizeSw = (unit.value(0) != 0);
    }
    if (unit.valueCount() > 1) {
        data.menuValSw = (unit.value(1) != 0);
    }
    if (unit.valueCount() > 2) {
        data.menuDownSW = (unit.value(2) != 0);
    }
    if (unit.valueCount() > 3) {
        data.menuUpSW = (unit.value(3) != 0);
    }
    if (unit.valueCount() > 4) {
        data.switchCameraSW = (unit.value(4) != 0);
    }
    if (unit.valueCount() > 5) {
        data.enableStabilizationSW = (unit.value(5) != 0);
    }
    if (unit.valueCount() > 6) {
        data.homePositionSW = (unit.value(6) != 0);
    }
    if (unit.valueCount() > 8) {
        data.loadAmmunitionSW = (unit.value(8) != 0);
    }
    if (unit.valueCount() > 9) {
        data.armGunSW = (unit.value(9) != 0);
    }
    if (unit.valueCount() > 10) {
        data.enableStationSW = (unit.value(10) != 0);
    }

    return std::make_unique<Plc21DataMessage>(data);
}

MessagePtr Plc21ProtocolParser::parseAnalogInputsReply(const QModbusDataUnit& unit) {
    Plc21PanelData data;
    data.isConnected = true;

    // Map analog inputs based on their indices
    if (unit.valueCount() > 0) {
        data.fireMode = unit.value(0);
    }
    if (unit.valueCount() > 1) {
        data.speedSW = unit.value(1);
    }
    if (unit.valueCount() > 2) {
        data.panelTemperature = unit.value(2);
    }

    return std::make_unique<Plc21DataMessage>(data);
}
