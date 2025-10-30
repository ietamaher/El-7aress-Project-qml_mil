# LESSON 7 - ZONE DEFINITION & MANAGEMENT

**Duration**: 4 hours
**Type**: Classroom + Practical
**References**: Operator manual, zone configuration guide, SystemStateModel documentation

---

## INTRODUCTION

This lesson teaches zone definition and management for the El 7arress RCWS. Zones provide safety restrictions, surveillance automation, and mission planning capabilities. You will learn to define no-fire zones, no-traverse zones, sector scan zones, Target Reference Points (TRPs), and manage zone configuration files.

---

## LEARNING OBJECTIVES

By the end of this lesson, you will be able to:
- Define no-fire zones for safety
- Define no-traverse zones for gimbal restrictions
- Create and manage sector scan zones for automated surveillance
- Add and modify Target Reference Points (TRPs)
- Save and load zone configuration files

---

## 7.1 ZONE TYPES OVERVIEW

### **Four Zone Types**

The RCWS supports four types of zones:

1. **No-Fire Zones** - Safety restriction: weapon cannot fire into these areas
2. **No-Traverse Zones** - Gimbal restriction: gimbal cannot enter these areas
3. **Auto Sector Scan Zones** - Surveillance: automated sector scanning patterns
4. **Target Reference Points (TRPs)** - Navigation: pre-defined aim points

### **Zone Type Summary**

| Zone Type | Purpose | Effect | Example Use |
|-----------|---------|--------|-------------|
| **No-Fire Zone** | Safety | Weapon fire blocked or warning issued | Friendly positions, civilian areas |
| **No-Traverse Zone** | Safety | Gimbal motion blocked | Vehicle superstructure, antennas |
| **Sector Scan Zone** | Automation | Gimbal scans sector automatically | Perimeter surveillance arcs |
| **TRP** | Navigation | Gimbal slews to pre-defined point | Checkpoints, known threat areas |

---

## 7.2 ACCESSING ZONE DEFINITIONS MENU

### **Menu Access**

**Path**: Main Menu → "Zone Definitions"

**Procedure**:
1. Press MENU/VAL to open main menu
2. Navigate to "Zone Definitions" (in SYSTEM section)
3. Press MENU/VAL to enter Zone Definitions submenu

### **Zone Definitions Submenu**

```
┌────────────────────────────────────┐
│    ZONE DEFINITIONS                │
├────────────────────────────────────┤
│                                    │
│  > No-Fire Zones                   │
│    No-Traverse Zones               │
│    Auto Sector Scan Zones          │
│    Target Reference Points (TRPs)  │
│    Save Configuration              │
│    Load Configuration              │
│    Return to Main Menu             │
│                                    │
└────────────────────────────────────┘
```

**Navigation**:
- UP/DOWN to select zone type or function
- MENU/VAL to enter
- Each zone type has its own management submenu

---

## 7.3 NO-FIRE ZONES

### **No-Fire Zone Purpose**

**Definition**: Geographic areas where weapon discharge is prohibited

**Purpose**: Prevent accidental firing into friendly or civilian areas

**Examples**:
- Friendly troop positions
- Civilian structures
- Protected infrastructure (hospitals, schools, religious sites)
- Command post locations
- Ammunition depots
- Safe havens

### **No-Fire Zone Behavior**

**When Gimbal Aims Into No-Fire Zone**:
1. Warning displayed on HUD: "NO-FIRE ZONE" or "ZONE VIOLATION"
2. Indicator light flashes red (Control Panel)
3. Weapon fire may be blocked (platform-specific configuration):
   - **Hard Block**: Fire button (Button 5) has no effect
   - **Soft Warning**: Fire button works but operator warned

**Operator Responsibility**:
- Always verify not in no-fire zone before engaging
- If warning appears, immediately move gimbal away from restricted area
- Do NOT fire if zone violation warning active

---

### **No-Fire Zone Parameters**

Each no-fire zone is defined by:

