#ifndef DATATYPES_H
#define DATATYPES_H

#include <QtCore>
#include <QStringList>

// ============================================================================
// CAMERA DATA STRUCTURES
// ============================================================================

/**
 * @brief Day camera data structure (Pelco-D protocol)
 */
/*struct DayCameraData {
    bool isConnected = false;
    bool errorState = false;
    quint8 cameraStatus = 0;
    
    // Zoom control
    bool zoomMovingIn = false;
    bool zoomMovingOut = false;
    quint16 zoomPosition = 0;   // 14-bit max for VISCA
    
    // Focus control
    bool autofocusEnabled = true;
    quint16 focusPosition = 0;  // 12-bit max
    
    // Field of view
    float currentHFOV = 11.0;

    bool operator!=(const DayCameraData &other) const {
        return (isConnected != other.isConnected ||
                errorState != other.errorState ||
                cameraStatus != other.cameraStatus ||
                zoomMovingIn != other.zoomMovingIn ||
                zoomMovingOut != other.zoomMovingOut ||
                zoomPosition != other.zoomPosition ||
                autofocusEnabled != other.autofocusEnabled ||
                focusPosition != other.focusPosition ||
                !qFuzzyCompare(currentHFOV, other.currentHFOV));
    }
};*/

/**
 * @brief Night camera data structure (TAU2 protocol)
 */
/*struct NightCameraData {
    bool isConnected = false;
    quint8 errorState = 0x00;
    bool ffcInProgress = false;
    bool digitalZoomEnabled = false;
    quint8 digitalZoomLevel = 0;
    double currentHFOV = 10.4;
    quint16 videoMode = 0;
    quint8 lut = 0;
    quint8 cameraStatus = 0;

    bool operator!=(const NightCameraData &other) const {
        return (isConnected != other.isConnected ||
                errorState != other.errorState ||
                ffcInProgress != other.ffcInProgress ||
                digitalZoomEnabled != other.digitalZoomEnabled ||
                digitalZoomLevel != other.digitalZoomLevel ||
                !qFuzzyCompare(currentHFOV, other.currentHFOV) ||
                videoMode != other.videoMode ||
                cameraStatus != other.cameraStatus);
    }
};*/

// ============================================================================
// SENSOR DATA STRUCTURES
// ============================================================================

/**
 * @brief Laser Range Finder data structure
 */
struct LrfData {
    bool isConnected = false;
    quint16 lastDistance = 0;
    bool isLastRangingValid = false;
    quint8 pulseCount = 0;
    quint8 rawStatusByte = 0;
    bool isFault = false;
    bool noEcho = false;
    bool laserNotOut = false;
    bool isOverTemperature = false;
    bool isTempValid = false;
    qint8 temperature = 0;
    quint32 laserCount = 0;

    bool operator!=(const LrfData &other) const {
        return (isConnected != other.isConnected ||
                lastDistance != other.lastDistance ||
                isLastRangingValid != other.isLastRangingValid ||
                pulseCount != other.pulseCount ||
                rawStatusByte != other.rawStatusByte ||
                isFault != other.isFault ||
                noEcho != other.noEcho ||
                laserNotOut != other.laserNotOut ||
                isOverTemperature != other.isOverTemperature ||
                isTempValid != other.isTempValid ||
                temperature != other.temperature ||
                laserCount != other.laserCount);
    }
};

/**
 * @brief IMU/Inclinometer data structure
 */
/*struct ImuData {
    bool isConnected = false;
    
    // Processed angles (from Kalman filter)
    double rollDeg = 0.0;
    double pitchDeg = 0.0;
    double yawDeg = 0.0;
    
    // Physical state
    double temperature = 0.0;
    
    // Raw IMU data
    double accelX_g = 0.0;
    double accelY_g = 0.0;
    double accelZ_g = 0.0;
    double angRateX_dps = 0.0;
    double angRateY_dps = 0.0;
    double angRateZ_dps = 0.0;

    bool operator!=(const ImuData &other) const {
        return (isConnected != other.isConnected ||
                rollDeg != other.rollDeg ||
                pitchDeg != other.pitchDeg ||
                yawDeg != other.yawDeg ||
                temperature != other.temperature ||
                accelX_g != other.accelX_g ||
                accelY_g != other.accelY_g ||
                accelZ_g != other.accelZ_g ||
                angRateX_dps != other.angRateX_dps ||
                angRateY_dps != other.angRateY_dps ||
                angRateZ_dps != other.angRateZ_dps);
    }
};*/

// ============================================================================
// SERVO/ACTUATOR DATA STRUCTURES
// ============================================================================

/**
 * @brief Servo driver data structure (Modbus)
 */
struct ServoDriverData {
    bool isConnected = false;
    float position = 0.0f;
    float rpm = 0.0f;
    float torque = 0.0f;
    float motorTemp = 0.0f;
    float driverTemp = 0.0f;
    bool fault = false;
    

