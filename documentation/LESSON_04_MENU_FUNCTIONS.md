# LESSON 4 - MENU FUNCTIONS

**Duration**: 5 hours
**Type**: Classroom + Practical
**References**: Operator manual, menu navigation guide

---

## INTRODUCTION

This lesson covers the El 7arress RCWS menu system, which provides access to all system configuration and settings. You will learn to navigate menus, configure reticle and color settings, access ballistics functions, view system status, and manage system operations through the menu interface.

---

## LEARNING OBJECTIVES

By the end of this lesson, you will be able to:
- Navigate all menu structures efficiently
- Configure reticle types and colors
- Access and modify ballistics settings (zeroing, windage)
- View system status information
- Perform system shutdown via menu

---

## 4.1 MENU NAVIGATION BASICS

### **Accessing the Main Menu**

**Method**: Press **MENU/VAL** button on Control Panel

**Result**: Main menu appears as overlay on display screen

**Display**:
- Video feed dims or pauses (depending on configuration)
- Menu window appears in center of screen
- Current selection highlighted
- Menu title displayed at top

### **Menu Controls**

| Button | Function |
|--------|----------|
| **MENU/VAL** | Open menu / Confirm selection / Exit menu |
| **UP (▲)** | Move selection up / Increase value |
| **DOWN (▼)** | Move selection down / Decrease value |

### **Navigation Workflow**

1. **Open Menu**: Press MENU/VAL
2. **Navigate**: Press UP or DOWN to highlight desired option
3. **Select**: Press MENU/VAL to enter submenu or activate option
4. **Return**: Navigate to "Return ..." option and press MENU/VAL, OR press MENU/VAL when NOT on a selectable item
5. **Exit**: Repeatedly return until back at operational screen

### **Menu Structure Overview**

```
MAIN MENU
├── RETICLE & DISPLAY
│   ├── Personalize Reticle
│   └── Personalize Colors
├── BALLISTICS
│   ├── Zeroing
│   ├── Clear Active Zero
│   ├── Windage
│   └── Clear Active Windage
├── SYSTEM
│   ├── Zone Definitions
│   ├── System Status
│   └── Shutdown System
├── INFO
│   └── Help/About
└── Return ...
```

---

## 4.2 MAIN MENU STRUCTURE

When you press MENU/VAL, the main menu displays with the following structure:

### **Main Menu Display**

```
┌────────────────────────────────────┐
│         MAIN MENU                  │
│  Navigate with UP/DOWN,            │
│  Select with MENU/VAL              │
├────────────────────────────────────┤
│                                    │
│  --- RETICLE & DISPLAY ---        │
│  > Personalize Reticle            │ ← Current selection (highlighted)
│    Personalize Colors              │
│  --- BALLISTICS ---                │
│    Zeroing                         │
│    Clear Active Zero               │
│    Windage                         │
│    Clear Active Windage            │
│  --- SYSTEM ---                    │
│    Zone Definitions                │
│    System Status                   │
│    Shutdown System                 │
│  --- INFO ---                      │
│    Help/About                      │
│    Return ...                      │
│                                    │
└────────────────────────────────────┘
```

**Section Headers** (non-selectable):
- "--- RETICLE & DISPLAY ---"
- "--- BALLISTICS ---"
- "--- SYSTEM ---"
- "--- INFO ---"

**Selectable Options**: All other items can be selected

**Navigation**: UP/DOWN buttons skip over section headers automatically

---

## 4.3 RETICLE & DISPLAY SETTINGS

### **4.3.1 Personalize Reticle Submenu**

**Access**: Main Menu → "Personalize Reticle"

**Purpose**: Select reticle type for aiming system

**Available Reticle Types**:

1. **Basic Crosshair**
   - Simple cross (+) design
   - Minimal visual clutter
   - Good for quick acquisition

2. **Box Crosshair**
   - Cross with surrounding box
   - Helps frame target
   - Good for tracking

3. **Standard Reticle**
   - Military-style crosshair with range marks
   - Horizontal and vertical reference lines
   - Suitable for general purpose

4. **Precision Reticle**
   - Fine crosshair with sub-degree marks
   - Minimal thickness
   - Best for long-range precision