| Parameter | Description | Units |
|-----------|-------------|-------|
| **Zone ID** | Unique identifier (auto-assigned) | Number (1, 2, 3...) |
| **Zone Name** | Descriptive name | Text (e.g., "Friendly HQ") |
| **Azimuth Start** | Left boundary | Degrees (0-359°) |
| **Azimuth Stop** | Right boundary | Degrees (0-359°) |
| **Elevation Min** | Bottom boundary | Degrees (-20 to +60°) |
| **Elevation Max** | Top boundary | Degrees (-20 to +60°) |
| **Range Min** | Near distance (optional) | Meters (0-9999) |
| **Range Max** | Far distance (optional) | Meters (0-9999) |
| **Active** | Zone enabled/disabled | Boolean (ON/OFF) |

**Zone Shape**: Zones are defined as azimuth/elevation "boxes" (angular sectors)

**Range Limits (Optional)**:
- If range limits defined, zone only applies within that range band
- Example: No-fire zone from 0-500m (close-in area), but OK to fire beyond 500m
- If range limits NOT defined, zone applies at all ranges

---

### **Defining a No-Fire Zone**

#### **Procedure**

**Step 1: Access No-Fire Zones Menu**
1. Main Menu → Zone Definitions → "No-Fire Zones"
2. Current zones list displayed (if any)

**No-Fire Zones Submenu**:
```
┌────────────────────────────────────┐
│    NO-FIRE ZONES                   │
├────────────────────────────────────┤
│  Existing Zones:                   │
│    1. Friendly HQ (Active)         │
│    2. Civilian Area (Active)       │
│                                    │
│  > Add New Zone                    │
│    Modify Zone                     │
│    Delete Zone                     │
│    Return to Zone Definitions      │
└────────────────────────────────────┘
```

**Step 2: Add New Zone**
1. Navigate to "Add New Zone"
2. Press MENU/VAL

**Step 3: Enter Zone Name**
1. System prompts: "Enter Zone Name"
2. Use UP/DOWN to select letters (A-Z, 0-9, space)
3. Press MENU/VAL to advance to next character
4. When name complete, navigate to "Done" or "OK"
5. Press MENU/VAL to confirm

**Example Name**: "Friendly HQ"

**Step 4: Define Azimuth Start**
1. System prompts: "Azimuth Start: ___°"
2. Options:
   - **Enter Manually**: Use UP/DOWN to adjust value (0-359°)
   - **Capture Current Position**: Aim gimbal at left edge of zone, select "Capture Current Az"
3. Press MENU/VAL to confirm

**Example**: Az Start = 315° (45° left of forward)

**Step 5: Define Azimuth Stop**
1. System prompts: "Azimuth Stop: ___°"
2. Use UP/DOWN to adjust value (0-359°)
3. OR aim gimbal at right edge of zone, select "Capture Current Az"
4. Press MENU/VAL to confirm

**Example**: Az Stop = 045° (45° right of forward)
**Result**: Zone covers 315° to 045° (90° arc centered on forward)

**Step 6: Define Elevation Min**
1. System prompts: "Elevation Min: ___°"
2. Use UP/DOWN to adjust value (-20 to +60°)
3. OR capture current elevation
4. Press MENU/VAL to confirm

**Example**: El Min = -5° (slightly below horizon)

**Step 7: Define Elevation Max**
1. System prompts: "Elevation Max: ___°"
2. Use UP/DOWN to adjust value (-20 to +60°)
3. Press MENU/VAL to confirm

**Example**: El Max = +30° (30° above horizon)
**Result**: Zone covers -5° to +30° elevation (35° vertical span)

**Step 8: Define Range Limits (Optional)**
1. System prompts: "Define Range Limits? Yes / No"
2. If No: Zone applies at all ranges (skip to Step 9)
3. If Yes:
   - Enter Range Min (meters): e.g., 0 m
   - Enter Range Max (meters): e.g., 1000 m
   - Zone only active from 0-1000m

**Step 9: Set Active Status**
1. System prompts: "Activate Zone? Yes / No"
2. Select Yes to enable immediately, No to save but keep disabled
3. Press MENU/VAL to confirm

**Step 10: Save Zone**
1. System displays zone summary:
   ```
   NO-FIRE ZONE SUMMARY

   Name: Friendly HQ
   Azimuth: 315° to 045°
   Elevation: -5° to +30°
   Range: All ranges
   Active: YES

   Save this zone? Yes / No
   ```
