# APPENDIX I: OPERATOR MAINTENANCE LOG TEMPLATE

**Purpose:** Standardized maintenance logging and record-keeping templates

---

## I.1 OVERVIEW

### I.1.1 Purpose

This appendix provides templates for:
- Daily pre-operation inspections
- Post-operation checks
- Preventive maintenance tracking
- Fault and repair logging
- System health monitoring
- Maintenance history records

### I.1.2 Record-Keeping Requirements

**Operator Responsibilities:**
- Complete pre-operation inspection before each use
- Log all faults and anomalies immediately
- Complete post-operation checklist
- Sign and date all entries
- Report maintenance issues to supervisor
- Maintain legible and accurate records

**Retention:**
- Daily logs: 30 days
- Monthly summaries: 1 year
- Repair records: System lifetime
- Calibration records: 5 years

---

## I.2 DAILY PRE-OPERATION INSPECTION

### I.2.1 Pre-Operation Checklist

**System ID:** ____________  **Date:** __________  **Operator:** __________

**Start Time:** __________  **Mission/Task:** __________

#### Visual Inspection

```
☐ Gimbal assembly: No visible damage, cracks, or loose parts
☐ Cameras: Lenses clean, no cracks, protective caps removed
☐ Control panel: All buttons/switches functional, no damage
☐ Joystick: No loose buttons, stick moves smoothly
☐ Cables: No cuts, kinks, or exposed wires, all connections secure
☐ Mounting: Gimbal securely attached, no loose bolts
☐ Weather protection: Covers removed (if applicable)
```

#### System Power-Up

```
☐ Main power applied (24V DC): ______ VDC measured
☐ Main processor boots successfully (< 60 seconds)
☐ Display shows video feed
☐ No error messages on startup
```

#### Subsystem Connection Check

```
☐ PLC21 (Control Panel): Connected
☐ PLC42 (Gimbal Station): Connected
☐ Azimuth Servo: Connected, no faults
☐ Elevation Servo: Connected, no faults
☐ Day Camera: Connected
☐ Thermal Camera: Connected
☐ Laser Range Finder: Connected
☐ IMU/Gyroscope: Connected
☐ Weapon Actuator: Connected
☐ Joystick: Connected, all buttons responding
```

#### Functional Checks

```
☐ Gimbal moves smoothly in azimuth (full range test)
☐ Gimbal moves smoothly in elevation (full range test)
☐ No unusual noises during motion
☐ E-Stop functions (press and release test)
☐ Day camera zoom operates (wide to tele)
☐ Thermal camera LUT changes (test 3 palettes)
☐ Laser range finder ranges a target: ______ meters
☐ Reticle visible on display
☐ Menu navigation functional (UP/DOWN/VAL/MENU)
```

#### Safety Checks

```
☐ E-Stop resets properly (twist to release)
☐ Dead man switch functional (test press/release)
☐ Master arm switch functional
☐ No-fire zones loaded and active
☐ No-traverse zones loaded and active
☐ Stabilization can be enabled/disabled
```

#### Pre-Operation Status

**Overall System Status:**

☐ FULLY OPERATIONAL - No issues, ready for mission
☐ OPERATIONAL WITH NOTES - See remarks, safe to operate
☐ NOT OPERATIONAL - Faults present, do not operate

**Remarks:** ____________________________________________________________

________________________________________________________________________

**Operator Signature:** __________  **Date:** __________  **Time:** __________

---

## I.3 POST-OPERATION CHECKLIST

### I.3.1 Post-Operation Shutdown

**System ID:** ____________  **Date:** __________  **Operator:** __________

**End Time:** __________  **Total Operating Hours:** ______ hours

#### Shutdown Procedure

```
☐ Weapon cleared (if applicable)
☐ Gimbal returned to home/stow position
☐ Tracking stopped (if active)
☐ LAC disabled
☐ Zeroing/windage settings saved (if changed)
☐ Zone files saved (if modified)
☐ System shutdown via menu (graceful shutdown)
☐ Main power removed
```

#### Post-Operation Inspection

