# LESSON 8: ADVANCED JOYSTICK OPERATIONS

**Duration:** 4 hours
**Type:** Practical Skills Training
**Prerequisites:** Lessons 1-7

---

## LESSON OVERVIEW

This lesson provides comprehensive training on advanced joystick control techniques for the El 7arress RCWS. Students will master all button functions, context-sensitive controls, and develop proficiency in complex multi-function operations.

### TERMINAL LEARNING OBJECTIVE (TLO)

Upon completion of this lesson, the student will be able to operate all joystick controls with proficiency, demonstrating proper button usage in various operational contexts and executing complex multi-step procedures.

### ENABLING LEARNING OBJECTIVES (ELO)

1. Identify all 20 joystick buttons and their primary functions
2. Execute context-sensitive button operations based on system state
3. Perform smooth analog stick control for gimbal positioning
4. Demonstrate proper tracking controls using multi-step button sequences
5. Execute fire control procedures with correct safety protocols
6. Operate camera controls including zoom and thermal palette selection
7. Perform motion mode cycling and zone selection
8. Apply proper safety interlocks and dead man switch procedures

---

## 8.1 JOYSTICK OVERVIEW

### 8.1.1 Hardware Configuration

The El 7arress RCWS uses a professional-grade HOTAS (Hands On Throttle And Stick) style joystick controller optimized for precision weapon station operations.

**Primary Components:**
- **Main Stick:** 2-axis analog control (X/Y)
- **Trigger:** 2-stage fire control
- **Throttle Section:** Mode controls and auxiliary functions
- **Hat Switch (D-Pad):** 8-way directional control
- **20 Programmable Buttons:** Multi-function controls

### 8.1.2 Control Philosophy

**Hands-On Operation:** All critical functions accessible without removing hands from controls.

**Context-Sensitive Design:** Button functions adapt based on current system state (Idle, Tracking, Surveillance, Engagement).

**Safety Interlocks:** Critical operations require deliberate multi-step actions to prevent accidental activation.

---

## 8.2 ANALOG CONTROLS

### 8.2.1 Main Stick - Gimbal Control

#### **X-Axis (Azimuth Control)**
- **Function:** Left/right gimbal movement
- **Range:** -1.0 (full left) to +1.0 (full right)
- **Neutral:** 0.0 (centered)
- **Scaling:** Value × 10.0 deg/s
- **Active Mode:** Manual motion mode

**Operation:**
```
Stick Position → Gimbal Response
Full Left  (-1.0) → -10.0 deg/s (counterclockwise)
Center     (0.0)  →   0.0 deg/s (stationary)
Full Right (+1.0) → +10.0 deg/s (clockwise)
```

#### **Y-Axis (Elevation Control)**
- **Function:** Up/down gimbal movement
- **Range:** -1.0 (full forward) to +1.0 (full back)
- **Neutral:** 0.0 (centered)
- **Scaling:** -Value × 10.0 deg/s (inverted)
- **Active Mode:** Manual motion mode

**Operation:**
```
Stick Position → Gimbal Response
Forward (-1.0) → +10.0 deg/s (gimbal up)
Center  (0.0)  →   0.0 deg/s (stationary)
Back    (+1.0) → -10.0 deg/s (gimbal down)
```

### 8.2.2 Smooth Control Techniques

**Fine Positioning (Small Movements):**
- Use 10-30% stick deflection
- Smooth, gradual inputs
- Allow gimbal to settle before adjusting

**Rapid Slewing (Large Movements):**
- 70-100% stick deflection
- Anticipate momentum and lead your stop point
- Return to center when target area reached

**Target Tracking:**
- Small circular motions to keep target centered
- Avoid jerky inputs - smooth and predictable
- Let system stabilization assist your movements

### 8.2.3 Stick Control Exercises

**Exercise 1: Precision Positioning**
1. Start with gimbal at home position (Az: 0°, El: 0°)
2. Move to Az: +45°, El: +15° using smooth inputs
3. Hold position steady for 10 seconds
4. Return to home position
5. Repeat for all quadrants

**Exercise 2: Figure-Eight Pattern**
1. Trace smooth figure-eight pattern in field of view
2. Maintain constant speed throughout
3. Practice at different speeds (slow, medium, fast)
4. Minimize jerky transitions at pattern crossover

**Exercise 3: Horizon Tracking**
1. Identify distant horizon or building line
2. Track smoothly from left to right
3. Maintain constant elevation
4. Practice both directions

---

## 8.3 HAT SWITCH (D-PAD)

### 8.3.1 Function Overview

The 8-way hat switch (D-Pad) is a **context-sensitive control** with function changing based on tracking phase.

