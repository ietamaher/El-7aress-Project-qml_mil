# LESSON 5 - ENGAGEMENT PROCESS / SIMULATION EXERCISE

**Duration**: 4 hours
**Type**: Simulator + Practical
**References**: Operator manual, engagement procedures, joystick manual

---

## INTRODUCTION

This lesson teaches the complete target engagement sequence from initial detection through weapon employment. You will learn target acquisition, tracking system operation, tracking phases, and simulated weapons engagement. This is the most critical lesson for combat operations.

---

## LEARNING OBJECTIVES

By the end of this lesson, you will be able to:
- Describe the complete target engagement sequence
- Initiate and control tracking operations
- Adjust acquisition box size during target selection
- Execute simulated weapons engagement
- Abort tracking in emergency situations

---

## 5.1 TARGET ENGAGEMENT SEQUENCE OVERVIEW

### **Complete Engagement Cycle**

```
1. DETECT
   ↓
2. IDENTIFY
   ↓
3. ACQUIRE (Tracking Acquisition)
   ↓
4. TRACK (Lock-On and Active Tracking)
   ↓
5. ENGAGE (Weapons Employment)
   ↓
6. ASSESS (Battle Damage Assessment)
```

### **Detailed Sequence**

**Phase 1: DETECT**
- Scan area of interest (manual or automated scan modes)
- Visual detection of potential target
- May use radar cues (radar slew mode) if available
- Initial target location noted

**Phase 2: IDENTIFY**
- Slew gimbal to center target in reticle
- Use zoom to magnify target
- Positive identification (PID) of target
- Verify not friendly forces
- Verify target meets engagement criteria

**Phase 3: ACQUIRE**
- Enter tracking acquisition mode (Button 4)
- Position acquisition box over target
- Adjust box size to frame target (D-Pad / Hat Switch)
- Request tracker lock-on (Button 4 again)

**Phase 4: TRACK**
- System locks onto target
- Tracking system follows target automatically
- Gimbal follows target movement
- Maintain track (system automatically keeps reticle on target)

**Phase 5: ENGAGE**
- Verify target still valid
- Check fire authorization
- Enable Lead Angle Compensation (if moving target)
- Engage Master Arm (Button 0 / trigger stage 1)
- Fire weapon (Button 5 / trigger stage 2)
- Observe effect on target

**Phase 6: ASSESS**
- Cease fire
- Assess battle damage
- Continue tracking or stop tracking
- Report results
- Re-engage if necessary

---

## 5.2 TRACKING SYSTEM OVERVIEW

### **Tracking Modes**

The El 7arress RCWS uses **video-based automated tracking** to follow targets.

**Tracking Phases** (state machine):

1. **Off** - No tracking active
2. **Acquisition** - Operator positioning tracking gate
3. **Lock Pending** - System initializing tracker
4. **Active Lock** - Tracking target successfully
5. **Coast** - Temporary target loss (tracker predicting position)
6. **Firing** - Tracking active during weapon discharge

### **Tracking Phase Transitions**

```
     OFF
      ↓ [Press Button 4]
  ACQUISITION
      ↓ [Adjust box with D-Pad]
      ↓ [Press Button 4 again]
  LOCK PENDING
      ↓ [Tracker initialized]
  ACTIVE LOCK ⟷ COAST
      ↓         [Target temporarily lost]
   FIRING
      ↓ [Cease fire or tracking abort]
     OFF
```

### **Tracking Controls**

| Button | Function | Phase Active |
|--------|----------|--------------|
| **Button 4 (single press)** | Start Acquisition / Request Lock-On | Off → Acquisition, Acquisition → Lock Pending |
| **Button 4 (double-click < 500ms)** | Emergency Tracking Abort | Any tracking phase → Off |
| **D-Pad UP** | Decrease acquisition box height | Acquisition only |
| **D-Pad DOWN** | Increase acquisition box height | Acquisition only |
| **D-Pad LEFT** | Decrease acquisition box width | Acquisition only |
| **D-Pad RIGHT** | Increase acquisition box width | Acquisition only |

---

## 5.3 TRACKING ACQUISITION PHASE

### **Entering Acquisition Mode**

**Prerequisites**:
- System in MANUAL motion mode
- Target visually identified on screen
- Target centered in reticle (approximately)

**Procedure**:

**Step 1: Press Button 4 (Track Select)**