```
☐ Gimbal: No new damage or wear
☐ Cameras: Lenses clean, no new damage
☐ Cables: No new damage, all secured
☐ Control panel: All switches in neutral/off position
☐ Joystick: Stowed or protected
☐ Lens caps installed on cameras
☐ Weatherproof cover installed (if required)
```

#### Faults or Anomalies Observed

**List all issues encountered during operation:**

| Time | Subsystem | Issue Description | Action Taken |
|------|-----------|-------------------|--------------|
| ____ | _________ | _________________ | ____________ |
| ____ | _________ | _________________ | ____________ |
| ____ | _________ | _________________ | ____________ |

**Issues requiring maintenance:** ☐ Yes ☐ No

If yes, maintenance request submitted: ☐ Yes ☐ No

#### Usage Summary

**Operating Hours This Session:** ______ hours
**Cumulative Operating Hours:** ______ hours (odometer reading)

**Operations Performed:**
```
☐ Surveillance/patrol
☐ Target tracking
☐ Live fire
☐ Training/practice
☐ System test
☐ Other: _______________
```

**Environmental Conditions:**
- Temperature: ______ °C
- Humidity: ______ % (if known)
- Precipitation: ☐ None ☐ Light ☐ Heavy
- Dust/sand: ☐ None ☐ Moderate ☐ Severe
- Other: _______________

#### Post-Operation Status

☐ System returned in serviceable condition
☐ System requires cleaning/maintenance (see remarks)
☐ System requires repair (maintenance request submitted)

**Remarks:** ____________________________________________________________

________________________________________________________________________

**Operator Signature:** __________  **Date:** __________  **Time:** __________

---

## I.4 PREVENTIVE MAINTENANCE LOG

### I.4.1 Weekly Maintenance

**Week of:** __________  **Performed By:** __________

```
☐ Clean camera lenses (both day and thermal)
☐ Inspect all cables for wear, secure connections
☐ Check gimbal for smooth operation (no binding)
☐ Verify all indicator LEDs functional on control panel
☐ Test E-Stop function
☐ Check joystick buttons (all 20) for proper function
☐ Verify zone files backed up
☐ Review system logs for errors
☐ Clean dust/debris from gimbal housing
☐ Lubricate gimbal bearings (if accessible)
```

**Issues Found:** _______________________________________________________

________________________________________________________________________

**Corrective Actions:** _________________________________________________

________________________________________________________________________

**Signature:** __________  **Date:** __________

### I.4.2 Monthly Maintenance

**Month:** __________  **Performed By:** __________

```
☐ All weekly tasks completed
☐ Inspect servo drive connections (Az and El)
☐ Check servo temperatures under load (record below)
☐ Verify LRF accuracy (range known distance)
☐ Test IMU calibration (level platform test)
☐ Inspect weapon actuator for wear
☐ Check all power supply voltages (record below)
☐ Test battery backup (if equipped)
☐ Clean cooling vents/fans (if equipped)
☐ Update system software (if updates available)
☐ Backup configuration files
☐ Review and clear old log files
```

**Measurements:**

| Parameter | Specification | Measured | Status |
|-----------|---------------|----------|--------|
| Az Servo Temp (motor) | < 85°C | _____ °C | ☐ OK ☐ High |
| El Servo Temp (motor) | < 85°C | _____ °C | ☐ OK ☐ High |
| 48V Supply | 48±2V | _____ V | ☐ OK ☐ Out of spec |
| 24V Supply | 24±1V | _____ V | ☐ OK ☐ Out of spec |
| 12V Supply | 12±0.5V | _____ V | ☐ OK ☐ Out of spec |
| 5V Supply | 5±0.25V | _____ V | ☐ OK ☐ Out of spec |

**Issues Found:** _______________________________________________________

________________________________________________________________________

**Corrective Actions:** _________________________________________________

________________________________________________________________________

**Signature:** __________  **Date:** __________

### I.4.3 Quarterly Maintenance

**Quarter:** __________  **Performed By:** __________