**Primary Function:** Tracking Acquisition Box Resize (during Acquisition Phase only)

**Inactive States:** No effect during other tracking phases or surveillance modes

### 8.3.2 Acquisition Box Resize

**When Active:** Only during Tracking Phase = Acquisition

**Step Size:** ±4 pixels per press

**Directional Functions:**

| Direction | Function | Change |
|-----------|----------|--------|
| UP ↑ | Decrease box height | -4 pixels |
| DOWN ↓ | Increase box height | +4 pixels |
| LEFT ← | Decrease box width | -4 pixels |
| RIGHT → | Increase box width | +4 pixels |

**Diagonal Inputs:**
- UP-LEFT: Decrease both width and height
- UP-RIGHT: Decrease height, increase width
- DOWN-LEFT: Increase height, decrease width
- DOWN-RIGHT: Increase both width and height

### 8.3.3 Box Sizing Best Practices

**Target Coverage:**
- Box should be 20-30% larger than target
- Too small: Tracker may lose target on movements
- Too large: Tracker may lock onto background clutter

**Shape Adjustment:**
- Tall targets (poles, towers): Increase height, decrease width
- Wide targets (vehicles, buildings): Increase width, decrease height
- Compact targets (personnel): Square box

**Rapid Sizing:**
- Hold hat direction for continuous adjustment
- Watch real-time preview of box on display
- Release when optimal size achieved

---

## 8.4 PRIMARY WEAPONS CONTROLS

### 8.4.1 Button 0 - Master Arm

**Type:** Momentary switch (press and hold)
**Location:** Trigger Stage 1

**Function:** Master weapons engagement authorization

**Operation:**
- **Press:** Enables engagement systems
- **Release:** Disengages weapons systems
- **Status Call:** `SystemStateModel::commandEngagement(pressed)`

**Safety Requirements:**
- Station must be powered on
- No effect if station disabled

**Indicator:**
- Green "MASTER ARM" text on display when active
- Audible tone (optional configuration)

**Usage Guidelines:**
- Hold continuously during engagement sequence
- Release immediately after firing complete
- Never hold unnecessarily - fatigue risk

### 8.4.2 Button 5 - Fire Weapon

**Type:** Momentary switch (press and hold)
**Location:** Trigger Stage 2

**Function:** Primary weapon fire command

**Operation:**
- **Press:** `WeaponController::startFiring()`
- **Release:** `WeaponController::stopFiring()`

**Safety Requirements:**
- Station must be powered on
- Should be used with Master Arm (Button 0) active
- No effect if station disabled

**Fire Modes:**
- Single Shot: Quick press/release
- Burst Fire: Hold for duration of burst
- Fire rate determined by weapon configuration

**Procedure:**
1. Hold Button 0 (Master Arm)
2. Press Button 5 (Fire)
3. Release Button 5 when firing complete
4. Release Button 0 to disarm

### 8.4.3 Button 3 - Dead Man Switch

**Type:** Safety interlock switch (press and hold)
**Location:** Side grip button

**Function:** Master safety control for critical operations

**Status Call:** `SystemStateModel::setDeadManSwitch(pressed)`

**Protected Operations:**
- Lead Angle Compensation toggle (Button 2)
- Tracking initiation (Button 4) - recommended safety practice
- Fire control computer functions
- Critical ballistics calculations

**Safety Philosophy:**
- Requires deliberate, continuous operator presence
- Automatic disengagement if operator incapacitated
- Must be held throughout protected operations

**Operational Notes:**
- Position hand/thumb for comfortable hold
- Avoid fatigue - release when not needed
- Practice smooth engagement/disengagement

---

## 8.5 TRACKING CONTROLS

### 8.5.1 Button 4 - Track Select/Abort

**Type:** Multi-function button (context-sensitive)
**Location:** Stick top button

**Functions:**
- Single press: Advance tracking phase
- Double-click (<500ms): Emergency abort

**Safety Requirement:**
- Dead Man Switch (Button 3) must be active (recommended)
- System blocks operation if Dead Man Switch inactive

### 8.5.2 Single Press Behavior

**Phase-Dependent Actions:**

| Current Phase | Single Press Action | Next Phase |
|---------------|---------------------|------------|
| **Off** | Start acquisition | Acquisition |
| **Acquisition** | Request lock-on | Lock Pending |
| **Lock Pending** | No action | (Display message) |
| **Active Lock** | No action | (Display message) |
| **Coast** | No action | (Display message) |
| **Firing** | No action | (Display message) |

**Tracking Workflow:**
```
Step 1: [Press Button 4] → Enter ACQUISITION phase
Step 2: [Adjust box with D-Pad] → Size tracking gate
Step 3: [Press Button 4] → Request LOCK-ON
Step 4: System processes → LOCK PENDING
Step 5: Automatic transition → ACTIVE LOCK
```

