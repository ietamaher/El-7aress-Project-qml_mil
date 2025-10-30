# LESSON 11: WINDAGE COMPENSATION

**Duration:** 2 hours
**Type:** Practical Skills Training
**Prerequisites:** Lessons 1-10

---

## LESSON OVERVIEW

This lesson covers windage compensation to correct projectile drift caused by crosswind. Students learn to input wind conditions and apply automatic windage corrections to maintain accuracy in windy environments.

### TERMINAL LEARNING OBJECTIVE (TLO)

Upon completion, the student will be able to assess wind conditions, input windage parameters, and engage targets with windage compensation active.

### ENABLING LEARNING OBJECTIVES (ELO)

1. Explain how wind affects projectile trajectory
2. Assess wind direction and speed in the field
3. Perform windage setup procedure (align and set speed)
4. Verify windage compensation is active
5. Clear active windage when conditions change
6. Combine windage with zeroing and LAC

---

## 11.1 WINDAGE FUNDAMENTALS

### 11.1.1 Wind Effect on Projectiles

**Physical Reality:**
- Crosswind pushes projectile left or right during flight
- Longer flight time (range) = more wind drift
- Wind perpendicular to fire direction = maximum effect
- Headwind/tailwind = minimal lateral drift (affects range/drop)

**Diagram:**
```
No Wind:
Weapon → • • • → Target ✓ (hit)

Crosswind (left to right):
Weapon → • • •
              ↘
                → Miss (right of target)

With Windage Correction:
Weapon → • (aimed left)
          ↘ • •
              → Target ✓ (hit, wind compensated)
```

### 11.1.2 Windage Compensation Solution

**System Calculation:**
- **Input:** Wind speed (knots), wind direction (degrees)
- **Calculation:** Angular offset based on wind, range, projectile ballistics
- **Output:** Azimuth correction applied to reticle

**Result:**
- CCIP reticle shifts to compensate wind drift
- Operator aims at reticle (system handles correction)

### 11.1.3 System Storage

**SystemStateData Fields:**
- `windageSpeedKnots` (float, 0-50 knots)
- `windageDirectionDegrees` (float, 0-360°)
- `windageAppliedToBallistics` (bool)
- `windageModeActive` (bool)
- `windageDirectionCaptured` (bool)

**Persistence:** Windage values saved to configuration, but should be updated per mission based on current conditions

---

## 11.2 WIND ASSESSMENT

### 11.2.1 Field Wind Observation

**Visual Indicators:**

| Wind Speed | Visual Cues |
|------------|-------------|
| **0-5 knots** | Calm, smoke rises vertically |
| **5-10 knots** | Light breeze, leaves rustle, flags extended |
| **10-15 knots** | Moderate wind, small branches move, dust raised |
| **15-25 knots** | Fresh wind, small trees sway, whitecaps on water |
| **25+ knots** | Strong wind, large branches move, difficult to walk |

**Tools:**
- Handheld anemometer (wind speed meter)
- Flag observation at known location
- Smoke or dust patterns
- Vegetation movement

### 11.2.2 Wind Direction Determination

**Methods:**

**Flag Method:**
- Observe flag direction (points away from wind source)
- Wind comes FROM the direction flag pole is pointing

**Smoke Method:**
- Observe smoke drift direction
- Wind comes FROM opposite direction smoke travels

**Feel Method:**
- Face into wind (wind on your face)
- Note compass bearing
- Wind direction = bearing you're facing

**Example:**
```
Flag pointing East → Wind from West (270°)
Smoke drifting South → Wind from North (0°/360°)
Facing Northeast to feel wind → Wind from Northeast (45°)
```

### 11.2.3 Wind Speed Estimation

**Beaufort Scale (Simplified):**

```
Knots   | Description        | Field Signs
--------|--------------------|---------------------------------
0-3     | Calm               | Smoke rises vertically
4-6     | Light breeze       | Wind felt on face, leaves rustle
7-10    | Gentle breeze      | Leaves/twigs in motion
11-16   | Moderate breeze    | Small branches move, dust raised
17-21   | Fresh breeze       | Small trees sway
22-27   | Strong breeze      | Large branches move
28+     | Near gale+         | Whole trees move, hard to walk
```

**Anemometer Reading:**
- Most accurate method
- Position anemometer in open area
- Take reading at approximate target altitude if possible
- Record wind speed in knots