5. **Mil-Dot Reticle**
   - Dots spaced at mil-radian intervals
   - Range estimation capability
   - Popular for sniper-style engagements

6. **CCIP Reticle (Continuously Computed Impact Point)**
   - Dynamic reticle showing predicted impact point
   - Adjusts for ballistic drop, zeroing offset, windage, lead angle
   - Most advanced option - RECOMMENDED for engagement

**Reticle Submenu Display**:

```
┌────────────────────────────────────┐
│    PERSONALIZE RETICLE             │
│    Select reticle type             │
├────────────────────────────────────┤
│                                    │
│  > Basic Crosshair                 │
│    Box Crosshair                   │
│    Standard Reticle                │
│    Precision Reticle               │
│    Mil-Dot Reticle                 │
│    CCIP Reticle                    │
│    Return to Main Menu             │
│                                    │
└────────────────────────────────────┘
```

**Procedure to Change Reticle**:

1. Navigate to desired reticle type (UP/DOWN)
2. Press MENU/VAL to select
3. Reticle changes immediately
4. Visual preview shown on screen (if available)
5. Navigate to "Return to Main Menu"
6. Press MENU/VAL to return

**Current Reticle Indicator**: Selected reticle may be marked with "✓" or highlighted

**Best Practices**:
- Use **CCIP Reticle** for combat engagements (accounts for all ballistic factors)
- Use **Precision Reticle** for long-range surveillance
- Use **Basic Crosshair** for rapid target acquisition

---

### **4.3.2 Personalize Colors Submenu**

**Access**: Main Menu → "Personalize Colors"

**Purpose**: Customize user interface color scheme and reticle color

**Configurable Colors**:
- **Reticle Color**: Color of aiming reticle
- **UI Accent Color**: Highlighting color for menus, tracking boxes, and HUD elements

**Color Submenu Display**:

```
┌────────────────────────────────────┐
│    PERSONALIZE COLORS              │
│    Select color style              │
├────────────────────────────────────┤
│                                    │
│  > Green (Military Standard)       │
│    Red                             │
│    Orange                          │
│    Blue                            │
│    Yellow                          │
│    White                           │
│    Cyan                            │
│    Magenta                         │
│    Return to Main Menu             │
│                                    │
└────────────────────────────────────┘
```

**Available Colors**:
1. **Green** - Military standard, high visibility, low eye strain (RECOMMENDED)
2. **Red** - High contrast, good for low-light, may affect night vision
3. **Orange** - High visibility, warm tone
4. **Blue** - Cool tone, less common
5. **Yellow** - Maximum contrast on dark backgrounds
6. **White** - Neutral, maximum brightness
7. **Cyan** - Good visibility, modern aesthetic
8. **Magenta** - High contrast, distinctive

**Procedure to Change Color**:

1. Navigate to desired color (UP/DOWN)
2. Press MENU/VAL to select
3. Color changes immediately throughout UI
4. Reticle color updates
5. Menu highlighting updates
6. Tracking box color updates
7. Navigate to "Return to Main Menu"
8. Press MENU/VAL to return

**Considerations**:
- **Green**: Best for prolonged use, standard military color
- **Red**: Avoid if using night vision devices (red light affects NVG)
- **Yellow/White**: Maximum brightness may cause glare in low-light
- **Personal Preference**: Select color that provides best visibility for your eyes and environment

---

## 4.4 BALLISTICS SETTINGS

### **4.4.1 Zeroing Procedure**

**Access**: Main Menu → "Zeroing"

**Purpose**: Calibrate weapon to camera boresight to correct aim point offset

**When to Use**:
- First-time weapon installation
- After weapon change
- After camera adjustment
- After impact/shock to system
- Periodic verification (monthly recommended)

**Overview**:
Zeroing corrects the offset between where the camera is pointed (reticle center) and where the weapon actually shoots. This offset exists because the camera and weapon are physically separated.

**Zeroing Process Flow**:
1. Enter zeroing mode from menu
2. Aim at known target at known distance (e.g., 100 meters)
3. Fire test round
4. Observe where round impacts relative to reticle
5. Adjust zeroing offset to move reticle to match impact point
6. Fire confirmation round
7. Finalize zeroing

