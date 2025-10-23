# 🎮 Weapon Station Joystick Control Manual

**Thrustmaster HOTAS Warthog Configuration**

---

## 📊 Quick Reference Card

| Control | Function | Mode Dependency |
|---------|----------|-----------------|
| **Main Stick** | Gimbal Control | Always Active |
| **Hat (D-Pad)** | Acquisition Box Resize | Acquisition Phase Only |
| **Button 3** | Dead Man Switch | Safety Critical |
| **Button 4** | Track Select/Abort | All Modes |
| **Button 5** | Fire Weapon | Engagement Mode |
| **Button 0** | Master Arm | Station Enabled |

---

## 🕹️ ANALOG CONTROLS

### **Main Stick (Axes)**

#### **X-Axis (Axis 0) - Azimuth Control**
- **Function:** Gimbal azimuth (left/right) velocity control
- **Scaling:** Value × 10.0 deg/s
- **Range:** -1.0 to +1.0
- **Neutral:** 0.0 (centered)
- **Mode:** Active in Manual motion mode

#### **Y-Axis (Axis 1) - Elevation Control**
- **Function:** Gimbal elevation (up/down) velocity control
- **Scaling:** -Value × 10.0 deg/s (inverted)
- **Range:** -1.0 to +1.0
- **Neutral:** 0.0 (centered)
- **Mode:** Active in Manual motion mode

---

## 🎯 HAT SWITCH (D-PAD)

### **8-Way Hat Switch (Hat 0)**

**Function:** Tracking Acquisition Box Resize

| Direction | Function | Step Size |
|-----------|----------|-----------|
| **UP ↑** | Decrease box height | -4 pixels |
| **DOWN ↓** | Increase box height | +4 pixels |
| **LEFT ←** | Decrease box width | -4 pixels |
| **RIGHT →** | Increase box width | +4 pixels |

**⚠️ IMPORTANT:**
- **Only active during Acquisition Phase** (TrackingPhase::Acquisition)
- Used to manually size the tracking gate before lock-on
- No effect in other tracking phases or surveillance modes

---

## 🔘 PRIMARY WEAPONS CONTROLS

### **Button 0 - MASTER ARM** (Trigger Stage 1)
- **Type:** Momentary switch
- **Function:** Master weapons engagement authorization
- **Action:** 
  - Press & Hold: Enable engagement systems
  - Release: Disengage
- **Safety:** Requires station to be powered on
- **Status:** Commands `SystemStateModel::commandEngagement()`

---

### **Button 5 - FIRE WEAPON** (Trigger Stage 2)
- **Type:** Momentary switch (hold-to-fire)
- **Function:** Primary weapon fire command
- **Action:**
  - Press: `WeaponController::startFiring()`
  - Release: `WeaponController::stopFiring()`
- **Safety:** Should be used in conjunction with Master Arm (Button 0)
- **Modes:** Respects current fire rate setting (Single/Burst)

---

### **Button 3 - DEAD MAN SWITCH** ⚠️
- **Type:** Safety switch (hold required)
- **Function:** Master safety interlock
- **Action:** Must be held for critical operations
- **Protected Operations:**
  - Lead Angle Compensation (LAC) toggle
  - Tracking initiation (recommended)
  - Fire control computer functions
- **Status:** `SystemStateModel::setDeadManSwitch(pressed)`

---

## 🎯 TRACKING CONTROLS

### **Button 4 - TRACK SELECT/ABORT** 🔴
**Multi-Function Button - Context Sensitive**

#### **Single Press Behavior:**

| Current Phase | Action | Result |
|---------------|--------|--------|
| **Off** | Start acquisition | → Acquisition Phase |
| **Acquisition** | Request lock-on | → Tracking_LockPending |
| **LockPending** | (No action) | Display message |
| **ActiveLock** | (No action) | Display message |
| **Coast/Firing** | (No action) | Display message |

#### **Double-Click (< 500ms):**
- **Function:** Emergency tracking abort
- **Action:** Immediate stop, return to Off phase
- **Use:** Cancel unwanted tracks or tracking errors

