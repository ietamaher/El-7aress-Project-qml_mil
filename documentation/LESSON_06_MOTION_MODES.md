# LESSON 6 - MOTION MODES & SURVEILLANCE PATTERNS

**Duration**: 3 hours
**Type**: Classroom + Practical
**References**: Operator manual, SystemStateModel documentation

---

## INTRODUCTION

This lesson covers the El 7arress RCWS motion modes, which control how the gimbal moves during surveillance and operations. You will learn to cycle between modes, operate automated surveillance patterns, utilize Target Reference Points (TRPs), and integrate radar cues if available.

---

## LEARNING OBJECTIVES

By the end of this lesson, you will be able to:
- Explain the purpose of each motion mode
- Cycle through motion modes safely
- Operate automatic sector scan mode
- Utilize TRP (Target Reference Point) scan mode
- Operate radar slew mode (if radar available)

---

## 6.1 MOTION MODES OVERVIEW

### **Motion Modes Available**

The RCWS has four primary motion modes:

1. **Manual Mode** - Direct operator control via joystick
2. **AutoSectorScan Mode** - Automated sector scanning patterns
3. **TRPScan Mode** - Sequential Target Reference Point scanning
4. **RadarSlew Mode** - Gimbal follows radar cues

### **Motion Mode Purpose**

**Manual Mode**:
- Operator has full control of gimbal via joystick
- Used for: Direct engagement, precise aiming, search operations
- Default mode on system startup

**AutoSectorScan Mode**:
- Automated gimbal movement through pre-defined sector patterns
- Used for: Perimeter surveillance, watch sectors, routine patrols
- Frees operator attention for monitoring multiple displays

**TRPScan Mode**:
- Sequential slewing to pre-defined Target Reference Points
- Used for: Checkpoint verification, known threat areas, periodic scans
- Efficient for covering multiple fixed locations

**RadarSlew Mode**:
- Gimbal automatically slews to radar-detected targets
- Used for: Rapid threat response, radar integration, automated cueing
- Requires radar system integration

---

## 6.2 MODE CYCLING CONTROLS

### **Cycling Between Modes**

**Controls**:
- **Button 11** OR **Button 13** on joystick (either button cycles modes)

**Cycle Sequence**:
```
Manual → AutoSectorScan → TRPScan → RadarSlew → Manual (loop)
```

**Procedure**:
1. Press Button 11 or Button 13
2. Mode advances to next in sequence
3. Display shows new mode: "Mode: MANUAL" / "Mode: SCAN" / "Mode: TRP" / "Mode: RADAR"
4. Repeat to continue cycling

**Current Mode Display**:
- HUD displays current mode (bottom of screen)
- Control Panel indicator (if equipped)

---

### **Mode Cycling Restrictions**

**Cannot Cycle Modes When**:
- **Tracking Acquisition Phase**: System blocks mode cycling (must exit acquisition first)
- **Active Tracking Lock**: System stops tracking before allowing mode change
- **Station Not Enabled**: Station Enable must be ON

**Warning Messages**:
- "CANNOT CHANGE MODE DURING ACQUISITION" - Exit acquisition first (double-click Button 4)
- "TRACKING STOPPED - MODE CHANGED" - Tracking was aborted to change mode
- "STATION NOT ENABLED" - Turn on Station Enable switch

### **Safety When Cycling Modes**

**Best Practices**:
- Cycle modes only when safe (no immediate threats)
- Announce mode changes to crew (if multi-operator)
- Verify gimbal behavior after mode change (ensure expected motion)
- Return to Manual mode if unexpected behavior

**If Tracking Active When Cycling**:
- System automatically stops tracking
- Gimbal motion transitions to new mode
- Operator must restart tracking manually if needed

---

## 6.3 MANUAL MODE

### **Manual Mode Overview**

**Description**: Operator has direct control of gimbal via joystick axes

**Default Mode**: System starts in Manual mode on power-up