**Detailed Procedure** (see Lesson 10 for complete step-by-step):

**Step 1: Access Zeroing Mode**
- Main Menu → Ballistics → Zeroing
- System displays "ZEROING MODE ACTIVE" message
- Zeroing adjustment controls activated

**Step 2: Initial Aim**
- Aim camera reticle at center of target
- Use known distance target (100m, 200m, etc.)
- Target should be clearly visible and stable

**Step 3: Fire Test Round**
- Engage Master Arm
- Fire single round
- Observe impact point on target

**Step 4: Measure Offset**
- Note impact point relative to reticle aim point
  - Example: "Impact 5cm LEFT, 10cm LOW of reticle"
- Convert to angular offset (use reference marks on target if available)

**Step 5: Adjust Zeroing Offset**
- In zeroing mode, use adjustment controls (platform-specific - may be joystick or menu)
- Adjust azimuth offset (left/right correction)
- Adjust elevation offset (up/down correction)
- Reticle moves on screen to show new aim point

**Step 6: Verification Fire**
- Aim reticle at same target center
- Fire confirmation round
- Impact should now be at reticle aim point (within tolerance)

**Step 7: Finalize**
- If verification successful, select "Finalize Zeroing" in menu
- If verification failed, repeat adjustment and verification
- System saves zeroing offsets
- Return to operational mode

**Zeroing Offset Range**: ±10° (azimuth and elevation)

**Zeroing Display Indicators**:
- "ZEROING MODE ACTIVE" status message
- Current zeroing offsets shown (Az: x.xx°, El: x.xx°)
- Reticle may display differently in zeroing mode (e.g., dotted outline)

**Safety During Zeroing**:
- Follow all range safety protocols (Lesson 1)
- Ensure backstop and range clear
- Use minimum ammunition (2-4 rounds typical)
- Always verify weapon safe after zeroing

---

### **4.4.2 Clear Active Zero**

**Access**: Main Menu → "Clear Active Zero"

**Purpose**: Remove zeroing adjustments and return to factory boresight

**When to Use**:
- Changing weapon type
- Troubleshooting aiming issues
- Starting fresh zeroing procedure
- Testing without zeroing offsets

**Procedure**:

1. Navigate to Main Menu → Ballistics → "Clear Active Zero"
2. Press MENU/VAL
3. System displays confirmation prompt:
   ```
   CLEAR ACTIVE ZERO?

   This will remove all zeroing adjustments.
   You will need to re-zero the weapon.

   > No (Cancel)
     Yes (Clear Zero)
   ```
4. Navigate to "Yes (Clear Zero)"
5. Press MENU/VAL to confirm
6. System clears zeroing offsets
7. Displays "ZEROING CLEARED" message
8. Returns to main menu

**Effect**:
- Azimuth zeroing offset = 0.0°
- Elevation zeroing offset = 0.0°
- CCIP reticle returns to camera boresight (no ballistic correction)

**Warning**: After clearing zero, the weapon will NOT be accurately zeroed. You must perform zeroing procedure (Section 4.4.1) before operational use.

---

### **4.4.3 Windage Procedure**

**Access**: Main Menu → "Windage"

**Purpose**: Compensate for crosswind effects on projectile trajectory

**When to Use**:
- Operating in significant wind conditions (>10 knots crosswind)
- Long-range engagements (>500 meters)
- Before mission in known wind environment

**Overview**:
Windage compensation adjusts the aim point to account for wind pushing the projectile left or right. The system calculates windage correction based on:
- Wind speed (knots)
- Wind direction (degrees relative to fire direction)
- Target range (from LRF)
- Projectile ballistic properties

**Windage Process Flow**:
1. Enter windage mode from menu
2. Capture wind direction (aim weapon into wind direction)
3. Input wind speed (knots)
4. Finalize windage
5. System applies windage correction to reticle aim point

**Detailed Procedure**:

**Step 1: Access Windage Mode**
- Main Menu → Ballistics → Windage
- System displays "WINDAGE MODE ACTIVE" message