```
☐ All monthly tasks completed
☐ Perform boresight/zeroing verification
☐ Calibrate IMU (if procedure available)
☐ Inspect all mechanical fasteners (torque check)
☐ Deep clean all components
☐ Inspect gimbal limit switches
☐ Test all safety interlocks
☐ Verify tracking system accuracy
☐ Test LAC function with moving target
☐ Inspect electrical connections for corrosion
☐ Update maintenance records to supervisor
☐ Schedule any depot-level maintenance required
```

**Issues Found:** _______________________________________________________

________________________________________________________________________

**Corrective Actions:** _________________________________________________

________________________________________________________________________

**Signature:** __________  **Date:** __________

---

## I.5 FAULT AND REPAIR LOG

### I.5.1 Fault Report Template

**Fault Report Number:** __________

**Date/Time Reported:** __________  **Reported By:** __________

**System ID:** __________  **Operating Hours:** ______ hours

**Fault Description:**

________________________________________________________________________

________________________________________________________________________

________________________________________________________________________

**Subsystem Affected:**

☐ Gimbal (Az/El servos)
☐ Day Camera
☐ Thermal Camera
☐ Laser Range Finder
☐ IMU/Gyroscope
☐ Control Panel (PLC21)
☐ Gimbal Station (PLC42)
☐ Weapon Actuator
☐ Joystick
☐ Main Processor
☐ Power System
☐ Communication/Network
☐ Other: _______________

**Fault Severity:**

☐ CRITICAL - System inoperable, safety hazard
☐ MAJOR - Subsystem inoperable, mission capability degraded
☐ MINOR - Reduced functionality, workaround available
☐ COSMETIC - No operational impact

**Immediate Action Taken:**

________________________________________________________________________

________________________________________________________________________

**System Operational Status:**

☐ Fully operational (fault cleared)
☐ Operational with reduced capability
☐ Not operational (awaiting repair)

**Maintenance Request Submitted:** ☐ Yes ☐ No

**Reported By (Signature):** __________  **Date:** __________

---

### I.5.2 Repair Record Template