---

## 11.3 WINDAGE SETUP PROCEDURE

### 11.3.1 Pre-Setup Requirements

**Environmental Assessment:**
- ✓ Wind speed measured (anemometer or estimated)
- ✓ Wind direction determined (compass bearing)
- ✓ Wind conditions relatively steady (not gusty)
- ✓ Wind speed significant enough to matter (>5 knots)

**System Status:**
- ✓ Station powered and initialized
- ✓ Zeroing already applied (windage adds to zero)
- ✓ Platform stable and operational

### 11.3.2 Complete Windage Procedure

**Step 1: Access Windage Menu**
```
☐ Press MENU button on Control Panel
☐ Navigate to "Windage" option
☐ Press VAL button to enter
☐ Windage setup screen appears
```

**Step 2: Align to Wind Direction**
```
☐ System displays: "Align Weapon Station TOWARDS THE WIND"
☐ Use joystick to rotate gimbal towards wind source
☐ Example: If wind from North, point gimbal North
☐ Alignment doesn't need to be perfect (±5° acceptable)
☐ Press VAL (SELECT) when aligned
```

**Step 3: Capture Wind Direction**
```
☐ System captures current azimuth as wind direction
☐ Example: Gimbal at 045° → Wind direction = 045° stored
☐ Screen transitions to wind speed input
```

**Step 4: Set Wind Speed**
```
☐ System displays: "Set HEADWIND speed"
☐ Current value shown (e.g., "Headwind: 0 knots")
☐ Press UP button to increase speed (+1 knot per press)
☐ Press DOWN button to decrease speed (-1 knot per press)
☐ Range: 0-50 knots
☐ Set to measured or estimated wind speed
☐ Example: Set to 15 knots if wind measured at 15 knots
```

**Step 5: Apply Windage**
```
☐ Press VAL (SELECT) to confirm wind speed
☐ System calculates windage correction
☐ Completion screen shows: "Windage set to XX knots and applied"
☐ "W" indicator appears on OSD (confirms windage active)
☐ Press VAL to return to Main Menu
```

### 11.3.3 Windage Controller States

**State Machine:**
1. **Idle:** No windage setup active
2. **Instruct_AlignToWind:** User aligning gimbal to wind direction
3. **Set_WindSpeed:** User adjusting wind speed value (UP/DOWN)
4. **Completed:** Windage applied, ready to return

---

## 11.4 WINDAGE VERIFICATION

### 11.4.1 Visual Confirmation

**OSD Indicator:**
- "W" appears on On-Screen Display when windage active
- Confirms system applying windage correction
- If "W" not visible, windage not applied

**Reticle Position:**
- CCIP reticle shifts perpendicular to wind direction
- Shift compensates for expected wind drift
- Example: Wind from left → reticle shifts left (aims upwind)

### 11.4.2 Test Fire Verification

**Procedure:**
```
☐ Set up target at 300-500m (wind effect more visible)
☐ Apply windage for current conditions
☐ Aim CCIP pipper at target center
☐ Fire test shot
☐ Observe impact (should be on target center)
☐ If impact offset, reassess wind conditions
```

**Expected Results:**

| Wind Condition | Without Windage | With Windage |
|----------------|-----------------|--------------|
| 10 knot crosswind at 300m | Impact offset 20-30cm | Impact on target |
| 15 knot crosswind at 500m | Impact offset 50-70cm | Impact on target |
| 5 knot crosswind at 200m | Impact offset 5-10cm | Impact on target |

---

## 11.5 CLEAR ACTIVE WINDAGE

### 11.5.1 When to Clear Windage

**Clear Windage When:**
- ❌ Wind conditions significantly changed (speed or direction)
- ❌ Wind dropped below 5 knots (negligible effect)
- ❌ Moving to different operational area (different wind)
- ❌ Mission complete, returning to base
- ❌ Wind now from opposite direction (requires new setup)

### 11.5.2 Clear Windage Procedure

**Access:**
```
☐ Press MENU button
☐ Navigate to "Clear Active Windage"
☐ Press VAL button
☐ System clears windage parameters to 0
☐ "W" indicator disappears from OSD
☐ Return to Main Menu
```

**Result:**
- `windageSpeedKnots = 0.0`
- `windageDirectionDegrees = 0.0`
- `windageAppliedToBallistics = false`
- No windage correction applied to reticle

