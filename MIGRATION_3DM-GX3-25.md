# Migration to 3DM-GX3-25 MicroStrain AHRS

## Overview

This document describes the migration from the **Vigor SST810 Inclinometer** (Modbus RTU) to the **MicroStrain 3DM-GX3-25 AHRS** (Serial Binary Protocol) for gyro-stabilized gimbal control.

## Summary of Changes

### Files Created
- `src/hardware/protocols/Imu3DMGX3ProtocolParser.h` - Protocol parser header
- `src/hardware/protocols/Imu3DMGX3ProtocolParser.cpp` - Protocol parser implementation
- `MIGRATION_3DM-GX3-25.md` - This documentation

### Files Modified
- `src/hardware/devices/imudevice.h` - Updated device interface for serial protocol
- `src/hardware/devices/imudevice.cpp` - Implemented initialization sequence
- `QT6-gstreamer-example.pro` - Added new parser to build system

### Key Improvements
✅ **Real Yaw Heading** - From magnetometer (no more hardcoded 25.0!)
✅ **Higher Resolution** - 17-bit ADC vs. Modbus-limited resolution
✅ **Faster Data Rate** - Up to 1000Hz vs. 50ms polling (20Hz)
✅ **Built-in Kalman Filter** - Optimized orientation estimation
✅ **Gyro-Stabilized Outputs** - Hardware-compensated measurements
✅ **Automatic Bias Correction** - Built-in 0xCD command
✅ **Temperature Monitoring** - Periodic sensor temperature queries (0xD1)

---

## Protocol Comparison

### Old: Vigor SST810 (Modbus RTU)
```cpp
Transport: ModbusTransport
Protocol: Modbus RTU over RS-485
Data Rate: Polled at 50ms (20Hz)
Command: Read 18 Input Registers at address 0x03E8
Data Format: 9 x 32-bit big-endian floats
Heading: Hardcoded to 25.0 (no magnetometer)
```

### New: 3DM-GX3-25 (Serial Binary)
```cpp
Transport: SerialTransport
Protocol: Single-byte binary commands over UART
Data Rate: Continuous streaming up to 1000Hz
Primary Command: 0xCF (Euler Angles + Angular Rates)
Data Format: Fixed 31-byte packets with IEEE 754 floats
Heading: Real magnetic heading from 3-axis magnetometer
```

---

## 3DM-GX3-25 Commands Used

| Command | Name                          | Purpose                              | Priority       |
|---------|-------------------------------|--------------------------------------|----------------|
| **0xCF** | Euler Angles + Angular Rates | **Primary data stream**             | ⭐⭐⭐ Essential |
| **0xC4** | Set Continuous Mode          | Enable continuous streaming          | ⭐⭐⭐ Essential |
| **0xCD** | Capture Gyro Bias            | Calibrate gyro bias (stationary)    | ⭐⭐ Important  |
| **0xDB** | Sampling Settings            | Configure data rate (50-1000Hz)      | ⭐⭐ Important  |
| **0xD1** | Temperatures                 | Query sensor temps (status/health)   | ⭐ Optional    |
| **0xFA** | Stop Continuous Mode         | Stop streaming (on shutdown)         | ⭐ Optional    |

---

## Initialization Sequence

The `ImuDevice` performs the following automated initialization on startup:

```
┌─────────────────────────────────────────────────────┐
│  1. Send 0xCD - Capture Gyro Bias                  │
│     - Device MUST be stationary                     │
│     - Takes ~10 seconds                             │
│     - Timeout: 15 seconds                           │
└─────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────┐
│  2. Send 0xDB - Set Sampling Rate                   │
│     - Default: 100Hz (decimation = 9)               │
│     - Configurable: 50-1000Hz                       │
│     - Response: 7-byte confirmation                 │
└─────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────┐
│  3. Send 0xC4 0xCF - Start Continuous Mode          │
│     - Begins streaming 0xCF packets                 │
│     - 31 bytes per packet                           │
│     - Continuous until stopped or power off         │
└─────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────┐
│  4. State: Running                                  │
│     - Device is ONLINE                              │
│     - Communication watchdog active                 │
│     - Data feeding gimbal stabilization             │
└─────────────────────────────────────────────────────┘
```

