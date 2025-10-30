# LESSON 12: SYSTEM STATUS & MONITORING

**Duration:** 2 hours
**Type:** Technical Knowledge Training
**Prerequisites:** Lessons 1-11

---

## LESSON OVERVIEW

This lesson covers the System Status display, teaching operators to monitor system health, interpret status indicators, identify faults, and understand alarm conditions. Essential for operational readiness and troubleshooting.

### TERMINAL LEARNING OBJECTIVE (TLO)

Upon completion, the student will be able to access the System Status display, interpret all status sections, identify fault conditions, and determine appropriate corrective actions.

### ENABLING LEARNING OBJECTIVES (ELO)

1. Access and navigate the System Status display
2. Interpret gimbal servo status (azimuth and elevation)
3. Monitor IMU (gyroscope) health and orientation
4. Check Laser Range Finder status and fault codes
5. Verify camera system status (day and thermal)
6. Monitor Control Panel (PLC) connections
7. Identify and interpret system alarms
8. Determine when to notify maintenance

---

## 12.1 ACCESSING SYSTEM STATUS

### 12.1.1 Access Procedure

**From Main Menu:**
```
☐ Press MENU button on Control Panel
☐ Navigate to "System Status" (SYSTEM section)
☐ Press VAL button to enter
☐ System Status display appears
```

**Exit Procedure:**
```
☐ Press MENU or BACK button
☐ Returns to Main Menu
```

### 12.1.2 Display Layout

**System Status sections displayed:**
1. Azimuth Servo
2. Elevation Servo
3. IMU (Inertial Measurement Unit)
4. Laser Range Finder (LRF)
5. Day Camera
6. Night/Thermal Camera
7. PLC Status (Control Panels)
8. Servo Actuator (Weapon)
9. Alarms/Warnings

**Update Rate:** Real-time (continuous updates while displayed)

---

## 12.2 GIMBAL SERVO STATUS

### 12.2.1 Azimuth Servo

**Displayed Parameters:**

| Parameter | Description | Normal Range | Units |
|-----------|-------------|--------------|-------|
| **Connected** | Communication status | ✓ Connected | Boolean |
| **Position** | Current azimuth angle | 0-360° | Degrees |
| **RPM** | Rotation speed | 0-60 | RPM |
| **Torque** | Motor load | 0-100 | % |
| **Motor Temp** | Motor temperature | 20-60°C | Celsius |
| **Driver Temp** | Driver electronics temp | 20-60°C | Celsius |
| **Fault** | Fault indicator | No Fault | Boolean |

**Status Interpretation:**

**✓ Normal:**
```
Connected: ✓
Position: 045.2°
RPM: 0
Torque: 15%
Motor Temp: 42°C
Driver Temp: 38°C
Fault: No
```

**⚠ Warning - High Temperature:**
```
Motor Temp: 72°C (>70°C threshold)
Driver Temp: 68°C

Action: Allow cooling, reduce continuous motion
```

**❌ Fault:**
```
Connected: ✗ (Disconnected)
OR
Fault: Yes

Action: Notify maintenance immediately
```

### 12.2.2 Elevation Servo

**Parameters identical to Azimuth Servo:**
- Connected status
- Position (elevation angle, -20° to +60°)
- RPM, Torque, Motor Temp, Driver Temp, Fault

**Normal Elevation Range:** -20° (down) to +60° (up)

**Fault Conditions:**
- Disconnection
- Over-temperature (>70°C)
- Fault flag active
- Position exceeding mechanical limits

---

## 12.3 IMU (INERTIAL MEASUREMENT UNIT)

### 12.3.1 IMU Parameters

**Displayed Information:**

| Parameter | Description | Range | Units |
|-----------|-------------|-------|-------|
| **Connected** | IMU communication | ✓ Connected | Boolean |
| **Roll** | Platform roll angle | ±180° | Degrees |
| **Pitch** | Platform pitch angle | ±90° | Degrees |
| **Yaw** | Platform heading | 0-360° | Degrees |
| **Temperature** | IMU sensor temp | 20-60°C | Celsius |

### 12.3.2 Orientation Interpretation