**Result**:
- System enters **ACQUISITION** phase
- Yellow acquisition box appears on screen
- Box initially centered on reticle
- Box is adjustable in size
- Joystick gimbal control remains active (you can still move gimbal)

**Display Changes**:
- Status indicator: "Mode: ACQUISITION"
- Yellow box displayed (default size: ~100x100 pixels)
- Instructions may appear: "Adjust box size with D-Pad, Press Button 4 to lock"

---

### **Positioning the Acquisition Box**

**Objective**: Frame the target within the yellow acquisition box

**Method 1: Gimbal Movement** (if target not centered)
- Use joystick to move gimbal
- Center target in field of view
- Acquisition box follows reticle (stays centered on screen)

**Method 2: Box Auto-Centered**
- Box automatically centered on current reticle position
- If reticle is on target, box is automatically on target

**Goal**: Get target inside the yellow box, with some margin around target edges

---

### **Adjusting Acquisition Box Size**

**Purpose**: Size the box to fit the target - not too tight, not too loose

**Controls**: D-Pad / Hat Switch on joystick

| D-Pad Direction | Effect | Step Size |
|-----------------|--------|-----------|
| **UP ↑** | Decrease box height | -4 pixels |
| **DOWN ↓** | Increase box height | +4 pixels |
| **LEFT ←** | Decrease box width | -4 pixels |
| **RIGHT →** | Increase box width | +4 pixels |

**Example**:
- Target is 80 pixels wide by 60 pixels tall
- Press D-Pad RIGHT 5 times → box width increases by 20 pixels (now 120 pixels wide)
- Press D-Pad DOWN 3 times → box height increases by 12 pixels (now 112 pixels tall)
- Target now framed with ~20-pixel margin on all sides (ideal)

**Box Sizing Guidelines**:

| Target Framing | Effect on Tracking |
|----------------|--------------------|
| **Too Tight** (target touches edges) | Tracker may lose target if target moves or rotates |
| **Optimal** (target has 10-30% margin) | Best tracking performance |
| **Too Loose** (target is small in box) | Background clutter may confuse tracker |

**Best Practices**:
- **Frame target with ~20% margin** on all sides
- For vehicles: Include entire vehicle hull, exclude ground
- For personnel: Include head, torso, and legs, exclude too much background
- For moving targets: Size slightly larger (anticipate motion)

---

### **Acquisition Box Display**

**Box Appearance**:
- Color: Yellow (or configured UI accent color)
- Style: Dashed or solid outline
- Size: Adjustable from ~50x50 to ~400x400 pixels (approximate)
- Position: Centered on screen / reticle

**Box Size Indicators** (may be displayed):
- "Box: 120x100" (width x height in pixels)

---

### **Requesting Lock-On**

**When Ready**:
- Target fully visible and framed in acquisition box
- Target has good contrast against background
- Target not moving erratically

**Action**: Press **Button 4** again (second press)

**Result**:
- System transitions to **LOCK PENDING** phase
- Acquisition box changes color (yellow → cyan or flashing)
- Status indicator: "Mode: LOCK PENDING"
- Video tracking system initializes tracker on target

**Duration**: Lock Pending phase typically lasts 0.5-2 seconds

---

### **Acquisition Phase Notes**

**⚠️ Important**:
- You CAN still move gimbal during acquisition (joystick active)
- D-Pad ONLY adjusts box size during acquisition (no effect in other modes)
- If target moves off-screen, move gimbal to re-center target before requesting lock-on
- You can cancel acquisition by double-clicking Button 4 (returns to Off phase)

---

## 5.4 LOCK PENDING PHASE

### **Lock Pending Overview**

**Purpose**: Video tracking system initializes discriminative correlation filter (DCF) tracker on target

**Duration**: 0.5 to 2 seconds (typically 1 second)

**Operator Action**: **WAIT** - Do not move gimbal rapidly during this phase

### **What Happens During Lock Pending**

1. System captures reference image of target from acquisition box area
2. Initializes DCF tracker algorithm
3. Calculates target features and correlation template
4. Begins tracking target in video stream

### **Lock Pending Display**

- **Status**: "Mode: LOCK PENDING"
- **Acquisition Box**: Color changes (yellow → cyan) or flashes
- **Message**: "Initializing tracker..." may display

### **Transition to Active Lock**

**Success**:
- Tracker initialization successful
- System automatically transitions to **ACTIVE LOCK** phase
- Acquisition box changes to tracking box (green solid outline)
- Status: "Mode: ACTIVE LOCK" or "TRACKING"