### 8.5.3 Double-Click Emergency Abort

**Timing Window:** < 500 milliseconds between presses

**Function:** Immediate tracking termination from any phase

**Effect:**
- Instantly stops all tracking operations
- Returns to Tracking Phase = Off
- Clears tracking gate from display
- Returns to Manual motion mode

**When to Use:**
- Tracking wrong target
- Target moves into restricted zone
- Loss of positive target identification
- System behaving erratically
- Emergency situation requires immediate stop

**Technique:**
- Two rapid presses in quick succession
- Do not need to hold Dead Man Switch for abort
- Confirm tracking stopped (gate disappears)

### 8.5.4 Tracking Control Exercise

**Complete Tracking Sequence:**
```
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 4 (Enter Acquisition)
☐ Verify acquisition box appears
☐ Adjust box size with D-Pad
☐ Press Button 4 again (Request Lock)
☐ Wait for Active Lock status
☐ Observe target tracking
☐ Double-click Button 4 (Emergency Abort)
☐ Verify tracking stopped
```

---

## 8.6 CAMERA CONTROLS

### 8.6.1 Button 6 - Zoom In

**Type:** Momentary switch (press and hold)

**Function:** Increase camera magnification

**Operation:**
- **Press:** Continuous zoom in while held
- **Release:** Stop zoom operation
- **Cameras:** Day camera and Thermal camera

**Controller Call:** `CameraController::zoomIn()`
**Stop Call:** `CameraController::zoomStop()`

**Zoom Characteristics:**
- Day Camera: 20x optical zoom (smooth, continuous)
- Thermal Camera: Digital zoom (stepped or continuous based on model)

**Usage:**
- Short press: Small zoom increment
- Hold: Continuous zoom until released
- Monitor field of view to avoid over-zoom

### 8.6.2 Button 8 - Zoom Out

**Type:** Momentary switch (press and hold)

**Function:** Decrease camera magnification

**Operation:**
- **Press:** Continuous zoom out while held
- **Release:** Stop zoom operation
- **Cameras:** Day camera and Thermal camera

**Controller Call:** `CameraController::zoomOut()`
**Stop Call:** `CameraController::zoomStop()`

**Usage:**
- Return to wide field of view for situational awareness
- Find targets before zooming in
- Avoid minimum zoom limits

### 8.6.3 Zoom Best Practices

**Target Acquisition Zoom Sequence:**
1. Start at wide field of view (zoomed out)
2. Locate general target area
3. Zoom in incrementally
4. Stop when target fills 30-50% of frame
5. Avoid maximum zoom unless required

**Tracking Considerations:**
- Wider FOV = Easier tracking, lower detail
- Narrow FOV = Harder tracking, higher detail
- Adjust zoom based on target range and motion

**LAC Zoom Limitation:**
- Lead Angle Compensation requires sufficient FOV
- "ZOOM OUT" warning appears if FOV too narrow
- Zoom out if LAC needed for moving target

### 8.6.4 Button 7 - Next Video LUT (Thermal Only)

**Type:** Momentary button (single press)

**Function:** Cycle thermal imaging color palette forward

**Range:** LUT 0 → LUT 1 → ... → LUT 12 (13 total palettes)

**Camera Restriction:** Thermal camera only (ignored on day camera)

**Controller Call:** `CameraController::nextVideoLUT()`

**Common LUT Palettes:**
- LUT 0: White Hot (hot = white, cold = black)
- LUT 1: Black Hot (hot = black, cold = white)
- LUT 2: Rainbow
- LUT 3: Iron
- LUT 4: Lava
- LUT 12: Color (generic multi-color)

**Usage:**
- Select palette based on target signature and background
- White Hot: General purpose, good contrast
- Black Hot: Reduced glare, better for very hot targets
- Color palettes: Enhanced detail discrimination

### 8.6.5 Button 9 - Previous Video LUT (Thermal Only)

**Type:** Momentary button (single press)

**Function:** Cycle thermal imaging color palette backward

**Range:** LUT 12 → LUT 11 → ... → LUT 0

**Camera Restriction:** Thermal camera only

**Controller Call:** `CameraController::prevVideoLUT()`

**Usage:**
- Quickly return to preferred palette
- Compare adjacent palettes for best target contrast

### 8.6.6 Camera Control Exercises

**Exercise 1: Zoom Control Proficiency**
1. Start at minimum zoom (widest FOV)
2. Identify distant object
3. Zoom in smoothly to maximum magnification
4. Hold steady for target observation
5. Zoom out smoothly to minimum
6. Repeat 5 times for muscle memory