**Stable Platform (Ship/Vehicle Level):**
```
Roll: 0.5°
Pitch: -1.2°
Yaw: 045.0°

Interpretation: Platform nearly level, heading northeast
```

**Platform Tilted (Ship Roll):**
```
Roll: 12.3°
Pitch: 3.5°
Yaw: 090.0°

Interpretation: Platform rolling 12° (normal for ship at sea)
```

**Excessive Tilt Warning:**
```
Roll: >30° or Pitch: >30°

Action:
- Gimbal stabilization may be degraded
- Tracking more difficult
- Consider platform stability before engagement
```

### 12.3.3 IMU Faults

**Disconnected:**
```
Connected: ✗

Impact:
- Loss of stabilization reference
- Tracking degraded
- LAC calculations may be affected

Action: Notify maintenance immediately
```

---

## 12.4 LASER RANGE FINDER (LRF)

### 12.4.1 LRF Parameters

**Displayed Information:**

| Parameter | Description | Normal Value |
|-----------|-------------|--------------|
| **Connected** | LRF communication | ✓ Connected |
| **Distance** | Last measured range | 50-4000m |
| **Temperature** | LRF laser temp | 20-50°C |
| **Laser Count** | Total laser fires | Incrementing |
| **System Status** | LRF internal status | 0 (OK) |
| **Fault** | General fault flag | No |
| **No Echo** | No return signal | No |
| **Laser Not Out** | Laser emission failure | No |
| **Over Temp** | Temperature exceeded | No |

### 12.4.2 Normal LRF Status

```
Connected: ✓
Distance: 1235m
Temperature: 38°C
Laser Count: 4582
System Status: 0
Fault: No
No Echo: No
Laser Not Out: No
Over Temp: No
```

### 12.4.3 LRF Fault Conditions

**No Echo:**
```
No Echo: Yes

Meaning: Laser fired but no reflection detected
Causes:
- Target beyond max range (>4000m)
- Target too far below min range (<50m)
- Target highly absorptive (black, non-reflective)
- Atmospheric interference (heavy rain, fog, smoke)

Action: Re-lase, try different aim point, wait for clearer conditions
```

**Over Temperature:**
```
Over Temp: Yes
Temperature: 65°C

Meaning: LRF laser overheated
Causes:
- Excessive use (many rapid measurements)
- High ambient temperature
- Cooling system issue

Action: Stop lasing, allow cooling period (5-10 min)
```

**Laser Not Out:**
```
Laser Not Out: Yes

Meaning: Laser failed to emit
Causes:
- Laser diode failure
- Power supply issue
- Safety interlock active

Action: Notify maintenance
```

**LRF Disconnected:**
```
Connected: ✗

Impact: No range measurements possible
Action: Notify maintenance, check cables
```

---

## 12.5 CAMERA SYSTEMS

### 12.5.1 Day Camera Status

**Displayed Parameters:**

| Parameter | Description | Notes |
|-----------|-------------|-------|
| **Connected** | Day camera comm | ✓ = Operational |
| **Active** | Currently selected | Yes/No |
| **HFOV** | Horizontal Field of View | Degrees (wide to narrow) |
| **Zoom Position** | Current zoom level | 1.0x - 20.0x |
| **Focus Position** | Focus setting | 0-65535 |
| **Autofocus** | AF enabled | On/Off |
| **Error** | Camera error flag | No error expected |
| **Status** | Internal camera status | Status code |

**Normal Day Camera:**
```
Connected: ✓
Active: Yes (✓ = currently displayed)
HFOV: 12.5°
Zoom: 8.2x
Focus: 45230
Autofocus: On
Error: No
Status: 0 (OK)
```

### 12.5.2 Night/Thermal Camera Status

**Displayed Parameters:**

| Parameter | Description | Notes |
|-----------|-------------|-------|
| **Connected** | Thermal camera comm | ✓ = Operational |
| **Active** | Currently selected | Yes/No |
| **HFOV** | Horizontal Field of View | Degrees |
| **Zoom Level** | Digital zoom | 1.0x - 4.0x (digital) |
| **FFC In Progress** | Flat Field Correction | Yes/No |
| **Error** | Camera error flag | No error expected |
| **Status** | Internal camera status | Status code |
| **Video Mode** | LUT palette number | 0-12 |