---

## 11.6 WIND CONDITION CHANGES

### 11.6.1 Monitoring Wind During Operations

**Continuous Assessment:**
- Monitor flags, vegetation, smoke throughout mission
- Watch for changes in wind speed or direction
- Gusts vs. steady wind (steady preferred)

**Indicators of Wind Change:**
- Flag direction shifts >30°
- Wind speed increases/decreases noticeably
- Miss patterns shifting (impacts consistently left/right of aim)

### 11.6.2 Updating Windage

**When Wind Changes Moderately (±5 knots or ±20°):**
```
Option 1: Re-run windage procedure
☐ Access Windage menu
☐ Align to new wind direction
☐ Set new wind speed
☐ Apply updated windage

Option 2: Clear and operate without windage
☐ If wind now minimal (<5 knots)
☐ Clear Active Windage
☐ Continue without compensation
```

**When Wind Changes Significantly (>10 knots or >45°):**
```
☐ Clear Active Windage (old correction now wrong)
☐ Reassess new wind conditions
☐ Perform fresh windage setup
☐ Verify with test fire if possible
```

---

## 11.7 CROSSWIND VS. HEADWIND/TAILWIND

### 11.7.1 Wind Direction Effects

**Crosswind (90° to fire direction):**
- Maximum lateral drift
- Full windage correction required
- Example: Firing North, wind from East or West

**Quartering Wind (45° to fire direction):**
- Moderate lateral drift + minor range effect
- Partial windage correction applied
- Example: Firing North, wind from Northeast or Northwest

**Headwind (0° to fire direction):**
- Minimal lateral drift
- Projectile slows faster (impacts low)
- Windage correction minimal, but ballistic drop increases

**Tailwind (180° to fire direction):**
- Minimal lateral drift
- Projectile slows less (impacts high)
- Windage correction minimal, ballistic drop decreases

### 11.7.2 System Calculation

**The system handles all calculations automatically:**
- Operator inputs wind direction (from alignment)
- Operator inputs wind speed
- System calculates current fire direction (gimbal azimuth)
- System computes angle between wind and fire direction
- System applies proportional correction

**Operator's job:**
- Accurate wind assessment
- Correct alignment to wind source
- Accurate wind speed input
- System does the trigonometry

---

## 11.8 INTEGRATION WITH OTHER SYSTEMS

### 11.8.1 Windage + Zeroing

**Combined Correction:**
```
Total Correction = Zeroing Offset + Windage Offset

Example at 300m:
Zeroing Azimuth: +0.85° (gun-camera boresight)
Windage Azimuth: -0.40° (left crosswind compensation)
Total Azimuth Correction: +0.45°

CCIP reticle shows combined offset
```

**Both corrections are additive and independent**

### 11.8.2 Windage + LAC

**Combined Correction:**
```
Total Correction = Zeroing + Windage + LAC

Example at 500m, moving target, 10 knot crosswind:
Zeroing Azimuth: +0.85°
Windage Azimuth: -0.30°
LAC Azimuth: +2.50° (moving target lead)
Total Azimuth Correction: +3.05°

CCIP pipper shows all three corrections applied
```

**System applies all corrections simultaneously**

### 11.8.3 System Indicators

**OSD Display:**
```
Z W LAC     → All three active
Z W         → Zeroing and Windage active
Z LAC       → Zeroing and LAC active
Z           → Only Zeroing active
(blank)     → No ballistic corrections
```

---

## 11.9 TROUBLESHOOTING WINDAGE ISSUES

### Problem: Impacts Still Drifting with Windage Applied

**Possible Causes:**
- Incorrect wind speed input
- Incorrect wind direction alignment
- Wind changed since windage setup
- Gusty wind (inconsistent)

**Solution:**
```
☐ Reassess wind conditions (may have changed)
☐ Clear Active Windage
☐ Perform fresh windage setup
☐ If wind gusty, wait for steady conditions
☐ Verify test fire at known range
```

---

### Problem: "W" Indicator Not Appearing

**Possible Causes:**
- Windage setup not completed
- Wind speed set to 0 knots
- System cleared windage

**Solution:**
```
☐ Re-run windage procedure
☐ Ensure wind speed set >0 knots
☐ Complete all steps (align, set speed, apply)
☐ Verify "W" appears after completion screen
```