**Exercise 2: Thermal Palette Optimization**
1. Switch to thermal camera
2. Identify warm target (vehicle, person, equipment)
3. Cycle through all LUTs (Button 7)
4. Identify LUT with best target contrast
5. Return to that LUT using Button 9
6. Repeat with different target types

---

## 8.7 FIRE CONTROL SYSTEMS

### 8.7.1 Button 2 - Lead Angle Compensation (LAC) Toggle

**Type:** Toggle switch (press to change state)

**Function:** Enable/disable predictive lead angle calculation for moving targets

**Safety Requirement:** **CRITICAL - Dead Man Switch (Button 3) MUST be held**

**Operation:**
```
State: LAC OFF → Press Button 2 (while holding Button 3) → LAC ON
State: LAC ON  → Press Button 2 (while holding Button 3) → LAC OFF
```

**Controller Call:** `SystemStateModel::setLeadAngleCompensationActive(!currentState)`

**Status Indicators:**

| Status | Display | Color | Meaning |
|--------|---------|-------|---------|
| **LEAD ANGLE ON** | Solid text | Green | LAC active, calculations valid |
| **LEAD ANGLE LAG** | Warning | Yellow | LAC active, tracking data insufficient |
| **ZOOM OUT** | Warning | Red | FOV too narrow for LAC calculation |
| *No indicator* | - | - | LAC disabled |

### 8.7.2 LAC Activation Procedure

**Step-by-Step:**
```
☐ Establish active track on moving target
☐ Verify target velocity > 5 m/s
☐ Check camera FOV (avoid maximum zoom)
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (Toggle LAC)
☐ Verify "LEAD ANGLE ON" appears (green)
☐ Observe reticle shift to lead position
☐ "LAC" bracket appears on CCIP reticle
☐ Monitor LAC status throughout engagement
```

### 8.7.3 LAC Deactivation Procedure

**When to Deactivate:**
- Target stopped moving
- Lost track of target
- "ZOOM OUT" warning appears
- "LEAD ANGLE LAG" persists
- Engagement complete

**Procedure:**
```
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (Toggle LAC)
☐ Verify "LEAD ANGLE ON" disappears
☐ Reticle returns to boresight alignment
☐ "LAC" bracket removed from display
```

### 8.7.4 LAC Safety Notes

**Blocked Operation:**
If Button 2 pressed without Dead Man Switch (Button 3):
- System logs: "Cannot toggle Lead Angle Compensation"
- No state change occurs
- Prevents accidental LAC activation

**Usage Guidelines:**
- Only enable for moving targets
- Disable when not needed (reduces computational load)
- Monitor status indicators continuously
- Deactivate if warnings appear

---

## 8.8 MOTION MODE CYCLING

### 8.8.1 Button 11 or Button 13 - Cycle Motion Modes

**Type:** Momentary button (single press either button)

**Function:** Cycle through surveillance scan patterns

**Cycle Sequence:**
```
Manual → AutoSectorScan → TRPScan → RadarSlew → Manual (loops)
```

**Mode Descriptions:**

| Mode | Description | Gimbal Control |
|------|-------------|----------------|
| **Manual** | Full manual joystick control | Operator stick input |
| **AutoSectorScan** | Automated sector scanning | Automatic scan pattern |
| **TRPScan** | Sequential TRP navigation | Automatic TRP-to-TRP |
| **RadarSlew** | Follow radar cues | Automatic radar slewing |

### 8.8.2 Mode Cycling Restrictions

**BLOCKED During Acquisition Phase:**
- Cannot cycle modes while sizing tracking gate
- Prevents accidental mode change during critical operation
- System logs: "Cannot cycle motion modes during Tracking Acquisition"

**AUTO-STOPS Active Tracking:**
- If in Active Lock and button pressed, tracking stops first
- System automatically transitions to Manual mode
- Then mode cycling continues normally

**Requires Station Power:**
- Station must be enabled (powered on)
- System logs: "Cannot cycle modes, station is off" if blocked

### 8.8.3 Mode Cycling Procedure

**Basic Cycling:**
```
☐ Verify station powered on
☐ Ensure not in Acquisition phase
☐ Press Button 11 or Button 13
☐ Observe current mode indicator change
☐ Mode transitions immediately
☐ Verify gimbal behavior changes accordingly
```

**Cycling from Active Track:**
```
☐ Currently in Active Lock (tracking target)
☐ Press Button 11 or Button 13
☐ System stops tracking automatically
☐ Tracking gate disappears
☐ Mode changes to Manual
☐ Press again to continue cycling
```

---

## 8.9 UP/DOWN SELECTORS

### 8.9.1 Button 14 - UP/NEXT Selector

**Type:** Context-sensitive button