**Step 2: Capture Wind Direction**
- Aim weapon in the direction the wind is blowing FROM
  - Example: If wind blowing from LEFT to RIGHT, aim LEFT
- Press designated button (platform-specific) or select "Capture Wind Direction" in menu
- System records current azimuth as wind direction
- Display shows: "WIND DIRECTION: xxx°"

**Step 3: Input Wind Speed**
- System prompts for wind speed
- Use UP/DOWN buttons to adjust wind speed value (in knots)
- Range: 0 to 50 knots
- Increment: 1 knot per button press
- Display shows: "WIND SPEED: xx knots"

**Step 4: Finalize Windage**
- Navigate to "Finalize Windage" option
- Press MENU/VAL
- System saves windage parameters
- Windage correction now active

**Step 5: Return to Operational Mode**
- Navigate to "Return to Main Menu"
- Press MENU/VAL
- System applies windage correction to CCIP reticle

**Windage Display Indicators**:
- "WINDAGE ACTIVE" status indicator
- Wind speed and direction shown on HUD (if configured)
- CCIP reticle offset includes windage component

**Estimating Wind Speed**:

| Wind Speed | Observable Effects |
|------------|-------------------|
| 5-10 knots | Leaves rustle, light flag movement |
| 10-15 knots | Small branches move, flags extended |
| 15-20 knots | Large branches move, difficult to walk against |
| 20-30 knots | Whole trees move, very difficult to walk |
| >30 knots | Structural damage, operations should cease |

**Limitations**:
- Windage assumes constant wind across projectile path (not always accurate)
- Does not account for vertical wind components
- Accuracy decreases with range >1000m

---

### **4.4.4 Clear Active Windage**

**Access**: Main Menu → "Clear Active Windage"

**Purpose**: Remove windage compensation and return to no-wind aiming

**When to Use**:
- Wind conditions changed significantly
- Wind ceased
- Troubleshooting aiming issues
- Starting fresh windage calculation

**Procedure**:

1. Navigate to Main Menu → Ballistics → "Clear Active Windage"
2. Press MENU/VAL
3. System displays confirmation prompt:
   ```
   CLEAR ACTIVE WINDAGE?

   This will remove windage compensation.

   > No (Cancel)
     Yes (Clear Windage)
   ```
4. Navigate to "Yes (Clear Windage)"
5. Press MENU/VAL to confirm
6. System clears windage parameters
7. Displays "WINDAGE CLEARED" message
8. Returns to main menu

**Effect**:
- Wind speed = 0 knots
- Wind direction = 0°
- CCIP reticle no longer includes windage offset

---

## 4.5 SYSTEM CONFIGURATION

### **4.5.1 Zone Definitions**

**Access**: Main Menu → "Zone Definitions"

**Purpose**: Define, modify, and manage safety zones and surveillance patterns

**Zone Types**:
1. **No-Fire Zones** - Areas where weapon discharge is prohibited
2. **No-Traverse Zones** - Areas gimbal cannot enter
3. **Auto Sector Scan Zones** - Automated surveillance scan patterns
4. **Target Reference Points (TRPs)** - Preset aim points

**Detailed procedures covered in Lesson 7 - Zone Definition & Management**

**Quick Access Overview**:

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

**Typical Operations**:
- **Add Zone**: Select zone type → "Add New Zone" → Define parameters → Save
- **Modify Zone**: Select zone type → Select existing zone → Modify parameters → Save
- **Delete Zone**: Select zone type → Select existing zone → "Delete Zone" → Confirm
- **Save/Load**: Save current zones to file or load zones from file

---

### **4.5.2 System Status**

**Access**: Main Menu → "System Status"

**Purpose**: View detailed system status and diagnostic information

**Status Display Categories**:

1. **Device Connection Status**
   - Day Camera: Connected / Disconnected / Fault
   - Thermal Camera: Connected / Disconnected / Fault
   - Laser Range Finder: Connected / Disconnected / Fault
   - Gimbal Drive System (Az/El): Connected / Disconnected / Fault
   - Gyroscope/IMU: Connected / Disconnected / Fault
   - Weapon Actuator: Connected / Disconnected / Fault
   - Control Panel: Connected / Disconnected / Fault
   - Joystick: Connected / Disconnected / Fault