**When to Use**:
- Direct engagement of targets
- Precise aiming requirements
- Search operations requiring operator judgment
- Response to threats requiring immediate action

### **Manual Mode Operation**

**Gimbal Control**:
- **X-axis (Left/Right)**: Joystick left/right controls azimuth
- **Y-axis (Up/Down)**: Joystick forward/back controls elevation
- **Velocity Control**: Stick deflection = gimbal speed
- **Stop**: Center stick = gimbal stops

**Review**: See Lesson 3, Section 3.4 for detailed joystick gimbal control

### **Manual Mode Display**

- **Status**: "Mode: MANUAL"
- **Gimbal Position**: Az: xxx.xx°, El: xx.xx°
- **No automated motion**: Gimbal only moves when operator commands

### **Transitioning from Manual to Other Modes**

**Procedure**:
1. Press Button 11 or 13
2. Mode changes to AutoSectorScan (or next in sequence)
3. Gimbal behavior changes immediately
4. Release joystick (automated modes ignore joystick axes)

**Note**: Always safe to return to Manual mode - press Button 11/13 until "Mode: MANUAL" displays

---

## 6.4 AUTO SECTOR SCAN MODE

### **Auto Sector Scan Overview**

**Purpose**: Automated surveillance of pre-defined azimuth/elevation sectors

**Use Cases**:
- Perimeter defense (scan 180° arc in front of vehicle)
- Watch sectors (scan specific threat axis)
- Routine patrols (free operator for other tasks)

**How It Works**:
1. Operator defines sector scan zones via menu (Lesson 7)
2. Each zone has:
   - Azimuth start and stop angles
   - Elevation angle
   - Scan rate (degrees per second)
3. System automatically slews gimbal back and forth across sector
4. Operator monitors video feed for targets

### **Auto Sector Scan Operation**

#### **Entering Auto Sector Scan Mode**

**Procedure**:
1. Press Button 11 or 13 to cycle to AutoSectorScan mode
2. Display shows "Mode: SCAN" or "Mode: AUTO SECTOR SCAN"
3. If scan zones defined, gimbal begins scanning first zone
4. If no scan zones defined, message displays: "NO SCAN ZONES DEFINED"

#### **Scan Behavior**

**Scan Pattern** (example):
```
Zone: "Front Arc 90°"
  Az Start: 315° (45° left of forward)
  Az Stop: 045° (45° right of forward)
  Elevation: 10° (slightly up)
  Rate: 5°/second

Gimbal motion:
  315° → (slowly traverse right) → 045° → (reverse) → 315° (repeat)
```

**Scan Motion**:
- Gimbal slews from start angle to stop angle at defined rate
- Reverses direction at limits
- Continuous back-and-forth motion
- Elevation held constant (or varies if zone defined with elevation sweep)

#### **Selecting Scan Zones**

**Multiple Zones**: If multiple sector scan zones are defined, select which zone to scan:

**Controls**:
- **Button 14 (UP)**: Select NEXT sector scan zone
- **Button 16 (DOWN)**: Select PREVIOUS sector scan zone

**Procedure**:
1. In AutoSectorScan mode, press Button 14 or 16
2. Zone changes (gimbal slews to new zone)
3. Display shows new zone name: "Scan: [Zone Name]"
4. Gimbal begins scanning new zone

**Example**:
```
Defined Zones:
  1. Front Arc 90°
  2. Left Side Arc 60°
  3. Right Side Arc 60°

Current: Zone 1 "Front Arc 90°"
Press Button 14 (UP/Next) → Zone 2 "Left Side Arc 60°"
Press Button 14 again → Zone 3 "Right Side Arc 60°"
Press Button 14 again → Zone 1 "Front Arc 90°" (wraps around)
```

#### **Auto Sector Scan Display**