**💡 Tracking Workflow:**
```
OFF → [Press 4] → ACQUISITION → [Resize with D-Pad] → [Press 4] → LOCK PENDING → ACTIVE LOCK
                                                                   ↓
                                                        [Double-click 4] → OFF
```

---

## 📹 CAMERA CONTROLS

### **Button 6 - ZOOM IN** 🔍
- **Type:** Momentary switch
- **Action:**
  - Press: Zoom in (increase magnification)
  - Release: Stop zoom
- **Speed:** Continuous zoom while held
- **Cameras:** Day camera and Night/Thermal camera

---

### **Button 8 - ZOOM OUT** 🔍
- **Type:** Momentary switch
- **Action:**
  - Press: Zoom out (decrease magnification)
  - Release: Stop zoom
- **Speed:** Continuous zoom while held
- **Cameras:** Day camera and Night/Thermal camera

---

### **Button 7 - NEXT VIDEO LUT** (Thermal Only) 🌡️
- **Function:** Cycle thermal imaging color palette forward
- **Range:** LUT 0 → LUT 12
- **Camera:** Night/Thermal camera only (ignored on day camera)
- **Examples:** White Hot, Black Hot, Rainbow, Iron, etc.

---

### **Button 9 - PREVIOUS VIDEO LUT** (Thermal Only) 🌡️
- **Function:** Cycle thermal imaging color palette backward
- **Range:** LUT 12 → LUT 0
- **Camera:** Night/Thermal camera only (ignored on day camera)

---

## 🔄 MOTION MODE CYCLING

### **Button 11 or Button 13 - CYCLE MOTION MODES**
- **Function:** Cycle through surveillance scan patterns
- **Sequence:** 
  ```
  Manual → AutoSectorScan → TRPScan → RadarSlew → Manual (loop)
  ```

**Mode Descriptions:**

| Mode | Description |
|------|-------------|
| **Manual** | Full manual gimbal control via joystick |
| **AutoSectorScan** | Automated sector scanning pattern |
| **TRPScan** | Target Reference Point sequential scan |
| **RadarSlew** | Gimbal follows radar cues |

**⚠️ Safety Restrictions:**
- **Blocked during Acquisition Phase** - Cannot cycle while sizing tracking gate
- **Auto-stops Active Tracking** - Cycling from ActiveLock stops tracker first
- **Requires Station On** - Station must be powered

---

## 🎚️ UP/DOWN SELECTORS

### **Button 14 - UP/NEXT SELECTOR** ⬆️
**Context-Sensitive Function:**

| Operational Mode | Motion Mode | Function |
|------------------|-------------|----------|
| **Idle** | Any | `setUpSw()` |
| **Tracking** | Any | `setUpTrack()` |
| **Surveillance** | TRPScan | Select next TRP location page |
| **Surveillance** | AutoSectorScan | Select next sector scan zone |

---

### **Button 16 - DOWN/PREVIOUS SELECTOR** ⬇️
**Context-Sensitive Function:**

| Operational Mode | Motion Mode | Function |
|------------------|-------------|----------|
| **Idle** | Any | `setDownSw()` |
| **Tracking** | Any | `setDownTrack()` |
| **Surveillance** | TRPScan | Select previous TRP location page |
| **Surveillance** | AutoSectorScan | Select previous sector scan zone |

---

## 🎯 FIRE CONTROL SYSTEMS

### **Button 2 - LEAD ANGLE COMPENSATION (LAC) TOGGLE** 🎯
- **Type:** Toggle switch
- **Function:** Enable/Disable predictive lead angle calculation
- **Safety:** **Requires Dead Man Switch (Button 3) to be active**
- **Action:**
  - First press (LAC off): Enable LAC → Calculates lead for moving targets
  - Second press (LAC on): Disable LAC → Reticle returns to bore sight