**Functions Vary by Operational Mode:**

| Operational Mode | Motion Mode | Function |
|------------------|-------------|----------|
| **Idle** | Any | Call `setUpSw()` |
| **Tracking** | Any | Call `setUpTrack()` |
| **Surveillance** | TRPScan | Select next TRP location |
| **Surveillance** | AutoSectorScan | Select next scan zone |

### 8.9.2 Button 16 - DOWN/PREVIOUS Selector

**Type:** Context-sensitive button

**Functions Vary by Operational Mode:**

| Operational Mode | Motion Mode | Function |
|------------------|-------------|----------|
| **Idle** | Any | Call `setDownSw()` |
| **Tracking** | Any | Call `setDownTrack()` |
| **Surveillance** | TRPScan | Select previous TRP location |
| **Surveillance** | AutoSectorScan | Select previous scan zone |

### 8.9.3 Zone Selection in Surveillance Mode

**TRPScan Mode:**
```
Button 14 → Next TRP in sequence
Button 16 → Previous TRP in sequence

Display shows: "TRP 2 of 5" (example)
Gimbal automatically slews to selected TRP
```

**AutoSectorScan Mode:**
```
Button 14 → Next sector scan zone
Button 16 → Previous sector scan zone

Display shows: "Scan Zone 3: Perimeter North"
Gimbal begins scanning selected zone
```

**Selection Wrap-Around:**
- At last item, pressing Button 14 returns to first
- At first item, pressing Button 16 goes to last
- Circular navigation through all defined zones/TRPs

### 8.9.4 Selector Exercise

**TRP Navigation Practice:**
```
☐ Define 5 TRPs in Zone Definitions menu
☐ Set motion mode to TRPScan (Button 11/13)
☐ Observe gimbal move to first TRP
☐ Press Button 14 to advance to TRP 2
☐ Press Button 14 three more times (TRP 3, 4, 5)
☐ Press Button 14 once more (wraps to TRP 1)
☐ Press Button 16 to go back to TRP 5
☐ Continue cycling to understand navigation
```

---

## 8.10 UNASSIGNED BUTTONS

The following buttons are currently **not mapped** and available for future functionality:

**Available Buttons:**
- Button 1
- Button 10
- Button 12
- Button 15
- Button 17
- Button 18
- Button 19

**Potential Future Assignments:**
- Weapon type selection (primary/secondary)
- Firing mode selection (single/burst/automatic)
- Stabilization toggle
- Emergency stop (all systems)
- Preset position recall
- Reticle type quick selection
- Night vision mode toggle

**Note:** Do not rely on these buttons for any current operations. Pressing them will have no effect.

---

## 8.11 SAFETY PROTOCOLS

### 8.11.1 Critical Safety Rules

**1. Dead Man Switch (Button 3) Required For:**
- Lead Angle Compensation activation/deactivation
- Tracking initiation (recommended best practice)
- Critical fire control operations

**2. Master Arm (Button 0) Required Before:**
- Weapon fire (Button 5)
- Ensures deliberate engagement intent

**3. Double-Click Button 4 For:**
- Emergency tracking abort from any phase
- No other safety interlocks required for abort

**4. Station Must Be Powered For:**
- Motion mode cycling (Button 11/13)
- Engagement commands (Button 0)
- Weapon fire (Button 5)

### 8.11.2 Accidental Activation Prevention

**Multi-Step Critical Operations:**
- Fire: Button 0 (Master Arm) + Button 5 (Fire)
- LAC Toggle: Button 3 (Dead Man) + Button 2 (LAC)
- Tracking: Button 3 (Dead Man) + Button 4 (Track)

**Momentary Switches for Weapons:**
- Button 0 and Button 5 must be held continuously
- Release = Immediate disarm/cease fire
- Prevents prolonged accidental engagement

**Context Restrictions:**
- Mode cycling blocked during acquisition
- Tracking stop required before mode change
- Dead Man Switch blocks LAC without active hold

---

## 8.12 OPERATIONAL CHECKLISTS

### 8.12.1 Complete Engagement Sequence

**Pre-Engagement:**
```
☐ Station powered on and initialized
☐ Cameras operational (day/thermal)
☐ Gimbal movement confirmed (stick test)
☐ Weapon system status: READY
☐ Operator hands positioned comfortably on controls
```

**Target Acquisition:**
```
☐ Manual mode active
☐ Use stick to slew gimbal to target area
☐ Zoom in for positive identification (Button 6)
☐ Confirm target is hostile/valid
☐ No friendlies in vicinity
```

**Tracking Initiation:**
```
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 4 (Enter Acquisition)
☐ Adjust tracking box with D-Pad
☐ Press Button 4 again (Request Lock)
☐ Wait for Active Lock confirmation
☐ Release Button 3 (Dead Man can be released during track)
```