**HUD Elements**:
- **Mode**: "Mode: SCAN" or "Mode: AUTO SECTOR SCAN"
- **Current Zone**: "Scan: Front Arc 90°" (zone name)
- **Scan Direction**: Arrow indicating current scan direction (← or →)
- **Gimbal Position**: Az: xxx.xx°, El: xx.xx° (continuously updating)

---

### **Defining Sector Scan Zones**

**Access**: Main Menu → Zone Definitions → Auto Sector Scan Zones (detailed in Lesson 7)

**Quick Overview**:
1. Enter menu system
2. Navigate to Zone Definitions → Auto Sector Scan Zones
3. Select "Add New Zone"
4. Define parameters:
   - Zone name (e.g., "Front Arc 90°")
   - Azimuth start angle (e.g., 315°)
   - Azimuth stop angle (e.g., 045°)
   - Elevation angle (e.g., 10°)
   - Scan rate (e.g., 5°/second)
5. Save zone
6. Zone now available for scanning

**Recommendation**: Define sector scan zones during mission planning, before operations.

---

### **Auto Sector Scan Best Practices**

**When to Use**:
- Low-threat environment (routine surveillance)
- Defined perimeter or sector of responsibility
- Operator needs to monitor multiple systems simultaneously
- Prolonged surveillance missions (reduces operator fatigue)

**Limitations**:
- Predictable pattern (enemy may time movements between scans)
- May miss rapidly-appearing threats (depends on scan rate)
- Operator must remain vigilant (monitor screen continuously)

**Tips**:
- Define multiple overlapping scan zones for comprehensive coverage
- Vary scan rates to reduce predictability
- Use in conjunction with radar (if available) for threat cueing
- Periodically switch to Manual mode for random scans

---

## 6.5 TRP SCAN MODE

### **TRP Scan Overview**

**TRP** = **Target Reference Point**: Pre-defined location of interest

**Purpose**: Sequential slewing to fixed points of interest for verification or engagement

**Use Cases**:
- Checkpoint verification (is checkpoint clear?)
- Known threat areas (check building where enemy previously seen)
- Navigation reference points
- Communication relay positions
- Periodic scans of critical infrastructure

**How It Works**:
1. Operator defines TRPs via menu (Lesson 7) - stores azimuth/elevation coordinates
2. Each TRP has a name (e.g., "Checkpoint Alpha", "Overwatch Hill")
3. In TRP Scan mode, operator cycles through TRPs
4. System slews gimbal to each TRP azimuth/elevation
5. Operator observes target area, then moves to next TRP

---

### **TRP Scan Operation**

#### **Entering TRP Scan Mode**

**Procedure**:
1. Press Button 11 or 13 to cycle to TRPScan mode
2. Display shows "Mode: TRP" or "Mode: TRP SCAN"
3. If TRPs defined, gimbal slews to first TRP
4. If no TRPs defined, message displays: "NO TRPs DEFINED"

#### **TRP Display**

**HUD Elements**:
- **Mode**: "Mode: TRP"
- **Current TRP**: "TRP: Checkpoint Alpha" (TRP name)
- **TRP Position**: Az: xxx.xx°, El: xx.xx° (target position)
- **TRP Count**: "TRP 1 of 5" (current TRP number / total TRPs)

**TRP List Overlay** (may be displayed):
```
┌────────────────────────────┐
│   TRP SCAN MODE            │
├────────────────────────────┤
│                            │
│  > 1. Checkpoint Alpha     │ ← Current TRP
│    2. Overwatch Hill       │
│    3. Bridge North         │
│    4. Compound Entrance    │
│    5. Rally Point Bravo    │
│                            │
│  Press ▲/▼ to change TRP   │
└────────────────────────────┘
```

---

#### **Navigating Between TRPs**

**Controls**:
- **Button 14 (UP)**: Select NEXT TRP
- **Button 16 (DOWN)**: Select PREVIOUS TRP

**Procedure**:
1. In TRP Scan mode, press Button 14 or 16
2. System slews gimbal to next/previous TRP
3. Display updates with new TRP name and position
4. Observe target area
5. Repeat to cycle through all TRPs