2. Navigate to "Yes"
3. Press MENU/VAL
4. Zone saved and assigned Zone ID (e.g., Zone 3)
5. Message: "Zone Saved: ID 3"

**Step 11: Return to Menu**
1. Navigate to "Return to No-Fire Zones Menu"
2. Press MENU/VAL
3. New zone now appears in zone list

---

### **Modifying a No-Fire Zone**

**Procedure**:
1. No-Fire Zones Menu → "Modify Zone"
2. Select zone to modify from list (UP/DOWN, MENU/VAL)
3. System displays current zone parameters
4. Navigate through parameters (similar to Add New Zone)
5. Change desired parameters
6. Save changes

**Example**: Change "Friendly HQ" zone azimuth stop from 045° to 060°

---

### **Deleting a No-Fire Zone**

**Procedure**:
1. No-Fire Zones Menu → "Delete Zone"
2. Select zone to delete from list
3. System prompts: "Delete Zone 'Friendly HQ'? Yes / No"
4. Navigate to "Yes"
5. Press MENU/VAL
6. Zone deleted

**Warning**: Deleted zones cannot be recovered unless configuration file backup exists.

---

### **No-Fire Zone Best Practices**

**When to Define**:
- Before operations (mission planning phase)
- When friendly positions established
- When civilians in area
- Before live fire exercises

**Tips**:
- Define zones generously (larger than minimum needed - safety margin)
- Name zones clearly ("Friendly HQ" not "Zone 1")
- Verify zones after defining (aim gimbal into zone, verify warning appears)
- Update zones if friendly positions change
- Save configuration after defining zones