**Fire Control Setup:**
```
☐ Laser range target (Control Panel LRF button)
☐ Verify range displayed correctly
☐ If target moving: Hold Button 3 + Press Button 2 (Enable LAC)
☐ Verify "LEAD ANGLE ON" (green)
☐ Monitor CCIP reticle position
☐ Check confidence bar: GREEN (>70%)
```

**Engagement:**
```
☐ Final safety check (positive ID, no friendlies)
☐ Hold Button 0 (Master Arm)
☐ Verify "MASTER ARM" indicator appears
☐ Align CCIP pipper on target
☐ Press Button 5 (Fire)
☐ Hold until desired rounds fired
☐ Release Button 5 (Cease Fire)
☐ Release Button 0 (Disarm)
```

**Post-Engagement:**
```
☐ Assess target (damage, status)
☐ If LAC active: Hold Button 3 + Press Button 2 (Disable LAC)
☐ Double-click Button 4 (Stop Tracking)
☐ Return to surveillance or home position
☐ Log engagement details
```

### 8.12.2 Emergency Abort Checklist

**Immediate Threat / Wrong Target:**
```
☐ Double-click Button 4 (Abort Tracking) - IMMEDIATE
☐ Release Button 5 (Cease Fire) if firing
☐ Release Button 0 (Disarm)
☐ Slew gimbal away from threat area
☐ Notify command of situation
```

---

## 8.13 TROUBLESHOOTING

### 8.13.1 Common Issues and Solutions

**Problem: Joystick not moving gimbal**

**Check:**
- Current motion mode - must be in Manual mode
- Station power - station must be enabled
- Joystick connection - verify USB connected

**Solution:**
```
1. Press Button 11/13 to cycle to Manual mode
2. Verify "MANUAL" mode indicator on display
3. Test stick with small movements
4. If still no movement, check Control Panel power switch
```

---

**Problem: Cannot start tracking (Button 4 no effect)**

**Check:**
- Dead Man Switch (Button 3) - must be held
- Current tracking phase - may already be in tracking
- Station power - must be enabled

**Solution:**
```
1. Hold Button 3 (Dead Man Switch) firmly
2. Press Button 4
3. If already tracking, double-click Button 4 to abort first
4. Then retry tracking initiation
```

---

**Problem: D-Pad not resizing acquisition box**

**Check:**
- Current tracking phase - must be in Acquisition phase

**Solution:**
```
1. Verify you pressed Button 4 once (entered Acquisition)
2. Confirm acquisition box visible on display
3. If in Lock Pending or Active Lock, D-Pad will not work
4. Abort tracking (double-click Button 4) and restart
```

---

**Problem: LAC won't enable (Button 2 no effect)**

**Check:**
- Dead Man Switch (Button 3) - must be held
- Current tracking state - should have active track

**Solution:**
```
1. Hold Button 3 (Dead Man Switch)
2. Keep holding and press Button 2
3. Verify "LEAD ANGLE ON" appears
4. If not, check system logs for error message
```

---

**Problem: Cannot cycle motion modes**

**Check:**
- Station power - must be enabled
- Tracking phase - cannot cycle during Acquisition
- Active tracking - will stop tracking first

**Solution:**
```
1. Verify station powered on (Control Panel)
2. If in Acquisition phase, complete or abort tracking first
3. Press Button 11 or Button 13
4. Observe mode indicator change
```

---

**Problem: Button 4 double-click too sensitive**

**Check:**
- Timing between presses

**Solution:**
```
1. Slow down between presses if accidentally aborting
2. Wait > 500ms between presses for single-press actions
3. For emergency abort, press rapidly (< 500ms)
4. Practice timing in training mode
```

---

**Problem: Zoom buttons not working**

**Check:**
- Camera power - cameras must be initialized
- Camera selected - day or thermal must be active

**Solution:**
```
1. Verify camera display shows video feed
2. Check Control Panel camera status indicators
3. Try switching cameras (Control Panel button)
4. If thermal camera selected, verify thermal initialized
```

---

## 8.14 ADVANCED TECHNIQUES

### 8.14.1 Rapid Target Engagement

**Technique: Speed-Optimized Engagement**

Used when time is critical and target positively identified.

```
☐ Pre-position thumbs on Button 3 and Button 4
☐ Target appears in FOV
☐ Slew with stick to center target
☐ Button 3 + Button 4 (Acquisition) - simultaneous press
☐ Quick D-Pad adjustment (2-3 presses max)
☐ Button 4 again (Lock Request) - immediate
☐ Button 0 (Master Arm) - while waiting for lock
☐ Button 5 (Fire) - as soon as Active Lock confirmed
☐ Release all buttons when complete
```