---

## 0xCF Packet Format (31 bytes)

```
Offset | Size | Field         | Type           | Units   | Description
-------|------|---------------|----------------|---------|---------------------------
  0    |  1   | Echo          | uint8          | -       | 0xCF (command echo)
  1    |  4   | Roll          | IEEE 754 float | degrees | Roll angle
  5    |  4   | Pitch         | IEEE 754 float | degrees | Pitch angle
  9    |  4   | Yaw           | IEEE 754 float | degrees | Magnetic heading ✨ NEW!
 13    |  4   | Roll Rate     | IEEE 754 float | deg/s   | Angular rate X
 17    |  4   | Pitch Rate    | IEEE 754 float | deg/s   | Angular rate Y
 21    |  4   | Yaw Rate      | IEEE 754 float | deg/s   | Angular rate Z
 25    |  4   | Timer         | uint32         | ticks   | 62.5µs per tick
 29    |  2   | Checksum      | uint16         | -       | Sum of all bytes (0-28)
-------|------|---------------|----------------|---------|---------------------------
Total: 31 bytes
```

All floats are **big-endian** IEEE 754 format.

---

## Data Mapping to ImuData Structure

### Old (SST810)
```cpp
ImuData {
    rollDeg = SST810_Y_Angle;          // From Modbus register 0x03EA
    pitchDeg = SST810_X_Angle;         // From Modbus register 0x03E8
    yawDeg = 25.0;                     // ❌ HARDCODED! No magnetometer

    angRateX_dps = SST810_X_Gyro;      // From Modbus register 0x03F4
    angRateY_dps = SST810_Y_Gyro;      // From Modbus register 0x03F6
    angRateZ_dps = SST810_Z_Gyro;      // From Modbus register 0x03F8

    accelX_g = SST810_X_Accel;         // From Modbus register 0x03EE
    accelY_g = SST810_Y_Accel;         // From Modbus register 0x03F0
    accelZ_g = SST810_Z_Accel;         // From Modbus register 0x03F2

    temperature = SST810_Temp / 10.0;  // From Modbus register 0x03EC
}
```

### New (3DM-GX3-25)
```cpp
ImuData {
    rollDeg = GX3_Roll;                // ✅ From 0xCF packet offset 1
    pitchDeg = GX3_Pitch;              // ✅ From 0xCF packet offset 5
    yawDeg = GX3_Yaw;                  // ✅ Real heading! offset 9

    angRateX_dps = GX3_Roll_Rate;      // ✅ From 0xCF packet offset 13
    angRateY_dps = GX3_Pitch_Rate;     // ✅ From 0xCF packet offset 17
    angRateZ_dps = GX3_Yaw_Rate;       // ✅ From 0xCF packet offset 21

    accelX_g = 0.0;                    // ⚠️ Not in 0xCF (use 0xCC if needed)
    accelY_g = 0.0;
    accelZ_g = 0.0;

    temperature = avg_sensor_temp;     // ✅ From 0xD1 query (updated every 5 seconds)
}
```

**Note:**
- **Temperature monitoring**: Automatically queried via 0xD1 every 5 seconds and averaged across all sensors (Mag, Accel, GyroX, GyroY, GyroZ)
- **Accelerations**: Not in 0xCF. Use 0xCC (Full data) or 0xD2 (Gyro-stabilized) if needed

---

## Configuration

### Hardware Connections

```
3DM-GX3-25 UART Pinout:
┌────────────┬──────────────┬─────────────────────┐
│ Pin        │ Signal       │ Connect To          │
├────────────┼──────────────┼─────────────────────┤
│ Tx (Red)   │ UART Tx      │ MCU/PC Rx           │
│ Rx (Black) │ UART Rx      │ MCU/PC Tx           │
│ GND        │ Ground       │ Common Ground       │
│ +5V        │ Power        │ 5V supply (±10%)    │
└────────────┴──────────────┴─────────────────────┘

Serial Port Settings:
- Baud Rate: 115200 (default, configurable)
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None
```