2. **Gimbal Status**
   - Current Position: Az: xxx.xx°, El: xx.xx°
   - Azimuth Servo: Operational / Fault / Overtemp / Overcurrent
   - Elevation Servo: Operational / Fault / Overtemp / Overcurrent
   - Temperature: Az: xx°C, El: xx°C

3. **Camera Status**
   - Active Camera: Day / Thermal
   - Zoom Level: xx.x
   - Field of View: xx.x°
   - Video Feed: Active / Frozen / Lost

4. **LRF Status**
   - Last Range Reading: xxxx m
   - Reading Age: x seconds ago
   - Status: Ready / Busy / Fault / No Response

5. **Tracking Status**
   - Tracking Phase: Off / Acquisition / Lock Pending / Active Lock / Coast / Firing
   - Target Position: X: xxx, Y: xxx (pixels)
   - Track Confidence: xx%
   - Track Velocity: X: xx px/s, Y: xx px/s

6. **Ballistics Status**
   - Zeroing: Active / Inactive
     - Az Offset: x.xx°, El Offset: x.xx°
   - Windage: Active / Inactive
     - Speed: xx knots, Direction: xxx°
   - Lead Angle Compensation: Active / Inactive / Lag / Zoom Out Warning
     - Az Offset: x.xx°, El Offset: x.xx°

7. **System Information**
   - Software Version: x.x.x
   - Uptime: HH:MM:SS
   - Configuration File: filename.json

**System Status Display** (example):

```
┌────────────────────────────────────┐
│    SYSTEM STATUS                   │
│    [Press UP/DOWN to scroll]       │
├────────────────────────────────────┤
│                                    │
│  DEVICE CONNECTIONS:               │
│    Day Camera:       [✓] OK        │
│    Thermal Camera:   [✓] OK        │
│    LRF:              [✓] OK        │
│    Gimbal Az:        [✓] OK        │
│    Gimbal El:        [✓] OK        │
│    IMU:              [✓] OK        │
│    Weapon Actuator:  [✓] OK        │
│    Control Panel:    [✓] OK        │
│    Joystick:         [✓] OK        │
│                                    │
│  GIMBAL STATUS:                    │
│    Position: Az: 045.32° El: 12.5° │
│    Az Servo: Operational (Temp: 35°C)│
│    El Servo: Operational (Temp: 33°C)│
│                                    │
│  BALLISTICS:                       │
│    Zeroing: ACTIVE                 │
│      Offsets: Az +0.25° El -0.10°  │
│    Windage: INACTIVE               │
│    Lead Angle: INACTIVE            │
│                                    │
│  [More information below...]       │
│                                    │
│  Return to Main Menu               │
│                                    │
└────────────────────────────────────┘
```

**Navigation**:
- Use UP/DOWN to scroll through status information
- Press MENU/VAL on "Return to Main Menu" to exit

**Use Cases**:
- Troubleshooting device connection issues
- Monitoring gimbal temperature during prolonged operation
- Verifying ballistics settings (zeroing, windage) are active
- Checking tracking system performance
- Viewing software version for maintenance reporting

**Detailed status information covered in Lesson 11 - System Status & Monitoring**

---

### **4.5.3 Shutdown System**

**Access**: Main Menu → "Shutdown System"

**Purpose**: Software-initiated graceful shutdown of system and platform

**When to Use**:
- Normal end-of-operation shutdown
- Controlled power-down
- When menu shutdown is preferred over manual power-off

**Procedure**:

1. **IMPORTANT**: Ensure weapon is safe and clear BEFORE initiating shutdown
2. Navigate to Main Menu → "Shutdown System"
3. Press MENU/VAL
4. System displays confirmation prompt:
   ```
   SHUTDOWN SYSTEM?

   This will:
   - Quit the application
   - Power down the system
   - May shut down the platform computer

   Ensure weapon is safe and clear.

   > No (Cancel)
     Yes (Shutdown)
   ```
