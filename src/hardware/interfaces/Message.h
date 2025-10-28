#pragma once
#include <memory>

/**
 * @brief Base class for all message types in the system
 */
class Message {
public:
    enum class Type {
        Generic,
        // Servo Driver (Modbus)
        ServoDriverDataType,
        ServoDriverAlarmType,
        ServoDriverAlarmHistoryType,
        // Servo Actuator (Serial)
        ServoActuatorDataType,
        ServoActuatorAckType,
        ServoActuatorNackType,
        ServoActuatorCriticalFaultType,
        // PLC devices (Modbus)
        Plc21DataType,
        Plc42DataType,
        // Other devices (keep existing from MIL-STD)
        RadarPlotType,
        LrfDataType,
        LrfInfoType

    };
    
    virtual ~Message() = default;
    virtual Type typeId() const { return Type::Generic; }
};

using MessagePtr = std::unique_ptr<Message>;