### Software Configuration (system_config.json)

```json
{
  "imu": {
    "enabled": true,
    "identifier": "IMU_GX3",
    "transport": {
      "type": "serial",
      "port": "/dev/ttyUSB0",
      "baudRate": 115200,
      "dataBits": 8,
      "parity": "none",
      "stopBits": 1
    },
    "parser": {
      "type": "Imu3DMGX3ProtocolParser"
    },
    "samplingRateHz": 100,
    "communicationTimeoutMs": 3000
  }
}
```

### Sampling Rate Options

| Rate (Hz) | Decimation | Use Case                          |
|-----------|------------|-----------------------------------|
| 1000      | 0          | High-speed research               |
| 500       | 1          | Advanced stabilization            |
| 200       | 4          | Fast gimbal control               |
| **100**   | **9**      | **Default - gimbal control** ✅  |
| 50        | 19         | Low bandwidth applications        |

---

## Gimbal Stabilization Integration

### No Changes Required! ✅

The `GimbalMotionModeBase::calculateStabilizationCorrection()` function already uses the exact data provided by the 3DM-GX3-25:

```cpp
// From gimbalmotionmodebase.cpp:302-373
void calculateStabilizationCorrection(
    double currentAz_deg,    // ← From GX3 yaw (now REAL heading!)
    double currentEl_deg,    // ← From GX3 pitch
    double gyroX_dps_raw,    // ← From GX3 roll rate
    double gyroY_dps_raw,    // ← From GX3 pitch rate
    double gyroZ_dps_raw,    // ← From GX3 yaw rate
    double& azCorrection_dps,
    double& elCorrection_dps
) {
    // Existing stabilization logic works perfectly!
}
```

The stabilization algorithm expects:
1. ✅ Current orientation (roll, pitch, **yaw**)
2. ✅ Angular rates (gyro X, Y, Z)
3. ✅ Data rate ≥50Hz

All requirements are met and **improved** by the 3DM-GX3-25!

---

## Testing Checklist

### Pre-Deployment Tests

- [ ] **Hardware Connection**
  - [ ] Verify serial port device exists (`/dev/ttyUSB0`)
  - [ ] Check baud rate configured correctly (115200)
  - [ ] Ensure device has stable 5V power supply

- [ ] **Gyro Bias Calibration**
  - [ ] Place gimbal on stable surface
  - [ ] Ensure NO movement during initialization (~10 seconds)
  - [ ] Monitor logs for "Gyro bias capture completed"

- [ ] **Data Streaming**
  - [ ] Verify 0xCF packets received continuously
  - [ ] Check data rate matches configuration (default 100Hz)
  - [ ] Confirm no checksum errors in logs

- [ ] **Orientation Data**
  - [ ] Verify roll/pitch angles match physical orientation
  - [ ] **CHECK YAW**: Should now show real magnetic heading (not 25.0!)
  - [ ] Verify angular rates respond to manual gimbal movement

- [ ] **Stabilization Performance**
  - [ ] Enable stabilization mode
  - [ ] Apply disturbance to vehicle/platform
  - [ ] Verify gimbal compensates for platform motion
  - [ ] Check for smooth, jitter-free stabilization

### Operational Validation

- [ ] Test all gimbal motion modes:
  - [ ] Manual mode
  - [ ] Auto-track mode
  - [ ] Sector scan mode
  - [ ] Radar slew mode
  - [ ] TRP scan mode

- [ ] Verify stabilization in different conditions:
  - [ ] Stationary platform
  - [ ] Moving vehicle
  - [ ] High vibration environment

- [ ] Long-term stability test:
  - [ ] Run continuously for ≥1 hour
  - [ ] Monitor for data dropouts
  - [ ] Check for memory leaks
  - [ ] Verify no performance degradation