**Failure** (rare):
- Tracker initialization failed (poor contrast, target too small, etc.)
- System returns to **ACQUISITION** phase
- Operator must retry (re-position box, adjust size, request lock-on again)
- Or abort and start over

### **During Lock Pending - Do's and Don'ts**

**✅ DO**:
- Keep gimbal relatively steady (small movements OK)
- Keep target in field of view
- Wait patiently for lock confirmation

**❌ DON'T**:
- Make rapid gimbal movements
- Zoom in/out
- Switch cameras
- Press buttons (except emergency abort)

---

## 5.5 ACTIVE LOCK PHASE

### **Active Lock Overview**

**Congratulations!** The system is now automatically tracking the target.

**System Behavior**:
- Tracker follows target in video stream at 30 Hz (30 times per second)
- Gimbal automatically moves to keep target centered
- Operator does NOT need to manually control gimbal (joystick axis inputs ignored in tracking mode)
- Reticle stays on target center

### **Active Lock Display**

**Visual Indicators**:
- **Tracking Box**: Green solid outline around target
- **Status**: "Mode: ACTIVE LOCK" or "TRACKING"
- **Indicator Light**: "TRACKING" light ON (green) on Control Panel
- **Target Information**:
  - Target position (pixels): X: xxx, Y: xxx
  - Track confidence: xx% (typically >80% for good track)
  - Target velocity (if available): X: xx px/s, Y: xx px/s

**Tracking Box**:
- Color: Green (good track) or Yellow (marginal track)
- Size: Adjusts automatically to target size
- Position: Follows target movement

---

### **Operator Role During Active Lock**

**Primary Role**: **MONITOR** the track - system is doing the work

**Monitor For**:
1. **Track Quality**
   - Green box = good track
   - Yellow box = marginal track (may lose soon)
   - Track confidence indicator >70% (good), <50% (poor)