**Example**:
```
Current: TRP 1 "Checkpoint Alpha" (Az: 045°, El: 5°)
Press Button 14 (UP/Next) → TRP 2 "Overwatch Hill" (Az: 090°, El: 15°)
Press Button 14 again → TRP 3 "Bridge North" (Az: 010°, El: 2°)
Press Button 16 (DOWN/Previous) → TRP 2 "Overwatch Hill"
```

**Wrapping**:
- From last TRP, pressing Next (Button 14) → returns to first TRP
- From first TRP, pressing Previous (Button 16) → goes to last TRP

---

#### **TRP Scan Workflow**

**Typical Usage**:
1. Enter TRP Scan mode (cycle to TRP mode)
2. Gimbal slews to TRP 1
3. Operator observes area (look for threats, verify status)
4. If target/threat seen: Switch to Manual mode, engage or report
5. If area clear: Press Button 14 (Next TRP)
6. Gimbal slews to TRP 2
7. Repeat for all TRPs
8. Return to TRP 1 (wrap around) or switch modes

**Dwell Time**: Operator controls how long to observe each TRP (no automatic timer)

---

### **Defining TRPs**

**Access**: Main Menu → Zone Definitions → Target Reference Points (TRPs) (detailed in Lesson 7)

**Quick Overview**:
1. Enter menu system
2. Navigate to Zone Definitions → Target Reference Points (TRPs)
3. Select "Add New TRP"
4. Define parameters:
   - TRP name (e.g., "Checkpoint Alpha")
   - Azimuth (e.g., 045°) - can capture current gimbal position
   - Elevation (e.g., 5°) - can capture current gimbal position
5. Save TRP
6. TRP now available for TRP Scan mode

**Capturing Current Position as TRP**:
- Aim gimbal at desired location (Manual mode)
- Enter menu → Zone Definitions → TRPs → Add New TRP
- Select "Capture Current Position" (azimuth/elevation auto-filled)
- Enter TRP name
- Save

**Recommendation**: Define TRPs during mission planning or during reconnaissance phase.

---

### **TRP Scan Best Practices**

**When to Use**:
- Checkpoint control (periodic verification of checkpoints)
- Overwatch (scan known threat areas)
- Convoy escort (check waypoints ahead)
- Base defense (scan perimeter key points)

**Advantages**:
- Fast slewing to pre-defined locations (no manual aiming required)
- Ensures critical locations are checked
- Reduces operator workload
- Consistent coverage

**Limitations**:
- Only scans pre-defined points (may miss threats between TRPs)
- Requires pre-mission planning (TRPs must be defined)
- TRP coordinates may become outdated (if terrain changes)

**Tips**:
- Define 5-10 TRPs for typical mission (manageable number)
- Name TRPs clearly (use standard names familiar to all operators)
- Update TRP coordinates if mission area changes
- Use TRP Scan in combination with Manual mode (TRP scan for routine, Manual for response)

---

## 6.6 RADAR SLEW MODE

### **Radar Slew Overview**

**Purpose**: Automatically slew gimbal to radar-detected targets for visual identification and engagement

**Requires**: Radar system integrated with RCWS (radar provides target azimuth/elevation to fire control computer)

**Use Cases**:
- Radar-cued engagement (radar detects, camera identifies)
- Counter-UAV (detect small drones with radar, engage with weapon)
- Air defense (detect aircraft/helicopters)
- Ground vehicle detection (radar detects moving vehicles)

**How It Works**:
1. Radar system detects target (radar plot)
2. Radar provides target azimuth, elevation, range to RCWS
3. In Radar Slew mode, operator selects radar plot
4. Gimbal automatically slews to radar-provided coordinates
5. Operator visually confirms target on camera
6. Operator engages or continues scanning

---

### **Radar Slew Operation**