**Normal Thermal Camera:**
```
Connected: ✓
Active: No (day camera active)
HFOV: 18.0°
Zoom: 2.0x
FFC In Progress: No
Error: No
Status: 0 (OK)
Video Mode: 3 (LUT 3)
```

**FFC (Flat Field Correction) in Progress:**
```
FFC In Progress: Yes

Meaning: Thermal camera performing calibration
Duration: 2-3 seconds
Effect: Display frozen briefly during FFC
Action: Normal operation, wait for completion
```

### 12.5.3 Camera Faults

**Disconnected:**
```
Connected: ✗

Impact: Loss of that camera feed
Action:
- Switch to other camera if available
- Notify maintenance
- Check power and communication cables
```

**Error Flag Active:**
```
Error: Yes

Meaning: Camera internal error detected
Action:
- Note what operations were occurring
- Attempt camera switch (off/on)
- If persistent, notify maintenance
```

---

## 12.6 PLC (CONTROL PANEL) STATUS

### 12.6.1 PLC Parameters

**Displayed Information:**

| Parameter | Description | Normal |
|-----------|-------------|--------|
| **PLC21 Connected** | Control Panel comm | ✓ Connected |
| **PLC42 Connected** | Gimbal Station comm | ✓ Connected |
| **Station Enabled** | Master power status | Enabled |
| **Gun Armed** | Weapon arming status | Armed/Disarmed |

**Normal PLC Status:**
```
PLC21: ✓ Connected (Operator Control Panel)
PLC42: ✓ Connected (Gimbal Station Hardware)
Station: Enabled
Gun Armed: No (safe state)
```

### 12.6.2 PLC Connection Loss

**PLC21 Disconnected:**
```
PLC21: ✗ Disconnected

Impact:
- Loss of Control Panel buttons/switches
- Cannot use MENU, VAL, UP, DOWN buttons
- Cannot toggle switches (Master Arm, LRF, etc.)
- System inoperable from Control Panel

Action: Notify maintenance immediately - CRITICAL
```

**PLC42 Disconnected:**
```
PLC42: ✗ Disconnected

Impact:
- Loss of gimbal station hardware monitoring
- May lose weapon actuator control
- Safety systems may be affected

Action: Notify maintenance immediately - CRITICAL
```

### 12.6.3 Station Enabled Status

**Station Disabled:**
```
Station: Disabled

Meaning: Master power switch OFF on Control Panel

Impact:
- Gimbal motion restricted
- Weapon systems disabled
- Some functions unavailable

Action: Turn Station power ON (if intentional disable)
```

---

## 12.7 SERVO ACTUATOR (WEAPON)

### 12.7.1 Actuator Parameters

**Displayed Information:**

| Parameter | Description | Normal Range |
|-----------|-------------|--------------|
| **Connected** | Actuator communication | ✓ Connected |
| **Position** | Trigger/bolt position | 0-100% |
| **Velocity** | Movement speed | RPM |
| **Temperature** | Motor temperature | 20-60°C |
| **Bus Voltage** | Power supply voltage | Volts |
| **Torque** | Motor torque | % |
| **Motor Off** | Motor status | No (motor on) |
| **Fault** | Fault indicator | No Fault |

**Normal Actuator (Idle):**
```
Connected: ✓
Position: 0% (weapon ready)
Velocity: 0 RPM
Temperature: 35°C
Bus Voltage: 24.5V
Torque: 0%
Motor Off: No
Fault: No
```

**During Firing:**
```
Position: Cycling (0-100-0%)
Velocity: 150 RPM
Torque: 60%
Temperature: Rising
```

### 12.7.2 Actuator Faults

**Disconnected:**
```
Connected: ✗

Impact: Loss of weapon fire control
Action: Notify maintenance - CRITICAL
```

**Over Temperature:**
```
Temperature: >70°C

Cause: Sustained fire, cooling insufficient
Action: Cease fire, allow cooling
```

**Fault Flag:**
```
Fault: Yes

Meaning: Internal actuator error
Action: Cease fire, safe weapon, notify maintenance
```

---