5. Verify weapon safe
6. Navigate to "Yes (Shutdown)"
7. Press MENU/VAL to confirm
8. System begins shutdown sequence:
   - Application quits
   - Configuration saved (if changes pending)
   - System sends shutdown command to platform
   - Display turns off
   - Platform may power down (vehicle/ship computer)

**Shutdown Sequence** (automatic):
1. Menu selection confirmed
2. Display shows "SHUTTING DOWN..." message
3. Configuration auto-saved
4. Application closes
5. System issues platform shutdown command (platform-specific)
6. Power-down (5-10 seconds)

**Warning**: This shutdown method may power down the entire platform (vehicle/ship computer). Ensure:
- All other critical systems are secured
- Platform can be powered down safely
- Weapon is safe and clear
- Ammunition removed (if applicable)

**Alternative**: Use manual shutdown procedure (Lesson 3, Section 3.7) if you want more control over the shutdown sequence or if platform should remain powered.

---

## 4.6 HELP/ABOUT

### **Access**: Main Menu → "Help/About"

**Purpose**: Display system information and basic help

**Information Displayed**:
- System name: El 7arress RCWS
- Software version
- Build date
- Copyright information
- Brief usage tips
- Contact information for support (if configured)

**Help/About Display** (example):

```
┌────────────────────────────────────┐
│    EL 7ARRESS RCWS                 │
│    HELP / ABOUT                    │
├────────────────────────────────────┤
│                                    │
│  El 7arress Remote Controlled      │
│  Weapon Station                    │
│                                    │
│  Version: 4.5.0                    │
│  Build Date: 2025-10-30            │
│                                    │
│  © 2025 Tunisian Ministry of       │
│  Defense                           │
│                                    │
│  QUICK TIPS:                       │
│  - Press MENU/VAL to access menus  │
│  - Emergency Stop halts all motion │
│  - Use CCIP reticle for engagement │
│  - Perform zeroing after weapon    │
│    installation                    │
│                                    │
│  For operator manual and training, │
│  contact your unit training officer│
│                                    │
│  Return to Main Menu               │
│                                    │
└────────────────────────────────────┘
```

**Navigation**:
- Press UP/DOWN to scroll (if content exceeds screen)
- Press MENU/VAL on "Return to Main Menu" to exit

---

## 4.7 MENU NAVIGATION EXERCISES

### **Exercise 4.7.1: Menu Speed Navigation**

**Objective**: Navigate menus quickly and efficiently

**Procedure**:
1. Start at operational screen (no menus open)
2. Instructor calls out menu path (e.g., "Main Menu → Ballistics → Zeroing")
3. Navigate to specified location as quickly as possible
4. Announce when reached
5. Instructor records time
6. Repeat for different menu paths

**Scoring**:
- Goal: Complete each navigation in <10 seconds
- Proficiency: Can navigate anywhere in menu structure without hesitation

**Common Menu Paths to Practice**:
1. Operational → Main Menu → Personalize Reticle → CCIP Reticle
2. Operational → Main Menu → Ballistics → Zeroing
3. Operational → Main Menu → System Status → (view status)
4. Operational → Main Menu → Zone Definitions → No-Fire Zones
5. Operational → Main Menu → Shutdown System

---

### **Exercise 4.7.2: Reticle and Color Configuration**

**Objective**: Change reticle type and UI colors

**Procedure**:
1. Access Main Menu
2. Navigate to "Personalize Reticle"
3. Select **CCIP Reticle**
4. Observe reticle change on display
5. Return to Main Menu
6. Navigate to "Personalize Colors"
7. Select **Green** color
8. Observe UI color change
9. Return to Main Menu
10. Exit menu system

**Verification**:
- Reticle is CCIP type
- UI color is green (menus, overlays, tracking boxes)
- Settings persist after exiting menu

---

### **Exercise 4.7.3: System Status Review**

**Objective**: Access and interpret system status information

**Procedure**:
1. Access Main Menu
2. Navigate to "System Status"
3. Review device connection status - verify all devices connected
4. Review gimbal status - note current position and temperatures
5. Review ballistics status - check if zeroing/windage active
6. Scroll through entire status display
7. Identify any faults or warnings
8. Return to Main Menu
9. Exit menu system