**Repair Record Number:** __________  (links to Fault Report #: ______ )

**Date Repair Started:** __________  **Completed:** __________

**Technician:** __________  **Level:** ☐ Operator ☐ Field ☐ Depot

**Fault Diagnosis:**

________________________________________________________________________

________________________________________________________________________

**Repair Actions:**

________________________________________________________________________

________________________________________________________________________

________________________________________________________________________

**Parts Replaced:**

| Part Number | Nomenclature | Serial Number (old) | Serial Number (new) | Qty |
|-------------|--------------|---------------------|---------------------|-----|
| ___________ | ____________ | ___________________ | ___________________ | ___ |
| ___________ | ____________ | ___________________ | ___________________ | ___ |

**Post-Repair Test:**

```
☐ Functional test passed
☐ System returned to fully operational status
☐ Issue resolved, no further action required
```

**Follow-Up Required:** ☐ Yes ☐ No

If yes, describe: ________________________________________________________

**Technician Signature:** __________  **Date:** __________

**Supervisor Approval:** __________  **Date:** __________

---

## I.6 SYSTEM HEALTH MONITORING

### I.6.1 Temperature Monitoring Log

**Month:** __________

Record temperatures weekly or after extended operations:

| Date | Az Motor | Az Driver | El Motor | El Driver | IMU | LRF | Ambient | Notes |
|------|----------|-----------|----------|-----------|-----|-----|---------|-------|
| ____ | ____ °C | ____ °C | ____ °C | ____ °C | ____°C | ____°C | ____°C | _____ |
| ____ | ____ °C | ____ °C | ____ °C | ____ °C | ____°C | ____°C | ____°C | _____ |
| ____ | ____ °C | ____ °C | ____ °C | ____ °C | ____°C | ____°C | ____°C | _____ |
| ____ | ____ °C | ____ °C | ____ °C | ____ °C | ____°C | ____°C | ____°C | _____ |

**Temperature Limits:**
- Az/El Motor: 85°C max
- Az/El Driver: 60°C max
- IMU: 85°C max
- LRF: 60°C max

**Action if exceeded:** Reduce duty cycle, check cooling, initiate maintenance request

---

### I.6.2 Tracking Performance Log

**Month:** __________

Record tracking performance during operations:

| Date | Target Type | Range (m) | Lock Time (s) | Lost Track? | LAC Used? | Notes |
|------|-------------|-----------|---------------|-------------|-----------|-------|
| ____ | ___________ | ________ | _____________ | ☐ Y ☐ N | ☐ Y ☐ N | _____ |
| ____ | ___________ | ________ | _____________ | ☐ Y ☐ N | ☐ Y ☐ N | _____ |
| ____ | ___________ | ________ | _____________ | ☐ Y ☐ N | ☐ Y ☐ N | _____ |
| ____ | ___________ | ________ | _____________ | ☐ Y ☐ N | ☐ Y ☐ N | _____ |

**Performance Issues:** _________________________________________________

________________________________________________________________________

---

### I.6.3 Accuracy Verification Log

**Boresight/Zeroing Checks:**

| Date | Range (m) | Reticle Type | Offset (Az) | Offset (El) | Applied? | Technician |
|------|-----------|--------------|-------------|-------------|----------|------------|
| ____ | ________ | ____________ | _____ mrad | _____ mrad | ☐ Y ☐ N | _________ |
| ____ | ________ | ____________ | _____ mrad | _____ mrad | ☐ Y ☐ N | _________ |

**LRF Accuracy Checks:**

| Date | Known Distance | LRF Reading | Error | Status | Technician |
|------|----------------|-------------|-------|--------|------------|
| ____ | ________ m | ________ m | ±___ m | ☐ OK ☐ Fail | _________ |
| ____ | ________ m | ________ m | ±___ m | ☐ OK ☐ Fail | _________ |

**Accuracy Spec:** LRF ±1m

---

## I.7 CONSUMABLE TRACKING

### I.7.1 Consumable Usage Log

**Item:** Lens Cleaning Solution

| Date | Amount Used | Remaining | Reorder? | Signature |
|------|-------------|-----------|----------|-----------|
| ____ | _______ ml | ______ ml | ☐ Y ☐ N | _________ |
| ____ | _______ ml | ______ ml | ☐ Y ☐ N | _________ |

**Item:** Compressed Air

| Date | Canisters Used | Remaining | Reorder? | Signature |
|------|----------------|-----------|----------|-----------|
| ____ | _____________ | _________ | ☐ Y ☐ N | _________ |
| ____ | _____________ | _________ | ☐ Y ☐ N | _________ |

**Item:** Microfiber Cloths

| Date | Cloths Used | Remaining | Reorder? | Signature |
|------|-------------|-----------|----------|-----------|
| ____ | ___________ | _________ | ☐ Y ☐ N | _________ |
| ____ | ___________ | _________ | ☐ Y ☐ N | _________ |

---

## I.8 MONTHLY SUMMARY REPORT

**Month:** __________  **Prepared By:** __________

### I.8.1 Operating Summary

**Total Operating Hours (Month):** ______ hours
**Cumulative Operating Hours:** ______ hours
**Number of Missions:** ______
**Availability:** ______% (hours operational / hours required)

### I.8.2 Maintenance Summary

**Preventive Maintenance:**
- Weekly checks completed: ______ / 4
- Monthly check completed: ☐ Yes ☐ No

**Faults Reported:** ______
- Critical: ______
- Major: ______
- Minor: ______

**Repairs Completed:** ______
**Repairs Pending:** ______

### I.8.3 Parts and Consumables

**Parts Replaced:** ______________________________________________________

________________________________________________________________________

**Consumables Reordered:** ______________________________________________

________________________________________________________________________

### I.8.4 Recommendations

________________________________________________________________________

________________________________________________________________________

________________________________________________________________________

**Supervisor Signature:** __________  **Date:** __________

---

**END OF APPENDIX I**

**END OF RCWS OPERATOR MANUAL APPENDICES**