## 12.8 ALARMS AND WARNINGS

### 12.8.1 Alarm List

**System automatically generates alarms for:**

| Alarm | Severity | Description |
|-------|----------|-------------|
| **⚠ EMERGENCY STOP ACTIVE** | CRITICAL | E-stop button pressed |
| **⚠ Az/El Servo Fault** | CRITICAL | Gimbal motor fault |
| **⚠ Az/El Servo Disconnected** | CRITICAL | Lost comm with servo |
| **⚠ Az/El Driver Temp High** | WARNING | Driver >70°C |
| **⚠ Az/El Motor Temp High** | WARNING | Motor >70°C |
| **⚠ IMU Disconnected** | CRITICAL | No gyro data |
| **⚠ LRF Disconnected** | WARNING | No range data |
| **⚠ LRF Fault** | WARNING | LRF internal error |
| **⚠ LRF Over Temperature** | WARNING | LRF too hot |
| **⚠ Day Camera Disconnected** | WARNING | No day video |
| **⚠ Day Camera Error** | WARNING | Camera fault |
| **⚠ Night Camera Disconnected** | WARNING | No thermal video |
| **⚠ Night Camera Error** | WARNING | Camera fault |
| **⚠ PLC21 Disconnected** | CRITICAL | Control Panel lost |
| **⚠ PLC42 Disconnected** | CRITICAL | Station hardware lost |
| **ℹ Station Disabled** | INFO | Power switch OFF |
| **✓ All Systems Nominal** | OK | No faults |

### 12.8.2 Alarm Response Priority

**CRITICAL (Red) - Immediate Action Required:**
- Emergency Stop Active → Clear E-stop, investigate cause
- Servo Fault/Disconnected → Notify maintenance, system inoperable
- PLC Disconnected → Notify maintenance, system inoperable
- IMU Disconnected → Notify maintenance, stabilization lost

**WARNING (Yellow) - Prompt Action Required:**
- High Temperature → Allow cooling, reduce usage
- LRF Fault → Re-lase, if persistent notify maintenance
- Camera Disconnected/Error → Switch cameras, notify maintenance

**INFO (Blue) - Informational:**
- Station Disabled → Intentional power off, no action unless unintended

---

## 12.9 SYSTEM HEALTH CHECKS

### 12.9.1 Pre-Operation System Check

**Procedure:**
```
☐ Access System Status display
☐ Verify all "Connected" parameters show ✓
☐ Check temperatures (all <60°C)
☐ Verify "No Fault" on all devices
☐ Check alarms: Should show "✓ All Systems Nominal"
☐ If any warnings: Address before operations
```

**Go/No-Go Decision:**

**GO (System Ready):**
- All devices connected
- No faults
- Temperatures normal
- Alarms nominal

**NO-GO (System Not Ready):**
- Any CRITICAL alarm present
- Servo faults or disconnections
- PLC disconnections
- Multiple warnings

### 12.9.2 During Operations Monitoring

**Periodic Checks (Every 30-60 minutes):**
```
☐ Quick glance at System Status
☐ Verify temperatures not climbing
☐ Check for new alarms
☐ Monitor LRF laser count (excessive use?)
```

**After Intensive Operations:**
```
☐ Check servo temperatures after prolonged scanning
☐ Check LRF temperature after many range measurements
☐ Allow cooling if temperatures >60°C
```

---

## 12.10 TROUBLESHOOTING COMMON ISSUES

### Issue: One Servo Disconnected

**Symptoms:**
- Az or El Servo shows "✗ Disconnected"
- Gimbal may be unresponsive in that axis

**Troubleshooting:**
```
☐ Check physical cable connections to servo
☐ Power cycle system
☐ If still disconnected: Notify maintenance (cable or servo fault)
```

---

### Issue: LRF "No Echo" Every Shot

**Symptoms:**
- LRF connected but always returns "No Echo"
- No range measurements successful

**Troubleshooting:**
```
☐ Verify target within 50-4000m range
☐ Ensure target has reflective surface (not pure black)
☐ Check for atmospheric interference (fog, rain, smoke)
☐ Test at different range (try 200m known target)
☐ If persistent at all ranges: LRF hardware fault, notify maintenance
```