**Time Savings:** 2-4 seconds vs. methodical procedure

**Risk:** Less verification, requires excellent target ID skills

---

### 8.14.2 Multi-Target Engagement

**Technique: Sequential Target Prosecution**

```
☐ Engage first target (complete standard procedure)
☐ Do NOT release Button 0 (keep Master Arm hot)
☐ Double-click Button 4 (abort tracking of first target)
☐ Slew to second target immediately
☐ Button 3 + Button 4 (acquire second target)
☐ Quick box size (D-Pad)
☐ Button 4 (lock second target)
☐ Button 5 (fire second target)
☐ Repeat for additional targets
☐ Release Button 0 when all targets prosecuted
```

**Advantage:** Maintains weapon hot status, faster re-engagement

**Limitation:** Works for targets in same general area

---

### 8.14.3 One-Handed Emergency Operation

**Scenario:** One hand injured or needed for other controls

**Technique:**
- Prioritize stick control (gimbal aiming) with primary hand
- Use Control Panel buttons for camera, zoom, LRF
- Abort tracking (cannot operate Button 4 + D-Pad simultaneously)
- Switch to Manual mode and direct fire (no tracking)
- Call for assistance if sustained operation required

---

### 8.14.4 Night Engagement with Thermal

**Technique:**

```
☐ Switch to thermal camera (Control Panel)
☐ Cycle LUT palette (Button 7/9) for best target contrast
☐ Use Button 6 (Zoom In) conservatively (thermal has lower resolution)
☐ Acquire and track normally
☐ If LAC needed, verify sufficient FOV (thermal more restrictive)
☐ Engage as normal
```

**Thermal Considerations:**
- Lower resolution than day camera at distance
- Target signature varies by temperature differential
- LUT selection critical for target detection
- Avoid maximum zoom (pixelation and LAC limitations)

---

## 8.15 PROFICIENCY EXERCISES

### Exercise 1: Button Identification Drill (15 min)

**Objective:** Memorize all button locations without looking

**Procedure:**
1. Instructor calls out button number or function
2. Student touches correct button without looking at stick
3. Repeat until 100% accuracy achieved
4. Time goal: <2 seconds per button

**Buttons to Master:**
- Button 0 (Master Arm)
- Button 2 (LAC Toggle)
- Button 3 (Dead Man Switch)
- Button 4 (Track Select)
- Button 5 (Fire)
- Button 6/8 (Zoom In/Out)
- Button 7/9 (LUT Next/Prev)
- Button 11/13 (Mode Cycle)
- Button 14/16 (Up/Down Selector)
- D-Pad (all 8 directions)

---

### Exercise 2: Engagement Sequence Under Time Pressure (20 min)

**Objective:** Complete full engagement in <30 seconds

**Scenario:** Static target at 800m, clear conditions

**Procedure:**
1. Instructor starts timer when target designated
2. Student performs complete engagement sequence
3. Instructor stops timer at weapon fire
4. Repeat until <30 second standard achieved

**Target Times:**
- Expert: <20 seconds
- Proficient: 20-30 seconds
- Needs Practice: >30 seconds

---

### Exercise 3: Multi-Function Context Switching (20 min)

**Objective:** Master context-sensitive button behavior

**Procedure:**
1. Start in Manual mode, idle state
2. Press Button 14 (should call setUpSw)
3. Cycle to TRPScan mode
4. Press Button 14 (should select next TRP)
5. Initiate tracking
6. Press Button 14 (should call setUpTrack)
7. Abort tracking
8. Cycle to AutoSectorScan
9. Press Button 14 (should select next scan zone)
10. Repeat sequence until smooth

**Mastery Indicator:** No confusion about current button function based on system state

---

### Exercise 4: Emergency Abort Response (15 min)

**Objective:** Develop muscle memory for emergency abort

**Procedure:**
1. Begin tracking various targets (friendly, hostile, questionable)
2. Instructor randomly calls "ABORT!" during tracking
3. Student immediately double-clicks Button 4
4. Instructor confirms tracking stopped within 0.5 seconds
5. Repeat 20 times until reflexive

**Performance Standard:** <0.5 seconds from command to abort initiation

---

### Exercise 5: Blind Operation (30 min)

**Objective:** Operate controls by feel in low-light conditions

**Procedure:**
1. Dim or eliminate ambient lighting
2. Place hood over operator's head (cannot see controls)
3. Perform complete engagement sequence by touch only
4. Instructor verifies correct buttons pressed

**Skills Developed:**
- Tactile button identification
- Spatial awareness of stick layout
- Confidence in night operations
- Reduced dependency on visual confirmation