- **Status Display:** 
  - "LEAD ANGLE ON" (green)
  - "LEAD ANGLE LAG" (yellow - tracking data insufficient)
  - "ZOOM OUT" (red - FOV too narrow for calculation)

**💡 Usage:**
```
1. Hold Button 3 (Dead Man Switch)
2. Press Button 2 to toggle LAC
3. Reticle shifts to predicted impact point
4. "LAC" indicator appears on CCIP reticle
```

---

## ⚙️ UNASSIGNED BUTTONS

The following buttons are currently **not mapped** in the code:

- **Button 1** - Available
- **Button 10** - Available
- **Button 12** - Available
- **Button 15** - Available
- **Button 17** - Available
- **Button 18** - Available
- **Button 19** - Available

These can be assigned to future functions such as:
- Weapon type selection
- Firing mode (Single/Burst/Auto)
- Stabilization toggle
- Emergency stop
- Preset position recall
- Reticle type selection

---

## 🚨 SAFETY PROTOCOLS

### **Critical Safety Rules:**

1. **Dead Man Switch (Button 3)** must be held for:
   - LAC activation
   - Critical fire control operations
   
2. **Master Arm (Button 0)** required before:
   - Weapon fire (Button 5)
   
3. **Double-click Button 4** for:
   - Emergency tracking abort
   
4. **Station must be powered** for:
   - Mode cycling (Buttons 11/13)
   - Engagement commands

---

## 📋 OPERATIONAL CHECKLISTS

### **Starting Tracking Sequence:**
```
☐ Power on station
☐ Select Manual motion mode
☐ Center target in reticle
☐ Press Button 4 (Enter Acquisition)
☐ Adjust box size with D-Pad
☐ Press Button 4 again (Request Lock-On)
☐ Wait for "ActiveLock" status
☐ Track is now active
```

### **Weapons Engagement Sequence:**
```
☐ Acquire and track target
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (Enable LAC if moving target)
☐ Verify CCIP reticle position
☐ Hold Button 0 (Master Arm)
☐ Press Button 5 (Fire)
☐ Release Button 5 when complete
```

### **Emergency Abort:**
```
☐ Double-click Button 4 rapidly (< 500ms)
☐ All tracking stops immediately
☐ System returns to Manual mode
```

---

## 🔧 TROUBLESHOOTING

| Issue | Check | Solution |
|-------|-------|----------|
| Joystick not moving gimbal | Motion mode | Switch to Manual mode (Button 11/13) |
| Can't start tracking | Dead Man Switch | Hold Button 3 (recommended) |
| D-Pad not working | Tracking phase | Must be in Acquisition phase |
| LAC won't enable | Dead Man Switch | Hold Button 3 while pressing Button 2 |
| Can't cycle modes | Station power | Ensure station is powered on |
| Button 4 double-click too sensitive | Timing | Wait > 500ms between presses |

---

## 📖 APPENDIX: Technical Details

### **Axis Scaling Formula:**
```cpp
Azimuth Velocity = AxisValue × 10.0 (deg/s)
Elevation Velocity = -AxisValue × 10.0 (deg/s) // Inverted
```

### **Acquisition Box Resize Step:**
```cpp
Width/Height change = ±4 pixels per D-Pad press
```

### **Double-Click Detection:**
```cpp
DOUBLE_CLICK_INTERVAL_MS = 500 // Half second window
```

### **Video LUT Range:**
```cpp
LUT Index: 0 to 12 (13 total palettes)
```

---

## ⚡ QUICK TIPS

💡 **Pro Tips:**
- Use small, smooth stick movements for precise gimbal control
- Size acquisition box slightly larger than target for best track
- Enable LAC before engaging moving targets
- Keep Dead Man Switch held during critical operations
- Double-click Track button anytime to abort bad tracks

🎯 **Optimal Settings:**
- Stick sensitivity: Medium (10.0 scale factor)
- Acquisition box: 20-30% larger than target
- LAC: Enable for targets moving > 5 m/s

---

**End of Manual** | Version 1.0 | Thrustmaster HOTAS Warthog Configuration