---

### Issue: Camera Frozen

**Symptoms:**
- Camera connected but image not updating
- Status shows "FFC In Progress" for >10 seconds

**Troubleshooting:**
```
☐ Wait 30 seconds (may be normal processing delay)
☐ Switch to other camera
☐ Switch back to problem camera
☐ If still frozen: Note error status, notify maintenance
```

---

### Issue: High Temperature Alarms

**Symptoms:**
- Multiple temperature warnings (>70°C)
- After prolonged operations

**Troubleshooting:**
```
☐ Cease intensive operations (stop scanning, tracking)
☐ Allow 10-15 minute cooling period
☐ Ensure ventilation not blocked
☐ Check ambient temperature (extreme heat environment?)
☐ If temperatures don't decrease: Cooling system fault, notify maintenance
```

---

## 12.11 MAINTENANCE NOTIFICATION CRITERIA

### 12.11.1 When to Notify Maintenance

**Immediately (Stop Operations):**
- ❌ Servo fault or disconnection
- ❌ PLC21 or PLC42 disconnection
- ❌ IMU disconnection
- ❌ Emergency stop won't clear
- ❌ Weapon actuator fault

**Soon (Complete Current Task):**
- ⚠ Persistent LRF faults
- ⚠ Camera disconnection or errors
- ⚠ Temperatures not cooling after rest
- ⚠ Multiple simultaneous warnings

**At Convenient Time (After Mission):**
- Minor camera glitches that resolved
- Single temperature spike that cooled normally
- Intermittent "No Echo" under expected conditions (fog, etc.)

### 12.11.2 Information to Provide Maintenance

**When Reporting Fault:**
```
☐ Which device/subsystem
☐ Specific error (disconnected, fault, temp, etc.)
☐ When did it start (before/during operations)
☐ What were you doing when fault occurred
☐ Screenshot of System Status if possible
☐ Any recent system changes or maintenance
```

---

## 12.12 PROFICIENCY EXERCISES

### Exercise 1: System Status Interpretation (20 min)

**Objective:** Identify system state from status display

**Procedure:**
```
Instructor presents System Status screenshots
Student identifies:
☐ All connected devices
☐ Any fault conditions
☐ Temperature status
☐ Go/No-Go decision for operations
```

**Performance Standard:** 100% accuracy in identifying faults

---

### Exercise 2: Pre-Operation Check (15 min)

**Objective:** Perform complete system health check

**Procedure:**
```
☐ Access System Status
☐ Check all 9 subsystems
☐ Record any warnings/faults
☐ Make Go/No-Go decision
☐ Brief instructor on findings
```

**Performance Standard:** Complete check in <5 minutes

---

### Exercise 3: Fault Response Drill (30 min)

**Objective:** Respond appropriately to system faults

**Scenarios:**
```
Scenario 1: LRF Disconnected
→ Student identifies, determines impact, notifies maintenance

Scenario 2: High Servo Temperature
→ Student ceases operations, allows cooling, monitors

Scenario 3: Camera Error
→ Student switches cameras, tests, reports fault if persistent
```

**Performance Standard:** Correct response to all scenarios

---

## 12.13 LESSON SUMMARY

### Key Points

1. **Access System Status** via Main Menu → System Status

2. **Nine major subsystems** monitored: Az Servo, El Servo, IMU, LRF, Day Camera, Night Camera, PLCs, Servo Actuator, Alarms

3. **Servo monitoring:** Connection, position, RPM, torque, temperatures, faults

4. **IMU provides:** Platform orientation (roll, pitch, yaw) for stabilization

5. **LRF faults:** No Echo (no reflection), Over Temp (too hot), Laser Not Out (emission failure)

6. **Cameras:** Monitor connection, zoom, focus, errors, and thermal FFC status

7. **PLC disconnection:** CRITICAL - system inoperable, notify maintenance immediately

8. **Temperature thresholds:** >70°C = warning, cease operations and cool

9. **Alarm priorities:** CRITICAL (immediate), WARNING (prompt), INFO (informational)

10. **Pre-operation check:** Verify all connected, no faults, normal temps, alarms nominal

---

**END OF LESSON 12**