---

### Exercise 6: Joystick Coordination Test (25 min)

**Objective:** Simultaneous stick and button operation

**Procedure:**
1. Track moving target with stick (smooth circular motion)
2. While tracking, cycle LUT palette (Button 7/9)
3. While tracking, adjust zoom (Button 6/8)
4. While tracking, maintain tracking lock
5. Do not allow reticle to drift from target

**Mastery Indicator:** Target remains centered within ±5 pixels during button operations

---

## 8.16 PERFORMANCE STANDARDS

### 8.16.1 Skill Level Definitions

**NOVICE:**
- Can identify all buttons when prompted
- Performs engagement sequence with reference card
- Time to engage: >60 seconds
- Requires verbal guidance for multi-step procedures

**INTERMEDIATE:**
- Operates all buttons from memory
- Completes engagement sequence without reference
- Time to engage: 30-60 seconds
- Understands context-sensitive button behavior

**PROFICIENT:**
- Smooth, automatic button operation
- Completes engagement sequence: 20-30 seconds
- Operates correctly under stress
- Troubleshoots common problems independently

**EXPERT:**
- Reflexive control operation (muscle memory)
- Completes engagement sequence: <20 seconds
- Executes advanced techniques (multi-target, rapid engagement)
- Teaches others effectively

### 8.16.2 Evaluation Criteria

**Knowledge (25%):**
- Names and functions of all 20 buttons
- Context-sensitive behavior understanding
- Safety interlock requirements
- Troubleshooting procedures

**Skills (50%):**
- Smooth analog stick control
- Correct button sequences
- Emergency abort reflex
- Multi-function coordination

**Speed (15%):**
- Time to complete standard engagement
- Rapid mode switching
- Quick target re-acquisition

**Safety (10%):**
- Proper safety interlock usage
- Situational awareness
- Correct abort procedures
- Muzzle discipline (gimbal control)

---

## 8.17 LESSON SUMMARY

### Key Points

1. **20 Joystick Buttons** with context-sensitive functions based on system state

2. **Analog Stick Control** provides smooth, velocity-based gimbal movement in Manual mode

3. **D-Pad (Hat Switch)** resizes tracking acquisition box during Acquisition phase only

4. **Fire Control Sequence** requires multi-step safety interlocks (Button 0 + Button 5)

5. **Dead Man Switch (Button 3)** protects critical operations like LAC toggle and tracking

6. **Button 4 (Track Select)** has dual function: single press advances tracking phase, double-click aborts

7. **Camera Controls** (Buttons 6, 7, 8, 9) manage zoom and thermal LUT selection

8. **Motion Mode Cycling** (Buttons 11/13) with safety restrictions during tracking

9. **Context-Sensitive Selectors** (Buttons 14/16) change function based on operational and motion modes

10. **Emergency Procedures** emphasize double-click abort and immediate disarm capability

### Practical Application

Students should be able to:
- Operate all joystick controls without reference materials
- Execute complete engagement sequence in <30 seconds
- Perform emergency abort reflexively (<0.5 seconds)
- Troubleshoot common joystick-related issues
- Adapt button usage to current system state

### Next Lesson Preview

**Lesson 9: Lead Angle Compensation** will cover:
- LAC theory and ballistic calculations
- Enabling/disabling LAC with proper safety procedures
- LAC status indicators and warnings
- CCIP reticle interpretation with LAC active
- Moving target engagement techniques
- LAC limitations and troubleshooting

---

## APPENDIX A: QUICK REFERENCE CARD

### Button Map Summary

| Button | Primary Function | Safety Interlock | Context-Sensitive |
|--------|------------------|------------------|-------------------|
| **0** | Master Arm | Station ON | No |
| **2** | LAC Toggle | Dead Man (Btn 3) | No |
| **3** | Dead Man Switch | None | No |
| **4** | Track Select/Abort | Dead Man (recommended) | Yes (by phase) |
| **5** | Fire Weapon | Station ON | No |
| **6** | Zoom In | None | No |
| **7** | Next LUT | None | Yes (thermal only) |
| **8** | Zoom Out | None | No |
| **9** | Prev LUT | None | Yes (thermal only) |
| **11** | Cycle Mode | Station ON | No |
| **13** | Cycle Mode | Station ON | No |
| **14** | Up/Next Selector | None | Yes (by mode) |
| **16** | Down/Prev Selector | None | Yes (by mode) |
| **D-Pad** | Acquisition Box Resize | None | Yes (Acquisition phase) |
| **Stick X** | Azimuth Control | None | Yes (Manual mode) |
| **Stick Y** | Elevation Control | None | Yes (Manual mode) |

---

**END OF LESSON 8**