    bool operator!=(const ServoDriverData &other) const {
        return (isConnected != other.isConnected ||
                position != other.position ||
                rpm != other.rpm ||
                torque != other.torque ||
                motorTemp != other.motorTemp ||
                driverTemp != other.driverTemp ||
                fault != other.fault);
    }
};

/**
 * @brief Servo actuator status structure
 */
struct ActuatorStatus {
    bool isMotorOff = false;
    bool isLatchingFaultActive = false;
    QStringList activeStatusMessages;
    //quint32 ActuatorStatus = 0;

    bool operator!=(const ActuatorStatus &other) const {
        return (isMotorOff != other.isMotorOff ||
                isLatchingFaultActive != other.isLatchingFaultActive ||
                activeStatusMessages != other.activeStatusMessages);
    }
};

/**
 * @brief Servo actuator data structure
 */
struct ServoActuatorData {
    bool isConnected = false;
    double position_mm = 0.0;
    double velocity_mm_s = 0.0;
    double temperature_c = 0.0;
    double busVoltage_v = 0.0;
    double torque_percent = 0.0;
    ActuatorStatus status;

    bool operator!=(const ServoActuatorData &other) const {
        return (isConnected != other.isConnected ||
                !qFuzzyCompare(position_mm, other.position_mm) ||
                !qFuzzyCompare(velocity_mm_s, other.velocity_mm_s) ||
                !qFuzzyCompare(temperature_c, other.temperature_c) ||
                !qFuzzyCompare(busVoltage_v, other.busVoltage_v) ||
                !qFuzzyCompare(torque_percent, other.torque_percent) ||
                status != other.status);
    }
};

// ============================================================================
// PLC DATA STRUCTURES
// ============================================================================

/**
 * @brief PLC21 panel data structure
 */
struct Plc21PanelData {
    bool isConnected = false;

    // Digital inputs
    bool armGunSW = false;
    bool loadAmmunitionSW = false;
    bool enableStationSW = false;
    bool homePositionSW = false;
    bool enableStabilizationSW = false;
    bool authorizeSw = false;
    bool switchCameraSW = false;
    bool menuUpSW = false;
    bool menuDownSW = false;
    bool menuValSw = false;

    // Analog inputs
    int speedSW = 2;
    int fireMode = 0;
    int panelTemperature = 0;

    bool operator!=(const Plc21PanelData &other) const {
        return (isConnected != other.isConnected ||
                armGunSW != other.armGunSW ||
                loadAmmunitionSW != other.loadAmmunitionSW ||
                enableStationSW != other.enableStationSW ||
                homePositionSW != other.homePositionSW ||
                enableStabilizationSW != other.enableStabilizationSW ||
                authorizeSw != other.authorizeSw ||
                switchCameraSW != other.switchCameraSW ||
                menuUpSW != other.menuUpSW ||
                menuDownSW != other.menuDownSW ||
                menuValSw != other.menuValSw ||
                speedSW != other.speedSW ||
                fireMode != other.fireMode ||
                panelTemperature != other.panelTemperature);
    }
};

/**
 * @brief PLC42 data structure
 */
struct Plc42Data {
    bool isConnected = false;

    // Discrete inputs
    bool stationUpperSensor = false;
    bool stationLowerSensor = false;
    bool emergencyStopActive = false;
    bool ammunitionLevel = false;
    bool stationInput1 = false;
    bool stationInput2 = false;
    bool stationInput3 = false;
    bool solenoidActive = false;

    // Holding registers
    uint16_t solenoidMode = 0;
    uint16_t gimbalOpMode = 0;
    uint32_t azimuthSpeed = 0;
    uint32_t elevationSpeed = 0;
    uint16_t azimuthDirection = 0;
    uint16_t elevationDirection = 0;
    uint16_t solenoidState = 0;
    uint16_t resetAlarm = 0;

    bool operator!=(const Plc42Data &other) const {
        return (isConnected != other.isConnected ||
                stationUpperSensor != other.stationUpperSensor ||
                stationLowerSensor != other.stationLowerSensor ||
                emergencyStopActive != other.emergencyStopActive ||
                ammunitionLevel != other.ammunitionLevel ||
                stationInput1 != other.stationInput1 ||
                stationInput2 != other.stationInput2 ||
                stationInput3 != other.stationInput3 ||
                solenoidActive != other.solenoidActive ||
                solenoidMode != other.solenoidMode ||
                gimbalOpMode != other.gimbalOpMode ||
                azimuthSpeed != other.azimuthSpeed ||
                elevationSpeed != other.elevationSpeed ||
                azimuthDirection != other.azimuthDirection ||
                elevationDirection != other.elevationDirection ||
                solenoidState != other.solenoidState ||
                resetAlarm != other.resetAlarm);
    }
};

#endif // DATATYPES_H