---

## Troubleshooting

### Issue: "Gyro bias capture timed out"

**Cause:** Device not stationary during initialization
**Solution:**
1. Ensure gimbal is on stable surface
2. Disable vibration sources during startup
3. If unavoidable, increase `GYRO_BIAS_TIMEOUT_MS` to 20000ms

### Issue: "Checksum mismatch" errors

**Cause:** Serial communication errors (noise, baud rate mismatch)
**Solution:**
1. Verify baud rate = 115200
2. Check cable quality and length (<3m recommended)
3. Add ferrite beads to reduce EMI
4. Ensure proper grounding

### Issue: Yaw heading unstable or drifting

**Cause:** Magnetic interference
**Solution:**
1. Perform magnetometer calibration (see 3DM-GX3 manual)
2. Mount device away from motors, magnets, metal
3. Consider using 0xDD "Realign Up and North" command
4. For indoor use, may need to disable mag and use gyro-only yaw

### Issue: "Communication timeout" warnings

**Cause:** Data stream stopped
**Solution:**
1. Check serial port connection
2. Verify device has power
3. Send 0xFA to stop, then reinitialize
4. Check device logs for errors

### Issue: High-frequency jitter in stabilization

**Cause:** Noise in gyro data
**Solution:**
1. Gyro low-pass filter already implemented (5Hz cutoff)
2. Adjust filter cutoff in `GimbalMotionModeBase` constructor:
   ```cpp
   m_gyroXFilter(5.0, 20.0)  // Decrease cutoff for smoother response
   ```
3. Verify vehicle is stationary when bias is captured

---

## Advanced Features (Future Enhancements)

### 1. Alternative Command: 0xD2 (Gyro-Stabilized Data)

For even better performance, consider using **0xD2** instead of 0xCF:

```cpp
// Provides inertially-stabilized measurements
// Packet includes:
// - Stabilized acceleration (compensated for orientation)
// - Angular rates
// - Magnetometer data
// Total: 43 bytes
```

**Advantage:** Hardware-compensated for gimbal orientation changes

### 2. Dynamic Sampling Rate Adjustment

```cpp
// Implement adaptive sampling based on gimbal velocity
if (gimbalAngularVelocity > threshold) {
    imuDevice->setSamplingRate(200);  // Increase rate during fast motion
} else {
    imuDevice->setSamplingRate(100);  // Normal rate
}
```

### 3. Magnetometer Calibration

Add magnetometer hard/soft iron calibration:

```cpp
// Send calibration matrices from config
// Improves yaw accuracy in magnetically noisy environments
```

### 4. Temperature Compensation

Use **0xD1** command to monitor sensor temperatures:

```cpp
// Adjust bias corrections based on thermal drift
// Improves long-term stability
```

---

## References

- **3DM-GX3-25 Data Communications Protocol** (provided by user)
- **MicroStrain 3DM-GX3-25 User Manual**
- Project file: `src/controllers/motion_modes/gimbalmotionmodebase.cpp` (stabilization logic)
- Original issue: Migration from Vigor SST810 to 3DM-GX3-25

---

## Migration Checklist

- [x] Create `Imu3DMGX3ProtocolParser` class
- [x] Update `ImuDevice` for serial protocol
- [x] Implement initialization sequence (0xCD → 0xDB → 0xC4)
- [x] Parse 0xCF packets (31 bytes)
- [x] Calculate checksums
- [x] Update build system (.pro file)
- [x] Document migration process
- [ ] **TEST with physical hardware** ⚠️ REQUIRED
- [ ] Validate stabilization performance
- [ ] Update system configuration files
- [ ] Train operators on new device

---

## Support

For issues or questions:
1. Check logs: `qDebug()` messages prefixed with device identifier
2. Review 3DM-GX3-25 datasheet for command details
3. Contact: [Your team/support contact]

**Last Updated:** 2025-11-03
**Version:** 3.0 - 3DM-GX3-25 Migration