#### **Entering Radar Slew Mode**

**Procedure**:
1. Press Button 11 or 13 to cycle to RadarSlew mode
2. Display shows "Mode: RADAR" or "Mode: RADAR SLEW"
3. If radar plots available, gimbal slews to first plot
4. If no radar plots, message displays: "NO RADAR TARGETS" or "RADAR OFFLINE"

#### **Radar Plot Display**

**HUD Elements**:
- **Mode**: "Mode: RADAR"
- **Current Plot**: "Radar Track 1 of 3"
- **Plot Information**:
  - Azimuth: xxx°
  - Elevation: xx°
  - Range: xxxx m
  - Velocity: xx m/s (if provided by radar)
  - Track Age: x seconds (time since radar last updated this plot)

**Radar Plot List Overlay** (may be displayed):
```
┌────────────────────────────────────┐
│   RADAR SLEW MODE                  │
├────────────────────────────────────┤
│                                    │
│  > Track 1: Az 045° El 12° Rng 2500m │ ← Current
│    Track 2: Az 090° El 05° Rng 1800m │
│    Track 3: Az 120° El 08° Rng 3200m │
│                                    │
│  Press ▲/▼ to change track         │
└────────────────────────────────────┘
```

---

#### **Selecting Radar Tracks**

**Controls**:
- **Button 14 (UP)**: Select NEXT radar track
- **Button 16 (DOWN)**: Select PREVIOUS radar track

**Procedure**:
1. In Radar Slew mode, press Button 14 or 16
2. System slews gimbal to next/previous radar track
3. Display updates with new track information
4. Observe camera feed for visual confirmation
5. Repeat to cycle through all radar tracks

**Example**:
```
Current: Track 1 (Az: 045°, El: 12°, Rng: 2500m)
  → Gimbal points at Az 045°, El 12°
  → Operator looks at camera feed - sees small aircraft

Press Button 14 (UP/Next) → Track 2 (Az: 090°, El: 5°, Rng: 1800m)
  → Gimbal slews to Az 090°, El 5°
  → Operator looks at camera feed - sees ground vehicle
```

---

#### **Radar Slew Workflow**

**Typical Usage**:
1. Radar detects target (creates radar plot)
2. Operator enters Radar Slew mode
3. Gimbal slews to first radar plot
4. Operator visually confirms target on camera:
   - **Target confirmed as threat**: Switch to Manual or Tracking mode, engage
   - **Target confirmed friendly**: Move to next plot (Button 14)
   - **No visual on target**: May be false alarm, clutter, or target not visible - move to next plot
5. Repeat for all radar plots
6. When all plots checked, continue monitoring or switch modes

---

### **Radar Integration Details**

**Radar Plot Information**:
- **Azimuth**: Target direction (horizontal angle)
- **Elevation**: Target altitude angle (if radar provides)
- **Range**: Target distance (meters)
- **Velocity**: Target speed (m/s) - may indicate if approaching or receding
- **Track ID**: Unique identifier for this radar track
- **Track Age**: Time since radar last updated (stale tracks may be unreliable)

**Radar Plot Updates**:
- Radar continuously updates plot positions
- RCWS receives updates in real-time
- Gimbal tracks updated position if following active plot
- Old plots "time out" and disappear if radar loses target

**Radar Types Supported** (platform-specific):
- Ground surveillance radar
- Counter-UAV radar
- Short-range air defense radar
- Vehicle tracking radar

---

### **Radar Slew Best Practices**

**When to Use**:
- Radar-equipped platforms
- Counter-UAV missions
- Air defense
- Perimeter surveillance (radar detects, camera confirms)

**Advantages**:
- Rapid target cueing (radar detects before visual)
- 360° coverage (radar scans all directions)
- Detect small or distant targets (beyond visual range)
- Detect targets in low visibility (radar works in fog, dust, darkness)