**Questions to Answer**:
- Are all devices connected?
- What is current gimbal position (Az/El)?
- What are gimbal servo temperatures?
- Is zeroing active? What are the offsets?
- Is windage active? What are the parameters?
- What is current tracking status?
- Any faults or warnings present?

---

### **Exercise 4.7.4: Clear Zero and Windage**

**Objective**: Practice clearing ballistics settings

**Procedure**:
1. Access Main Menu
2. Navigate to Ballistics → "Clear Active Zero"
3. Confirm clearing zeroing
4. Observe "ZEROING CLEARED" message
5. Navigate to Ballistics → "Clear Active Windage"
6. Confirm clearing windage
7. Observe "WINDAGE CLEARED" message
8. Navigate to System Status
9. Verify zeroing and windage show as "INACTIVE"
10. Return to Main Menu
11. Exit menu system

**Note**: This is a training exercise. In operational use, do NOT clear zeroing unless there is a specific reason (weapon change, troubleshooting, etc.).

---

## 4.8 MENU OPERATION TIPS

### **Efficiency Tips**

1. **Memorize Common Paths**
   - Most-used: Personalize Reticle, System Status, Zone Definitions
   - Memorize the number of UP/DOWN presses to reach each

2. **Use Section Headers as Landmarks**
   - "--- RETICLE & DISPLAY ---" = top section
   - "--- BALLISTICS ---" = middle-top section
   - "--- SYSTEM ---" = middle-bottom section
   - "--- INFO ---" = bottom section

3. **Return Quickly**
   - "Return to Main Menu" or "Return ..." option always at bottom of submenus
   - OR press MENU/VAL when NOT on a selectable item (jumps out one level)

4. **Minimize Menu Time**
   - Plan your menu actions before opening menu
   - Execute quickly to minimize time away from operational display
   - Close menu immediately when done

### **Best Practices**

1. **Configuration Before Operations**
   - Set reticle type (CCIP recommended)
   - Set color preference
   - Verify zones loaded
   - Check system status
   - THEN begin operational tasks

2. **Menu Access Restrictions**
   - Some menus may be disabled during critical operations (e.g., tracking, firing)
   - If menu doesn't open, check current operational state

3. **Settings Persistence**
   - Most settings save automatically
   - Zone configurations should be saved manually (Save Configuration option)
   - Zeroing and windage persist across power cycles

4. **Menu Documentation**
   - If unsure about menu option, consult this manual or Help/About screen
   - Do not experiment with unfamiliar options during operations

---

## LESSON 4 SUMMARY

**Key Points**:
1. Access main menu with MENU/VAL button
2. Navigate with UP/DOWN, select with MENU/VAL
3. Menu structure: RETICLE & DISPLAY, BALLISTICS, SYSTEM, INFO
4. CCIP reticle recommended for engagement (accounts for all ballistic factors)
5. Color selection affects reticle, UI overlays, and menu highlighting
6. Zeroing corrects gun-camera boresight offset (requires range and test fire)
7. Windage compensates for crosswind (requires wind speed and direction input)
8. Clear Zero/Windage removes compensations (use cautiously)
9. Zone Definitions manages safety zones and surveillance patterns (Lesson 7 details)
10. System Status displays device connections, gimbal status, ballistics status (Lesson 11 details)
11. Shutdown System performs graceful software-initiated shutdown
12. Practice menu navigation to minimize time in menus during operations

**Skills Practiced**:
- Menu navigation (open, navigate, select, return, exit)
- Reticle type selection
- UI color customization
- Ballistics settings access (zeroing, windage)
- System status review
- Clearing ballistics settings
- Menu efficiency and speed

**Next Lesson**: Engagement Process / Simulation Exercise (Target acquisition, tracking, and weapons engagement)

---

**IMPORTANT REMINDERS**:
- Always ensure weapon safe before entering menus
- Minimize menu time - menus obscure operational display
- Verify settings after making changes (check System Status)
- Document configuration changes in maintenance log
- Do not clear zeroing unless there is a specific reason

---