2. **Target Status**
   - Target still correctly identified (didn't jump to wrong object)
   - Target still valid (meets engagement criteria)
   - Target not obscured or about to be obscured

3. **Gimbal Position**
   - Gimbal staying within operational limits
   - Not approaching no-traverse zones
   - Elevation not approaching limits (-20° / +60°)

**If Track Degrades**:
- If track confidence low or box turns yellow: Be prepared for Coast mode or track loss
- If tracking wrong object: Abort tracking (double-click Button 4) and restart

**If Target Lost**:
- System automatically enters **COAST** mode (see Section 5.6)

---

### **Joystick Control During Active Lock**

**Main Stick Axes** (X and Y):
- **IGNORED** during Active Lock
- Gimbal controlled by tracking system, not operator
- Moving joystick has no effect on gimbal

**Buttons** (still active):
- Button 0 (Master Arm): Active
- Button 2 (LAC toggle): Active
- Button 3 (Dead Man Switch): Active
- Button 4 (double-click abort): Active
- Button 5 (Fire): Active
- Button 6/8 (Zoom): Active (use cautiously - may affect track)
- Button 7/9 (LUT): Active (thermal camera only)
- Button 11/13 (Mode cycle): BLOCKED (cannot cycle modes during tracking)

**Camera and Zoom**:
- You CAN zoom in/out during tracking, but:
  - Sudden zoom may cause tracking to lose target (Coast mode)
  - Gradual zoom is safer
  - Recommended: Set zoom level before starting tracking

---

### **Active Lock Duration**

**Tracking continues indefinitely** as long as:
- Target remains in field of view
- Target contrast sufficient
- System maintains lock

**Tracking Ends When**:
- Operator aborts (double-click Button 4)
- Target lost (Coast mode, then returns to Off if not re-acquired)
- System error
- Mode change (if operator manually changes to different motion mode - not recommended)

---

## 5.6 COAST MODE

### **Coast Mode Overview**

**Purpose**: Maintain tracking during temporary target loss

**When Activated**:
- Target temporarily obscured (passes behind object)
- Target leaves field of view briefly (gimbal lag)
- Camera occlusion (dust, smoke, etc.)
- Tracker loses visual lock on target

**System Behavior in Coast Mode**:
- Tracker PREDICTS target position based on last known velocity
- Gimbal continues moving to predicted position
- System attempts to re-acquire target
- Tracking box may blink or change color (green → yellow/amber)

### **Coast Mode Display**

- **Status**: "Mode: COAST" or "TRACKING (COAST)"
- **Tracking Box**: Yellow or amber (instead of green)
- **Warning**: "COASTING - TARGET LOST" may display

### **Coast Duration**

**Typical**: 1-3 seconds

**Outcomes**:

**1. Target Re-Acquired** (success):
- Target reappears in field of view
- Tracker re-locks on target
- System returns to **ACTIVE LOCK** phase
- Tracking box returns to green
- Tracking continues normally

**2. Coast Timeout** (failure):
- Target not re-acquired within timeout period (typically 5 seconds)
- System gives up
- Tracking stops
- System returns to **OFF** phase
- Operator must restart tracking if desired

### **Operator Action During Coast**

**✅ DO**:
- Wait patiently - system is attempting re-acquisition
- Keep gimbal pointed in last known target direction (system doing this automatically)
- Be ready for track to resume

**❌ DON'T**:
- Panic
- Make manual gimbal movements (joystick ignored anyway)
- Abort prematurely (give system a chance to re-acquire)

**When to Abort**:
- If target definitely not coming back (destroyed, permanently obscured)
- If coasting in wrong direction (tracker confused)
- If mission changed

---

## 5.7 FIRING MODE

### **Firing Mode Overview**

**Purpose**: Special tracking mode during weapon discharge

**When Activated**:
- Tracking active (Active Lock or Coast)
- Master Arm engaged (Button 0)
- Fire button pressed (Button 5)

**System Behavior**:
- Tracking continues
- Gimbal holds steady during fire (minimizes recoil-induced motion)
- Reticle stays on target
- Weapon fires

### **Firing Mode Features**

**Stabilization**:
- Enhanced gimbal stabilization during fire
- Compensates for weapon recoil
- Maintains track despite weapon vibration

**Tracking During Fire**:
- Tracker continues updating target position
- Gimbal micro-adjusts to stay on target
- Provides continuous aim solution

**Lead Angle Compensation** (if active):
- CCIP reticle includes lead angle offset
- Aim point ahead of target (for moving targets)
- Detailed in Lesson 9 - Lead Angle Compensation

### **Firing Mode Ends When**:
- Fire button released (Button 5)
- Master Arm disengaged (Button 0)
- Tracking aborted (double-click Button 4)
- Target lost and Coast timeout

### **Operator Action During Firing**

**✅ DO**:
- Hold trigger steady (don't jerk or pulse unnecessarily)
- Observe effect on target (rounds impacting target?)
- Adjust fire as needed (cease fire if missing, continue if hitting)
- Monitor ammunition count (if available)

**❌ DON'T**:
- Release fire button too early (may need sustained fire for effect)
- Try to manually "steer" rounds with joystick (system is tracking automatically)
- Fire excessively (conserve ammunition)

---

## 5.8 TRACKING ABORT (EMERGENCY)

### **When to Abort Tracking**

**Abort tracking immediately if**:
- Tracking wrong target (locked on friendly, civilian, wrong target)
- Target no longer valid (no longer meets engagement criteria)
- Safety concern (entering no-fire zone, gimbal near obstruction, etc.)
- Mission change (new priority target, orders to cease)
- Tracking behaving erratically (gimbal moving unexpectedly)

### **Abort Procedure**

**Action**: **Double-click Button 4** (< 500ms between presses)

**Effect**:
1. Tracking IMMEDIATELY stops
2. System returns to **OFF** phase
3. Gimbal stops moving (holds current position)
4. Tracking box disappears
5. Weapon fire inhibited (even if Master Arm engaged)
6. Status: "Mode: MANUAL"

**Timing**:
- Two presses of Button 4 within 500 milliseconds (half a second)
- Press 1 → Press 2 (rapid succession)
- If presses too slow (>500ms apart), system interprets as two single presses (may re-start tracking instead of aborting)

**Practice**:
- Practice double-click timing during training
- Muscle memory critical for emergency abort

---

### **After Abort**

**System State**:
- Manual mode active
- Joystick gimbal control restored
- No tracking active
- Operator in full control

**Next Actions** (depending on situation):
- Re-acquire correct target and restart tracking
- Return to surveillance mode
- Engage different target
- Follow commander's orders

---

## 5.9 WEAPONS ENGAGEMENT SEQUENCE (DETAILED)

### **Pre-Engagement Checklist**

Before engaging target, verify:

- [ ] Target positively identified (PID)
- [ ] Target is valid (meets rules of engagement)
- [ ] Fire authorization received (if required)
- [ ] Not in no-fire zone (check HUD for zone violation warning)
- [ ] Friendly forces clear (check surroundings, check communications)
- [ ] Weapon loaded and ready
- [ ] Zeroing active (if applicable)
- [ ] Windage set (if applicable for current conditions)
- [ ] Track established (if using tracking)

**IF ANY ITEM CANNOT BE CHECKED, DO NOT FIRE.**

---

### **Engagement Procedure (Step-by-Step)**

#### **STEP 1: Acquire and Track Target**

1. Detect and identify target (Section 5.1)
2. Enter acquisition mode (press Button 4)
3. Frame target in acquisition box (adjust with D-Pad)
4. Request lock-on (press Button 4 again)
5. Wait for Active Lock (green tracking box, "TRACKING" status)
6. Monitor track quality (track confidence >70%, green box)

---

#### **STEP 2: Range Target**

1. Fire Laser Range Finder (LRF trigger - platform-specific button)
2. Wait for range reading (appears on HUD: "RNG: xxxx m")
3. Verify range reasonable (matches expected target distance)
4. Range will be used for ballistics calculations (CCIP reticle)

**Note**: LRF may fire automatically during tracking (platform-dependent configuration). Verify range displayed on HUD.

---

#### **STEP 3: Enable Lead Angle Compensation (If Moving Target)**

**If target is moving** (vehicle, personnel moving laterally):

1. Hold **Dead Man Switch** (Button 3)
2. Press **LAC Toggle** (Button 2)
3. Verify LAC active:
   - "LEAD ANGLE ON" indicator (green)
   - OR "LEAD ANGLE LAG" (yellow - insufficient tracking data, wait)
   - OR "ZOOM OUT" (red - FOV too narrow, zoom out slightly)
4. Observe CCIP reticle offset ahead of target (lead angle applied)
5. CCIP reticle shows where rounds will impact (predicted interception point)

**If target is stationary**:
- LAC not necessary
- CCIP reticle will be at target center (no lead)

**Detailed LAC procedures in Lesson 9.**

---

#### **STEP 4: Final Safety Checks**

1. Verify target still valid
2. Verify tracking still active (green box, good confidence)
3. Check HUD for warnings:
   - ❌ "ZONE VIOLATION" = DO NOT FIRE
   - ❌ "NO-FIRE ZONE" = DO NOT FIRE
   - ✅ No warnings = Clear to fire
4. Verify friendly forces clear
5. Verify backstop (if required)

---

#### **STEP 5: Engage Master Arm**

1. Pull trigger to **Stage 1** (half-pull) - engages **Master Arm** (Button 0)
   - OR toggle Master Arm switch on Control Panel (platform-specific)
2. Verify "ARMED" indicator light ON (red)
3. HUD may display "WEAPON ARMED" status

**⚠️ WEAPON IS NOW HOT - FINGER ON TRIGGER STAGE 1**

---

#### **STEP 6: Fire Weapon**

1. Final aim verification (CCIP reticle on target or lead point)
2. Pull trigger to **Stage 2** (full-pull) - activates **Fire** (Button 5)
3. Weapon fires
4. Hold trigger for desired burst length:
   - **Single Shot**: Quick press and release (weapon fires 1 round)
   - **Burst**: Hold for 2-3 seconds (weapon fires controlled burst)
   - **Sustained**: Hold longer (weapon fires continuously - use cautiously)

**During Firing**:
- Tracking system keeps reticle on target
- Gimbal compensates for recoil
- Observe rounds impacting
- Adjust fire as needed (continue or cease)

---

#### **STEP 7: Cease Fire**

1. Release **Fire button** (Button 5 / trigger stage 2)
2. Weapon stops firing
3. Release **Master Arm** (Button 0 / trigger stage 1) - weapon safe
4. Verify "ARMED" indicator OFF
5. Tracking continues (unless aborted)

---

#### **STEP 8: Assess Target**

1. Observe target effect:
   - Target destroyed?
   - Target damaged?
   - Target suppressed (stopped moving / taking cover)?
   - Missed target (no effect observed)?
2. Decide next action:
   - **Target destroyed**: Stop tracking (double-click Button 4), report success
   - **Target damaged**: Re-engage (repeat Steps 5-7)
   - **Target missed**: Adjust aim (check zeroing, windage, lead angle), re-engage
   - **Target suppressed**: Maintain track, be ready to re-engage

---

#### **STEP 9: Post-Engagement Actions**

1. If target neutralized: Stop tracking (double-click Button 4)
2. Report engagement results to command
3. Update ammunition count (if tracked manually)
4. Scan for additional targets
5. Resume surveillance or follow orders

---

## 5.10 SIMULATION EXERCISES

### **Exercise 5.10.1: Tracking Acquisition Drill**

**Objective**: Practice entering acquisition mode and sizing acquisition box

**Setup**: Instructor displays target on screen (video playback or live camera)

**Procedure**:
1. Identify target on screen
2. Center target with gimbal (joystick)
3. Press Button 4 to enter acquisition mode
4. Observe yellow acquisition box appears
5. Adjust box size with D-Pad to frame target (20% margin)
6. Instructor verifies box sizing
7. Abort tracking (double-click Button 4)
8. Repeat with different targets

**Scoring**:
- Box correctly sized (target framed with appropriate margin)
- Time to complete acquisition setup (<10 seconds)

---

### **Exercise 5.10.2: Full Tracking Sequence (Stationary Target)**

**Objective**: Execute complete tracking sequence from acquisition to active lock

**Setup**: Instructor displays stationary target (video or live)

**Procedure**:
1. Identify and center target
2. Enter acquisition mode (Button 4)
3. Size acquisition box (D-Pad)
4. Request lock-on (Button 4)
5. Wait for Lock Pending → Active Lock transition
6. Verify green tracking box and "TRACKING" status
7. Monitor track for 10 seconds
8. Abort tracking (double-click Button 4)

**Evaluation**:
- Acquisition box correctly sized
- Smooth transition to Active Lock
- Track maintained for 10 seconds (green box, confidence >70%)
- Clean abort

---

### **Exercise 5.10.3: Full Tracking Sequence (Moving Target)**

**Objective**: Track moving target

**Setup**: Instructor displays moving target (video playback or live target if available)

**Procedure**:
1. Identify and center moving target
2. Enter acquisition mode (Button 4)
3. Size acquisition box slightly larger (anticipate motion)
4. Request lock-on (Button 4)
5. Wait for Active Lock
6. Verify tracking follows target as it moves
7. Monitor for 15 seconds
8. Observe gimbal moving automatically to track target
9. Abort tracking (double-click Button 4)

**Evaluation**:
- Successfully locked on moving target
- Track maintained as target moves
- Gimbal automatically following target
- Smooth abort

---

### **Exercise 5.10.4: Simulated Engagement (No Live Fire)**

**Objective**: Execute complete engagement sequence (simulated - no actual firing)

**Setup**: Instructor displays target, provides scenario (e.g., "Enemy vehicle, 500m, moving right to left, you are authorized to engage")

**Procedure**:
1. Identify target (instructor provides identification)
2. Acquire and track target (full sequence from Exercise 5.10.2/5.10.3)
3. Simulate LRF fire (call out "Lasing" - instructor provides range)
4. Enable LAC (if moving target):
   - Hold Button 3 (Dead Man Switch)
   - Press Button 2 (LAC toggle)
   - Verify LAC indicators
5. Final safety checks (call out checks verbally)
6. Engage Master Arm:
   - Pull trigger to stage 1 (Button 0)
   - Call out "MASTER ARM ON"
   - Verify "ARMED" light
7. Simulate firing:
   - Pull trigger to stage 2 (Button 5) BUT DO NOT HOLD
   - Immediately release (no live fire in simulation)
   - Call out "FIRING" (simulated)
   - Hold for 2 seconds (simulated burst)
   - Call out "CEASE FIRE"
8. Release Master Arm (release trigger stage 1)
9. Call out "SAFE"
10. Assess (instructor provides results - e.g., "Target destroyed")
11. Stop tracking (double-click Button 4)
12. Call out "TRACKING STOPPED"

**Evaluation**:
- Correct tracking sequence
- LAC enabled (if applicable)
- Proper safety checks
- Correct firing sequence (simulated)
- Proper cease fire and safe procedures
- Clean tracking abort

---

### **Exercise 5.10.5: Emergency Tracking Abort Drill**

**Objective**: Practice rapid tracking abort

**Setup**: Operator actively tracking target (simulated)

**Procedure**:
1. Start tracking sequence (acquire and lock on target)
2. Instructor calls "ABORT TRACKING - FRIENDLY FORCES" (simulated emergency)
3. Operator immediately double-clicks Button 4 (<500ms)
4. Verify tracking stops, system returns to MANUAL mode
5. Call out "TRACKING ABORTED"

**Scoring**:
- Reaction time (goal: <2 seconds from command to abort complete)
- Correct double-click (tracking stopped, not restarted)
- Verbal confirmation

**Repeat** multiple times until double-click muscle memory established

---

## 5.11 ENGAGEMENT BEST PRACTICES

### **Target Selection**

**Good Targets for Tracking**:
- High contrast against background (e.g., dark vehicle on light desert)
- Clearly defined edges
- Sufficient size (>30 pixels in any dimension)
- Relatively stable motion (not erratic)

**Difficult Targets**:
- Low contrast (camouflaged vehicle in vegetation)
- Very small (distant personnel)
- Erratic motion (evasive maneuvering)
- Partially obscured

**If Tracking Fails**:
- Try manual engagement (no tracking)
- Improve contrast (different camera or LUT)
- Wait for better tracking opportunity

---

### **Lead Angle Compensation Tips**

**When to Use LAC**:
- Target moving laterally (crossing your field of view)
- Target speed >5 m/s (~10 mph)
- Range >100 meters

**When NOT Needed**:
- Stationary targets
- Targets moving directly toward or away from you (radial motion)
- Very close range (<50m)

**LAC Limitations**:
- Requires tracking to be active (can't use LAC without track)
- Requires sufficient FOV (zoomed out enough to measure target velocity)
- Assumes constant target velocity (less accurate if target maneuvering)

---

### **Ammunition Conservation**

**Fire Discipline**:
- Use controlled bursts (2-5 rounds) instead of full-auto spray
- Assess after each burst before re-engaging
- Don't "spray and pray" - precision is key

**Round Count**:
- Track ammunition expenditure
- Report to commander when low
- Conserve rounds for high-priority targets

---

### **Safety Reminders**

**ALWAYS**:
- Verify target before engaging (PID)
- Check for friendly forces
- Verify not in no-fire zone
- Follow rules of engagement
- Have fire authorization (if required)

**NEVER**:
- Fire without positive identification
- Fire into no-fire zones
- Fire if friendly forces potentially in line of fire
- Fire without authorization (if required)
- Assume tracking is infallible (monitor track quality)

---

## LESSON 5 SUMMARY

**Key Points**:
1. Engagement sequence: DETECT → IDENTIFY → ACQUIRE → TRACK → ENGAGE → ASSESS
2. Tracking phases: Off → Acquisition → Lock Pending → Active Lock (→ Coast) → Firing → Off
3. Button 4 (single press): Start acquisition or request lock-on
4. Button 4 (double-click <500ms): Emergency tracking abort
5. D-Pad (Acquisition phase): Adjust acquisition box size (UP/DOWN = height, LEFT/RIGHT = width)
6. Frame target with ~20% margin in acquisition box for best tracking
7. Active Lock: System tracks automatically, operator monitors
8. Coast mode: Temporary target loss, system predicts position and attempts re-acquisition
9. Weapons engagement: Track → Range → LAC (if moving) → Master Arm → Fire → Cease Fire → Assess
10. Double-click Button 4 anytime to abort tracking (emergency stop)

**Skills Practiced**:
- Target acquisition and identification
- Tracking acquisition mode entry
- Acquisition box sizing and positioning
- Lock-on request and Active Lock confirmation
- Track quality monitoring
- Tracking abort (double-click)
- Simulated engagement sequence (complete cycle)
- Lead Angle Compensation activation (basic)
- Safety checks and procedures

**Critical Safety Skills**:
- Positive target identification before engagement
- Verification not in no-fire zone
- Tracking abort on command (emergency response)
- Master Arm and Fire discipline
- Cease fire and weapon safe procedures

**Next Lesson**: Motion Modes & Surveillance Patterns (Manual, Auto Sector Scan, TRP Scan, Radar Slew)

---

**IMPORTANT REMINDERS**:
- Tracking is a tool, not a guarantee - always monitor track quality
- Emergency abort (double-click Button 4) immediately stops tracking - use without hesitation if unsafe
- Never fire without positive identification (PID)
- Practice tracking procedures until they are second nature
- In combat, speed and accuracy save lives - train to proficiency

---