**Limitations**:
- Radar false alarms (birds, clutter, etc.)
- Radar may not provide precise elevation (gimbal may not point exactly at target)
- Visual confirmation still required (radar can't identify target type)
- Requires radar integration (not all platforms have radar)

**Tips**:
- Always visually confirm target before engagement (radar detects, camera identifies)
- Use zoom to identify distant radar targets
- Prioritize closest or fastest-moving tracks (most threatening)
- If no visual confirmation after 5 seconds, move to next track (don't waste time)

---

## 6.7 MODE SELECTION BEST PRACTICES

### **Choosing the Right Mode**

| Situation | Recommended Mode | Reason |
|-----------|------------------|--------|
| **Direct engagement** | Manual | Precise control, immediate response |
| **Target tracking** | Manual (then Tracking) | Start in Manual, initiate tracking when target acquired |
| **Perimeter surveillance** | AutoSectorScan | Automated coverage, reduces operator workload |
| **Checkpoint control** | TRPScan | Efficient coverage of known locations |
| **Radar cueing** | RadarSlew | Rapid response to radar detections |
| **Search operations** | Manual | Requires operator judgment and flexibility |
| **Prolonged surveillance** | AutoSectorScan or TRPScan | Reduces operator fatigue |

---

### **Mode Transition Safety**

**Before Changing Modes**:
1. Verify no immediate threats
2. If tracking active, decide: Abort tracking or stay in Manual
3. Announce mode change (if multi-operator crew)
4. Monitor gimbal behavior after mode change

**After Changing Modes**:
1. Verify gimbal moving as expected
2. Check mode displayed correctly on HUD
3. If unexpected behavior, return to Manual mode (cycle back to Manual)
4. If gimbal motion unsafe, press Emergency Stop

---

### **Combining Modes**

**Example Mission Workflow**:
```
0600: Startup → Manual mode (system default)
0610: Begin perimeter surveillance → AutoSectorScan mode (Front Arc zone)
0645: Radar detects contact → RadarSlew mode (slew to radar track)
0646: Visual confirmation: friendly vehicle → Return to AutoSectorScan
0720: Checkpoint time → TRPScan mode (verify checkpoints)
0730: Return to perimeter surveillance → AutoSectorScan mode
0800: Contact reported at grid 12345 → Manual mode (slew to location)
0805: Target engaged → Manual mode (tracking initiated)
0810: Target neutralized → Return to AutoSectorScan mode
```

**Key Point**: Modes are tools - use the right tool for the task. Operators should be proficient in all modes and transition smoothly between them.

---

## 6.8 MOTION MODE EXERCISES

### **Exercise 6.8.1: Mode Cycling Practice**

**Objective**: Cycle through all modes and identify current mode

**Procedure**:
1. Start in Manual mode
2. Press Button 11 (or 13)
3. Identify new mode (read HUD display)
4. Announce mode verbally
5. Repeat until cycled through all 4 modes and returned to Manual
6. Instructor verifies correct mode identification

**Proficiency**: Complete cycle in <10 seconds, identify each mode correctly

---

### **Exercise 6.8.2: Auto Sector Scan Operation**

**Objective**: Operate auto sector scan mode and change scan zones

**Setup**: Instructor provides platform with pre-defined sector scan zones (minimum 2 zones)

**Procedure**:
1. Cycle to AutoSectorScan mode (Button 11/13)
2. Verify gimbal scanning (moving back and forth)
3. Identify current scan zone (read HUD display)
4. Press Button 14 (Next Zone)
5. Verify gimbal transitions to new zone
6. Identify new scan zone
7. Press Button 16 (Previous Zone)
8. Verify gimbal returns to original zone
9. Return to Manual mode

**Evaluation**:
- Correct mode entry
- Identification of scan zones
- Successful zone changes
- Smooth return to Manual mode

---

### **Exercise 6.8.3: TRP Scan Operation**

**Objective**: Navigate through TRPs and identify locations

**Setup**: Instructor provides platform with pre-defined TRPs (minimum 3 TRPs)

**Procedure**:
1. Cycle to TRPScan mode (Button 11/13)
2. Verify gimbal slews to first TRP
3. Identify TRP name and position (read HUD display)
4. Press Button 14 (Next TRP)
5. Verify gimbal slews to second TRP
6. Identify TRP name and position
7. Continue through all TRPs (press Button 14 repeatedly)
8. Wrap around to first TRP
9. Press Button 16 (Previous TRP) - verify reverse navigation
10. Return to Manual mode

**Evaluation**:
- Correct mode entry
- Identification of all TRPs
- Successful TRP navigation (forward and backward)
- Smooth return to Manual mode

---

### **Exercise 6.8.4: Radar Slew Operation (If Available)**

**Objective**: Select and slew to radar tracks

**Setup**: Instructor provides radar tracks (simulated or live)

**Procedure**:
1. Cycle to RadarSlew mode (Button 11/13)
2. Verify gimbal slews to first radar track
3. Identify track information (Az, El, Range)
4. Visually confirm target on camera (if visible)
5. Press Button 14 (Next Track)
6. Verify gimbal slews to second track
7. Continue through all radar tracks
8. Return to Manual mode

**Evaluation**:
- Correct mode entry
- Identification of radar track parameters
- Successful track navigation
- Visual target confirmation attempts
- Smooth return to Manual mode

---

### **Exercise 6.8.5: Mission Simulation**

**Objective**: Use multiple modes in simulated mission scenario

**Scenario**: "You are conducting perimeter surveillance. At 0800, command reports potential threat at Checkpoint Alpha. At 0805, radar detects airborne contact."

**Procedure**:
1. Start: AutoSectorScan mode (perimeter surveillance)
2. At 0800 (instructor cue): Switch to TRPScan mode
3. Navigate to "Checkpoint Alpha" TRP
4. Report observations
5. At 0805 (instructor cue): Switch to RadarSlew mode (if available) OR Manual mode
6. Slew to radar contact or instructor-provided location
7. Report observations
8. Return to AutoSectorScan mode (resume surveillance)

**Evaluation**:
- Appropriate mode selection for each task
- Smooth mode transitions
- Correct use of controls (Button 11/13, Button 14/16)
- Mission task completion

---

## LESSON 6 SUMMARY

**Key Points**:
1. Four motion modes: Manual, AutoSectorScan, TRPScan, RadarSlew
2. Cycle modes with Button 11 or Button 13
3. Cannot cycle during Tracking Acquisition phase
4. Manual mode: Direct joystick control (default mode)
5. AutoSectorScan mode: Automated sector scanning (Button 14/16 to change zones)
6. TRPScan mode: Sequential TRP navigation (Button 14/16 to change TRPs)
7. RadarSlew mode: Slew to radar tracks (Button 14/16 to change tracks)
8. Button 14 (UP): Next zone/TRP/track (context-sensitive)
9. Button 16 (DOWN): Previous zone/TRP/track (context-sensitive)
10. Use appropriate mode for mission task (Manual for engagement, AutoSectorScan for surveillance, etc.)

**Skills Practiced**:
- Mode cycling (Button 11/13)
- Mode identification (reading HUD display)
- Auto Sector Scan operation
- TRP Scan navigation
- Radar Slew operation (if available)
- Context-sensitive button usage (Button 14/16)
- Mission-appropriate mode selection

**Next Lesson**: Zone Definition & Management (Defining no-fire zones, no-traverse zones, sector scan zones, and TRPs)

---

**IMPORTANT REMINDERS**:
- Always safe to return to Manual mode - cycle back if unsure
- Cannot cycle modes during tracking acquisition (exit acquisition first)
- Automated modes free operator attention - remain vigilant
- Zones and TRPs must be defined before using AutoSectorScan or TRPScan modes (Lesson 7)
- Radar Slew requires radar integration (not available on all platforms)

---