**Common Mistakes**:
- ❌ Zones too small (don't provide adequate safety margin)
- ❌ Forgetting to activate zone (Active = No)
- ❌ Swapping azimuth start/stop (zone covers wrong area)
- ❌ Not verifying zones after definition

---

## 7.4 NO-TRAVERSE ZONES

### **No-Traverse Zone Purpose**

**Definition**: Angular sectors where gimbal motion is prohibited

**Purpose**: Prevent gimbal collision with platform structures or equipment

**Examples**:
- Vehicle/ship superstructure (gimbal would hit antenna mast)
- Antenna arrays (gimbal pointing at radio antennas)
- Personnel areas (gimbal pointing at crew hatches)
- Equipment that could be damaged (other sensors, lights)

### **No-Traverse Zone Behavior**

**When Gimbal Approaches No-Traverse Zone**:
1. Gimbal motion stops at zone boundary (hard stop)
2. Warning displayed: "NO-TRAVERSE ZONE"
3. Joystick commands away from zone still work (can exit zone boundary)
4. Joystick commands into zone ignored (cannot enter zone)

**Effect**:
- Gimbal physically prevented from entering zone
- No software override (safety feature)
- Operator cannot force gimbal into zone

**Operator Responsibility**:
- Understand platform-specific no-traverse zones
- Do not attempt to override (will not work and may damage equipment)
- If gimbal stops unexpectedly, check for no-traverse zone boundary

---

### **No-Traverse Zone Parameters**

Similar to no-fire zones, but simpler (no range limits):

| Parameter | Description | Units |
|-----------|-------------|-------|
| **Zone ID** | Unique identifier (auto-assigned) | Number |
| **Zone Name** | Descriptive name | Text (e.g., "Antenna Mast") |
| **Azimuth Start** | Left boundary | Degrees (0-359°) |
| **Azimuth Stop** | Right boundary | Degrees (0-359°) |
| **Elevation Min** | Bottom boundary | Degrees (-20 to +60°) |
| **Elevation Max** | Top boundary | Degrees (-20 to +60°) |
| **Active** | Zone enabled/disabled | Boolean (ON/OFF) |

**Note**: No range parameters (no-traverse zones apply at all ranges)

---

### **Defining a No-Traverse Zone**

**Procedure**: Nearly identical to no-fire zone definition (Section 7.3)

**Steps**:
1. Main Menu → Zone Definitions → "No-Traverse Zones"
2. Select "Add New Zone"
3. Enter zone name (e.g., "Antenna Mast")
4. Define azimuth start (e.g., 170°)
5. Define azimuth stop (e.g., 190°)
6. Define elevation min (e.g., 0°)
7. Define elevation max (e.g., 60°)
8. Set active (Yes)
9. Save zone

**Result**: Gimbal cannot enter 170°-190° azimuth, 0°-60° elevation sector (antenna mast protected)

---

### **No-Traverse Zone Best Practices**

**When to Define**:
- During system installation (platform-specific zones)
- Before first operation on new platform
- After platform modifications (new equipment installed)

**Platform-Specific Zones**:
- Vehicle-mounted: Typically rear arc (180° ±30°) to avoid vehicle body
- Ship-mounted: Multiple zones (superstructure, antennas, other equipment)

**Tips**:
- Define zones during platform integration (one-time setup)
- Verify zones by attempting gimbal motion into zones (should stop)
- Document zones in platform-specific manual
- No-traverse zones rarely change (unlike no-fire zones which are mission-specific)

---

## 7.5 AUTO SECTOR SCAN ZONES

### **Sector Scan Zone Purpose**

**Definition**: Pre-defined azimuth/elevation sectors for automated surveillance scanning

**Purpose**: Enable AutoSectorScan motion mode (Lesson 6)

**Examples**:
- Front Arc 90° (315° to 045°, elevation 10°)
- Left Side Arc (270° to 315°, elevation 5°)
- Right Side Arc (045° to 090°, elevation 5°)
- Perimeter 180° (270° to 090°, elevation 0°)

**Use**: See Lesson 6, Section 6.4 for Auto Sector Scan mode operation

---

### **Sector Scan Zone Parameters**

| Parameter | Description | Units |
|-----------|-------------|-------|
| **Zone ID** | Unique identifier (auto-assigned) | Number |
| **Zone Name** | Descriptive name | Text (e.g., "Front Arc 90°") |
| **Azimuth Start** | Left edge of scan | Degrees (0-359°) |
| **Azimuth Stop** | Right edge of scan | Degrees (0-359°) |
| **Elevation** | Vertical angle (constant during scan) | Degrees (-20 to +60°) |
| **Scan Rate** | Speed of gimbal movement | Degrees/second (0.1-60) |
| **Active** | Zone enabled/disabled | Boolean (ON/OFF) |

**Scan Behavior**:
- Gimbal slews from Azimuth Start to Azimuth Stop at Scan Rate
- Elevation held constant
- At stop point, reverses direction (Stop → Start)
- Continuous back-and-forth motion

**Scan Rate Selection**:
- **Slow (1-5 °/s)**: Detailed observation, target detection
- **Medium (5-15 °/s)**: Balanced coverage and detail
- **Fast (15-60 °/s)**: Rapid coverage, less detail

---

### **Defining a Sector Scan Zone**

**Procedure**:

**Step 1: Access Sector Scan Zones Menu**
1. Main Menu → Zone Definitions → "Auto Sector Scan Zones"
2. Existing zones list displayed

**Step 2: Add New Zone**
1. Navigate to "Add New Zone"
2. Press MENU/VAL

**Step 3: Enter Zone Name**
1. Enter descriptive name (e.g., "Front Arc 90°")
2. Confirm

**Step 4: Define Azimuth Start**
1. Enter azimuth start value (e.g., 315°)
2. OR capture current gimbal azimuth
3. Confirm

**Step 5: Define Azimuth Stop**
1. Enter azimuth stop value (e.g., 045°)
2. OR capture current gimbal azimuth
3. Confirm

**Example**: Az 315° to 045° = 90° arc (45° left to 45° right of forward)

**Step 6: Define Elevation**
1. Enter elevation value (e.g., 10°)
2. OR capture current gimbal elevation
3. Confirm

**Note**: Elevation is constant (does not sweep) during scan

**Step 7: Define Scan Rate**
1. Enter scan rate (e.g., 5 °/s)
2. Use UP/DOWN to adjust
3. Confirm

**Calculation**: 90° arc at 5°/s = 18 seconds to traverse from start to stop

**Step 8: Set Active**
1. Activate zone? Yes/No
2. Select Yes
3. Confirm

**Step 9: Save Zone**
1. Review zone summary
2. Save

**Result**: Sector scan zone now available in AutoSectorScan mode

---

### **Sector Scan Zone Best Practices**

**Planning**:
- Define 2-4 scan zones (covers different sectors)
- Overlap zones slightly (ensures no gaps in coverage)
- Name zones by coverage (e.g., "Front 90°", "Left 60°", "Right 60°")

**Scan Rate Selection**:
- Slow for high-threat areas (more time to detect targets)
- Fast for low-threat areas (broader coverage)
- Test scan rate (watch in AutoSectorScan mode, verify comfortable observation speed)

**Common Zones**:
- **Perimeter 180°**: Az 270° to 090° (left side to right side)
- **Front 90°**: Az 315° to 045° (45° left to 45° right)
- **Left 60°**: Az 270° to 330° (directly left to 30° left)
- **Right 60°**: Az 030° to 090° (30° right to directly right)

---

## 7.6 TARGET REFERENCE POINTS (TRPs)

### **TRP Purpose**

**Definition**: Pre-defined aim points (azimuth/elevation coordinates) for quick gimbal slewing

**Purpose**: Enable TRPScan motion mode (Lesson 6), mark locations of interest

**Examples**:
- Checkpoint Alpha (Az: 045°, El: 5°)
- Overwatch Hill (Az: 090°, El: 15°)
- Bridge North (Az: 010°, El: 2°)
- Compound Entrance (Az: 120°, El: 8°)
- Rally Point Bravo (Az: 180°, El: 0°)

**Use**: See Lesson 6, Section 6.5 for TRP Scan mode operation

---

### **TRP Parameters**

| Parameter | Description | Units |
|-----------|-------------|-------|
| **TRP ID** | Unique identifier (auto-assigned) | Number |
| **TRP Name** | Descriptive name | Text (e.g., "Checkpoint Alpha") |
| **Azimuth** | Aim point azimuth | Degrees (0-359°) |
| **Elevation** | Aim point elevation | Degrees (-20 to +60°) |
| **Description** | Optional notes | Text (e.g., "Main gate entrance") |
| **Active** | TRP enabled/disabled | Boolean (ON/OFF) |

**Simple Structure**: TRPs are just named azimuth/elevation coordinates (no area/sector like zones)

---

### **Defining a TRP**

**Procedure**:

**Step 1: Access TRPs Menu**
1. Main Menu → Zone Definitions → "Target Reference Points (TRPs)"
2. Existing TRPs list displayed

**Step 2: Add New TRP**
1. Navigate to "Add New TRP"
2. Press MENU/VAL

**Step 3: Enter TRP Name**
1. Enter descriptive name (e.g., "Checkpoint Alpha")
2. Confirm

**Step 4: Define Azimuth**
1. **Method A (Manual Entry)**:
   - Enter azimuth value (e.g., 045°)
   - Use UP/DOWN to adjust
   - Confirm
2. **Method B (Capture Current Position)** - RECOMMENDED:
   - First, aim gimbal at desired location (use joystick in Manual mode)
   - Select "Capture Current Position"
   - System auto-fills current gimbal azimuth
   - Confirm

**Step 5: Define Elevation**
1. **Method A (Manual Entry)**:
   - Enter elevation value (e.g., 5°)
   - Confirm
2. **Method B (Capture Current Position)** - RECOMMENDED:
   - System auto-fills current gimbal elevation (if captured in Step 4)
   - Confirm

**Step 6: Enter Description (Optional)**
1. System prompts: "Enter description (optional)"
2. Enter text (e.g., "Main gate entrance") or skip
3. Confirm

**Step 7: Set Active**
1. Activate TRP? Yes/No
2. Select Yes
3. Confirm

**Step 8: Save TRP**
1. Review TRP summary:
   ```
   TRP SUMMARY

   Name: Checkpoint Alpha
   Position: Az 045°, El 5°
   Description: Main gate entrance
   Active: YES

   Save this TRP? Yes / No
   ```
2. Navigate to "Yes"
3. Press MENU/VAL
4. TRP saved

**Result**: TRP now available in TRPScan mode

---

### **Recommended Method: Capture Current Position**

**Most Accurate and Easiest**:

1. Exit menu system (return to operational display)
2. Switch to Manual mode
3. Use joystick to aim gimbal at desired location (e.g., visually center checkpoint entrance)
4. Note current position on HUD (Az: 045.32°, El: 5.18°)
5. Enter menu: Main Menu → Zone Definitions → TRPs → Add New TRP
6. Enter TRP name
7. Select "Capture Current Position"
8. System captures current gimbal position (Az: 045.32°, El: 5.18°)
9. Optionally add description
10. Save TRP

**Advantage**: No manual coordinate entry, exact visual aiming

---

### **Modifying a TRP**

**Procedure**:
1. TRPs Menu → "Modify TRP"
2. Select TRP from list
3. Change name, azimuth, elevation, or description
4. Save changes

**Use Case**: TRP coordinate drifted (terrain changed, initial aim was imprecise)

---

### **Deleting a TRP**

**Procedure**:
1. TRPs Menu → "Delete TRP"
2. Select TRP from list
3. Confirm deletion
4. TRP deleted

---

### **TRP Best Practices**

**Planning**:
- Define 5-15 TRPs (manageable number for TRP Scan mode)
- Name TRPs clearly (use standard naming familiar to all operators)
- Use "Capture Current Position" method (most accurate)
- Document TRPs in mission plan (list TRP names and purposes)

**TRP Selection**:
- Checkpoints
- Known threat areas
- Avenues of approach
- Dead space (areas not visible from other positions)
- Communication relay positions
- Navigation references

**Naming Conventions**:
- Use NATO phonetic alphabet (Alpha, Bravo, Charlie...)
- Include type in name (Checkpoint Alpha, Overwatch Bravo, Bridge Charlie)
- Keep names short (displays may truncate long names)

**Common Mistakes**:
- ❌ Too many TRPs (overwhelming, slow to cycle through)
- ❌ Vague names ("Point 1", "Location A")
- ❌ Not updating TRPs when terrain changes
- ❌ Manual coordinate entry instead of capture (less accurate)

---

## 7.7 CONFIGURATION FILE MANAGEMENT

### **Save and Load Zones**

**Purpose**: Persist zone configurations between missions and platforms

**Configuration File Contains**:
- All defined no-fire zones
- All defined no-traverse zones
- All defined sector scan zones
- All defined TRPs
- Zone active/inactive status

**File Format**: JSON (text-based, human-readable)

**File Location**: Platform-specific (typically /config/zones/ directory)

---

### **Saving Configuration**

**When to Save**:
- After defining new zones
- After modifying existing zones
- Before shutdown (if zones changed)
- Periodically (backup)

**Procedure**:

**Step 1: Access Save Function**
1. Main Menu → Zone Definitions → "Save Configuration"
2. System prompts: "Save current zones to file?"

**Step 2: Enter Filename**
1. System prompts: "Enter filename"
2. Enter descriptive filename (e.g., "mission_alpha_zones")
3. Confirm
4. System appends ".json" automatically (e.g., "mission_alpha_zones.json")

**Step 3: Confirm Save**
1. System prompts: "Save to mission_alpha_zones.json? Yes/No"
2. Navigate to "Yes"
3. Press MENU/VAL
4. System saves file
5. Message: "Configuration Saved: mission_alpha_zones.json"

**Step 4: Return to Menu**
1. Press MENU/VAL or navigate to "Return to Zone Definitions"

---

### **Loading Configuration**

**When to Load**:
- Beginning of new mission (load mission-specific zones)
- After system restart (restore previous zones)
- Switching platforms (load platform-specific no-traverse zones)

**Procedure**:

**Step 1: Access Load Function**
1. Main Menu → Zone Definitions → "Load Configuration"
2. System displays list of available configuration files

**Configuration File List**:
```
┌────────────────────────────────────┐
│    LOAD CONFIGURATION              │
├────────────────────────────────────┤
│  Select configuration file:        │
│                                    │
│  > mission_alpha_zones.json        │
│    mission_bravo_zones.json        │
│    default_zones.json              │
│    platform_vehicle_01.json        │
│                                    │
│    Return to Zone Definitions      │
└────────────────────────────────────┘
```

**Step 2: Select File**
1. Navigate to desired configuration file (UP/DOWN)
2. Press MENU/VAL

**Step 3: Confirm Load**
1. System prompts: "Load mission_alpha_zones.json? This will replace current zones. Yes/No"
2. **WARNING**: Loading will DELETE all current zones and replace with zones from file
3. Navigate to "Yes"
4. Press MENU/VAL
5. System loads zones from file
6. Message: "Configuration Loaded: mission_alpha_zones.json"

**Step 4: Verify Zones Loaded**
1. Return to Zone Definitions menu
2. Navigate through zone types (No-Fire Zones, TRPs, etc.)
3. Verify expected zones present

---

### **Configuration File Best Practices**

**Naming**:
- **Mission-specific**: "mission_alpha_zones.json"
- **Platform-specific**: "platform_vehicle_01.json", "platform_ship_02.json"
- **Date-based**: "zones_2025_10_30.json"
- **Purpose-based**: "training_zones.json", "perimeter_defense.json"

**Organization**:
- Create separate config files for different missions
- Maintain "default" config with platform no-traverse zones
- Save frequently (after any zone changes)

**Backup**:
- Copy configuration files to external storage (USB drive)
- Keep backup of critical configurations
- Document configuration files in mission plan

**Loading Strategy**:
1. System startup → Load "default" config (platform no-traverse zones)
2. Mission start → Load "mission_alpha" config (adds mission no-fire zones and TRPs)
3. Mission end → Save any changes to "mission_alpha" config

---

## 7.8 ZONE MANAGEMENT EXERCISES

### **Exercise 7.8.1: Define No-Fire Zone**

**Objective**: Define a no-fire zone for friendly position

**Scenario**: Friendly forces at azimuth 030°-060°, elevation 0°-20°, range 0-800m

**Procedure**:
1. Access Main Menu → Zone Definitions → No-Fire Zones
2. Add New Zone
3. Name: "Friendly Position Alpha"
4. Az Start: 030°
5. Az Stop: 060°
6. El Min: 0°
7. El Max: 20°
8. Range Min: 0 m
9. Range Max: 800 m
10. Active: Yes
11. Save zone

**Verification**:
1. Exit menu
2. Aim gimbal into zone (Az ~045°, El ~10°)
3. Verify "NO-FIRE ZONE" warning appears
4. Aim gimbal outside zone
5. Verify warning disappears

**Success**: Zone defined, warning functions correctly

---

### **Exercise 7.8.2: Define Sector Scan Zone**

**Objective**: Define sector scan zone for perimeter surveillance

**Scenario**: Scan front arc 270° to 090° (left to right), elevation 5°, scan rate 10°/s

**Procedure**:
1. Access Auto Sector Scan Zones menu
2. Add New Zone
3. Name: "Perimeter 180°"
4. Az Start: 270° (left)
5. Az Stop: 090° (right)
6. Elevation: 5°
7. Scan Rate: 10°/s
8. Active: Yes
9. Save zone

**Verification**:
1. Exit menu
2. Cycle to AutoSectorScan mode (Button 11/13)
3. Observe gimbal scanning left to right
4. Verify scan covers 270° to 090°
5. Verify elevation ~5° (constant)

**Success**: Sector scan zone functions correctly

---

### **Exercise 7.8.3: Define TRPs**

**Objective**: Define 3 TRPs using capture method

**Scenario**: Define TRPs for Checkpoint Alpha, Overwatch Hill, Bridge North

**Procedure** (repeat for each TRP):
1. Switch to Manual mode
2. Aim gimbal at Checkpoint Alpha (instructor provides visual reference)
3. Note position on HUD
4. Enter menu → Zone Definitions → TRPs → Add New TRP
5. Name: "Checkpoint Alpha"
6. Select "Capture Current Position"
7. System captures current gimbal position
8. Description: "Main gate entrance" (optional)
9. Active: Yes
10. Save TRP
11. Repeat for Overwatch Hill and Bridge North

**Verification**:
1. Exit menu
2. Cycle to TRPScan mode (Button 11/13)
3. Press Button 14 (Next TRP)
4. Verify gimbal slews to each TRP in sequence
5. Verify TRP names displayed on HUD

**Success**: 3 TRPs defined and accessible in TRP Scan mode

---

### **Exercise 7.8.4: Save and Load Configuration**

**Objective**: Save zone configuration and reload it

**Procedure**:

**Part A: Save**
1. After defining zones in Exercises 7.8.1-7.8.3
2. Access Zone Definitions → "Save Configuration"
3. Filename: "training_exercise_zones"
4. Confirm save
5. Verify message: "Configuration Saved"

**Part B: Clear Zones (Simulated)**
1. Instructor deletes zones manually (or simulates zone loss)

**Part C: Load**
1. Access Zone Definitions → "Load Configuration"
2. Select "training_exercise_zones.json"
3. Confirm load (Warning: replaces current zones)
4. Verify message: "Configuration Loaded"

**Part D: Verify**
1. Navigate through zone types (No-Fire Zones, TRPs)
2. Verify all zones restored (same names and parameters as Part A)

**Success**: Configuration saved and restored correctly

---

### **Exercise 7.8.5: Complete Mission Zone Setup**

**Objective**: Define complete zone configuration for simulated mission

**Scenario**: "Perimeter defense mission at Forward Operating Base"

**Requirements**:
- 2 no-fire zones (friendly positions)
- 1 no-traverse zone (antenna mast)
- 2 sector scan zones (left arc, right arc)
- 5 TRPs (checkpoints and key terrain)

**Procedure**:
1. Define 2 no-fire zones (instructor provides friendly positions)
2. Define 1 no-traverse zone (instructor provides antenna location)
3. Define 2 sector scan zones (left arc 270°-315°, right arc 045°-090°)
4. Define 5 TRPs (instructor provides checkpoint/terrain locations)
5. Save configuration: "fob_perimeter_defense.json"

**Verification**:
1. Test each no-fire zone (aim gimbal, verify warnings)
2. Test no-traverse zone (attempt gimbal entry, verify blocked)
3. Test sector scan zones (AutoSectorScan mode, cycle between zones)
4. Test TRPs (TRPScan mode, cycle through TRPs)

**Evaluation**:
- All zones defined correctly
- Zones function as expected (warnings, gimbal behavior, scanning)
- Configuration saved successfully

**Time Limit**: 30 minutes

---

## LESSON 7 SUMMARY

**Key Points**:
1. Four zone types: No-Fire, No-Traverse, Auto Sector Scan, TRPs
2. Access zones via Main Menu → Zone Definitions
3. No-fire zones: Safety restriction, weapon fire blocked/warned
4. No-traverse zones: Gimbal restriction, motion blocked at boundary
5. Sector scan zones: Automated surveillance, used in AutoSectorScan mode
6. TRPs: Pre-defined aim points, used in TRPScan mode
7. Each zone type has Add, Modify, Delete functions
8. Use "Capture Current Position" for TRPs (most accurate)
9. Save configuration frequently (Save Configuration menu)
10. Load configuration at mission start (Load Configuration menu)

**Zone Definition Parameters**:
- **No-Fire/No-Traverse**: Az Start/Stop, El Min/Max, (Range for no-fire), Active
- **Sector Scan**: Az Start/Stop, Elevation, Scan Rate, Active
- **TRP**: Azimuth, Elevation, Description (optional), Active

**Best Practices**:
- Define zones during mission planning (not during operations)
- Name zones clearly and descriptively
- Verify zones after definition (test warnings, gimbal behavior)
- Save configurations frequently
- Maintain separate config files for missions and platforms
- Document zones in mission plan

**Skills Practiced**:
- Navigating Zone Definitions menu
- Defining no-fire zones
- Defining no-traverse zones
- Defining sector scan zones
- Defining TRPs (manual and capture methods)
- Modifying and deleting zones
- Saving zone configurations
- Loading zone configurations
- Complete mission zone setup

**Next Lesson**: Advanced Joystick Operations (Complete button mapping, context-sensitive functions, advanced techniques)

---

**IMPORTANT REMINDERS**:
- No-fire zones are safety-critical - define before live fire
- No-traverse zones protect platform equipment - verify before operations
- Save configuration after any zone changes
- Load correct configuration for each mission
- Verify zones function correctly (test before operations)
- Document all zone changes in mission log

---