---

### Problem: Cannot Align to Wind Direction

**Possible Causes:**
- Gimbal limits preventing rotation
- Wind direction in restricted zone
- Platform orientation limiting access

**Solution:**
```
☐ Rotate platform if possible
☐ Approximate wind direction (±10° acceptable)
☐ System calculates based on alignment, not perfection
☐ If wind from restricted zone, estimate offset manually
```

---

### Problem: Wind Too Strong for System

**System Limit:** 50 knots maximum input

**If wind >50 knots:**
```
☐ Input maximum 50 knots
☐ Understand correction may be insufficient
☐ Consider delaying engagement if possible
☐ Accept reduced accuracy at long range
☐ Close range if tactically feasible
```

---

## 11.10 PRACTICAL FIELD TECHNIQUES

### 11.10.1 Quick Wind Assessment

**30-Second Wind Check:**
```
☐ Wet finger held up (feel wind direction)
☐ Estimate wind speed from vegetation movement
☐ Quick alignment to wind (don't overthink)
☐ Set estimated speed
☐ Apply windage
☐ Engage
```

**Good enough for most tactical engagements**

### 11.10.2 Wind Bracketing

**If Uncertain About Wind Speed:**
```
☐ Estimate wind speed (e.g., 10 knots)
☐ Fire test shot, observe impact
☐ If impact right of target: wind stronger or direction wrong
☐ Increase wind speed to 15 knots
☐ Fire again, assess
☐ Iterate until impacts centered
```

### 11.10.3 Multi-Direction Engagement

**When Engaging Multiple Targets in Different Directions:**

**Option 1: Single Windage Setup (Fast)**
```
☐ Set windage once for current wind
☐ System automatically adjusts based on fire direction
☐ Engage all targets
☐ Works well if wind steady
```

**Option 2: No Windage, Manual Compensation (Simple)**
```
☐ Clear windage
☐ Use operator experience to aim off target
☐ Example: Crosswind, aim 1 target-width upwind
☐ Faster for experienced operators
```

---

## 11.11 PROFICIENCY EXERCISES

### Exercise 1: Complete Windage Setup (30 min)

**Objective:** Perform full windage procedure

**Procedure:**
```
☐ Measure wind: 12 knots from Northeast (045°)
☐ Access Windage menu
☐ Align gimbal to 045° (Northeast)
☐ Press SELECT (capture direction)
☐ Set wind speed to 12 knots (UP button 12 times)
☐ Press SELECT (apply windage)
☐ Verify "W" indicator on OSD
```

**Performance Standard:** Procedure complete in <5 minutes

---

### Exercise 2: Windage Verification Fire (45 min)

**Objective:** Verify windage accuracy with test fire

**Setup:**
- Target at 300m
- Wind: 10 knots crosswind
- Apply windage

**Procedure:**
```
☐ Fire 3-shot group with windage active
☐ Measure group center offset from target center
☐ Clear windage
☐ Fire 3-shot group without windage
☐ Compare impact locations
☐ Windage group should be centered, no-windage group offset
```

**Performance Standard:** Windage group within 10cm of center

---

### Exercise 3: Wind Condition Change Response (30 min)

**Objective:** Update windage when wind changes

**Scenario:**
```
Initial: Wind 10 knots from North
        → Apply windage, engage targets
Wind Changes: Now 15 knots from Northeast
        → Clear old windage
        → Set new windage (15 knots, NE)
        → Engage targets
```

**Performance Standard:** Detect change and update windage within 3 minutes

---

## 11.12 LESSON SUMMARY

### Key Points

1. **Windage compensates crosswind** pushing projectiles left/right during flight

2. **Two-step procedure:** Align gimbal to wind direction, set wind speed (knots)

3. **Wind assessment:** Use flags, vegetation, anemometer, or estimation

4. **System captures:** Wind direction (degrees), wind speed (0-50 knots)

5. **"W" indicator** appears on OSD when windage active

6. **Clear windage** when conditions change significantly

7. **Crosswind = maximum drift**, headwind/tailwind = minimal lateral effect

8. **Combines with zeroing and LAC:** System applies all corrections automatically

9. **Update windage** when wind changes >5 knots or >20° direction

10. **Test fire verification** recommended at 300-500m range

---

**END OF LESSON 11**
