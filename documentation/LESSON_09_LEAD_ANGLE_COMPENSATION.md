# LESSON 9: LEAD ANGLE COMPENSATION

**Duration:** 4 hours
**Type:** Advanced Fire Control Training
**Prerequisites:** Lessons 1-8

---

## LESSON OVERVIEW

This lesson provides comprehensive training on Lead Angle Compensation (LAC), the fire control system that automatically calculates aim point adjustments for engaging moving targets. Students will understand LAC theory, master activation/deactivation procedures, interpret status indicators, and develop proficiency in moving target engagements.

### TERMINAL LEARNING OBJECTIVE (TLO)

Upon completion of this lesson, the student will be able to effectively employ Lead Angle Compensation to engage moving targets, demonstrating proper LAC activation procedures, correct interpretation of system status indicators, and accurate assessment of when LAC should be used or disabled.

### ENABLING LEARNING OBJECTIVES (ELO)

1. Explain the ballistic principles behind lead angle calculation
2. Activate and deactivate LAC using proper safety procedures
3. Interpret all LAC status indicators (ON, LAG, ZOOM OUT)
4. Recognize when LAC should be enabled or disabled
5. Engage moving targets using LAC-assisted CCIP reticle
6. Troubleshoot LAC warnings and errors
7. Apply LAC limitations to tactical decision-making
8. Perform moving target engagements under various conditions

---

## 9.1 LEAD ANGLE COMPENSATION FUNDAMENTALS

### 9.1.1 The Moving Target Problem

When engaging a stationary target from a stationary platform, the weapon is aimed directly at the target. However, when the target is moving, the projectile's time-of-flight (TOF) means the target will have moved from its original position by the time the projectile arrives.

**Without Lead Compensation:**
```
Time T=0 (Fire)          Time T=TOF (Impact)

Target: [X]              Target: -----> [X]
         ↓
Bullet:  •               Bullet:         •

Result: MISS (bullet hits where target WAS)
```

**With Lead Compensation:**
```
Time T=0 (Fire)          Time T=TOF (Impact)

Target: [X]              Target: -----> [X]
         ↓ (aim ahead)                   ↓
Bullet:  →→→ •           Bullet:         •

Result: HIT (bullet meets target at predicted position)
```

### 9.1.2 Lead Angle Definition

**Lead Angle:** The angular offset between the target's current position and the predicted intercept point.

**Calculated Lead Angle Components:**
- **Azimuth Lead:** Horizontal angular offset (degrees)
- **Elevation Lead:** Vertical angular offset (degrees)

**Factors Affecting Lead Angle:**
1. **Target Velocity:** Faster target = more lead required
2. **Target Direction:** Crossing target = maximum lead, approaching/receding = minimal lead
3. **Range to Target:** Greater range = longer TOF = more lead required
4. **Projectile Velocity:** Slower projectile = longer TOF = more lead required
5. **Target Angular Rate:** How fast target crosses your field of view

### 9.1.3 LAC Calculation Process

The system performs continuous real-time calculations:

**Step 1: Measure Target Motion**
- Tracking system provides target angular rates:
  - Azimuth rate (degrees/second)
  - Elevation rate (degrees/second)

**Step 2: Determine Range**
- Laser Range Finder measures distance to target (meters)

**Step 3: Calculate Time-of-Flight**
- Based on range, projectile velocity, and ballistic trajectory
- Accounts for gravity drop during flight

**Step 4: Predict Target Position**
- Current position + (angular rate × TOF) = predicted position

**Step 5: Calculate Lead Angle**
- Angular difference between current aim point and predicted intercept point

**Step 6: Apply Offset to Reticle**
- CCIP reticle shifts to show lead-compensated aim point
- Operator aims at reticle, system handles the math

**Update Rate:** 30 Hz (calculations updated 30 times per second)

### 9.1.4 System Components

**BallisticsProcessor:**
- Core calculation engine
- Function: `calculateLeadAngle()`
- Inputs: range, target angular rates, muzzle velocity, TOF estimate, camera FOV
- Output: Lead angles (Az, El) and status

**WeaponController:**
- Manages fire control solution
- Function: `updateFireControlSolution()`
- Applies lead angles to reticle positioning

**SystemStateModel:**
- Stores LAC state: ON/OFF
- Flag: `leadAngleCompensationActive`
- Updated via: `setLeadAngleCompensationActive(bool)`

**CCIP Reticle:**
- Visual display of calculated aim point
- Shows "LAC" bracket when compensation active
- Pipper position includes lead offset

---

## 9.2 LAC ACTIVATION AND DEACTIVATION

### 9.2.1 Activation Requirements

**Prerequisites for LAC Activation:**
1. ✓ Active target track established (Tracking Phase = Active Lock)
2. ✓ Valid range data from Laser Range Finder
3. ✓ Target exhibiting motion (angular rate > threshold)
4. ✓ Sufficient camera Field of View (not zoomed in excessively)
5. ✓ System initialized and operational

**Safety Interlock:**
- **Dead Man Switch (Button 3) MUST be held** during activation

### 9.2.2 Activation Procedure

**Step-by-Step Activation:**

```
PRE-CONDITIONS:
☐ Target tracked (Active Lock achieved)
☐ Target moving at measurable velocity
☐ Range data valid (LRF successful)
☐ Camera FOV adequate (avoid max zoom)

ACTIVATION:
☐ Hold Button 3 (Dead Man Switch) with left hand
☐ Keep Button 3 held continuously
☐ Press Button 2 (LAC Toggle) with right hand
☐ Release Button 2 (toggle complete)
☐ Release Button 3 (Dead Man Switch)

VERIFICATION:
☐ "LEAD ANGLE ON" indicator appears (GREEN text)
☐ "LAC" bracket appears on CCIP reticle
☐ Reticle pipper shifts to lead position (ahead of target)
☐ Confidence bar remains GREEN (>70%)
```

**Blocked Activation:**

If Dead Man Switch (Button 3) is NOT held:
- System logs: "Cannot toggle Lead Angle Compensation"
- LAC state does not change
- No visual indicators appear
- Safety interlock prevents accidental activation

### 9.2.3 Deactivation Procedure

**When to Deactivate:**
- Target stops moving
- Lost track of target
- "ZOOM OUT" warning appears (FOV insufficient)
- "LEAD ANGLE LAG" persists (tracking data poor)
- Engagement complete
- Switching to stationary target

**Step-by-Step Deactivation:**

```
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (LAC Toggle)
☐ Release Button 2
☐ Release Button 3

VERIFICATION:
☐ "LEAD ANGLE ON" indicator disappears
☐ "LAC" bracket removed from CCIP reticle
☐ Reticle pipper returns to boresight alignment
☐ System ready for stationary target engagement
```

### 9.2.4 Automatic LAC Behavior

**System Does NOT Automatically:**
- Enable LAC when target moves (operator decision required)
- Disable LAC when target stops (operator must manually disable)
- Adjust LAC based on target velocity changes (calculations update continuously, but state remains)

**System DOES Automatically:**
- Update lead calculations 30 times per second when LAC enabled
- Display warnings if calculation quality degrades
- Continue tracking even if LAC disabled

---

## 9.3 LAC STATUS INDICATORS

### 9.3.1 "LEAD ANGLE ON" (Green)

**Appearance:** Solid green text on display

**Meaning:**
- ✓ LAC successfully enabled
- ✓ Lead angle calculations active
- ✓ Valid tracking data available
- ✓ Sufficient camera FOV for calculation
- ✓ CCIP reticle includes lead offset

**Operator Action:**
- Continue engagement
- Maintain tracking on target
- Monitor reticle position
- Fire when ready

**Display Elements When "LEAD ANGLE ON":**
- Green "LEAD ANGLE ON" text (top or side of display)
- "LAC" bracket on CCIP reticle (L-shaped corners)
- Pipper offset from target center (showing lead)
- Confidence bar (should remain GREEN)

---

### 9.3.2 "LEAD ANGLE LAG" (Yellow)

**Appearance:** Yellow warning text on display

**Meaning:**
- ⚠ LAC is enabled, but calculation quality is degraded
- ⚠ Tracking data insufficient or inconsistent
- ⚠ Target motion unpredictable
- ⚠ Lead angle calculation has high uncertainty
- ⚠ Hit probability reduced

**Common Causes:**
1. **Tracking Quality Poor:**
   - Target partially obscured
   - Target size very small
   - Erratic target motion
   - Tracking gate losing lock intermittently

2. **Insufficient Motion Data:**
   - Target just started moving (need 1-2 seconds of data)
   - Target velocity near zero (below detection threshold)
   - Tracking box resized recently (motion history reset)

3. **Range Data Issues:**
   - LRF measurement inconsistent
   - Target range changing rapidly
   - Environmental interference (rain, fog, smoke)

4. **System Processing Load:**
   - Multiple simultaneous calculations
   - Temporary processing delay

**Operator Action:**

**Option 1: Wait for Improvement**
```
☐ Maintain steady track on target
☐ Wait 2-3 seconds for system to gather more data
☐ Monitor if "LAG" warning clears to "ON"
☐ If clears to green, proceed with engagement
```

**Option 2: Re-Lase Target**
```
☐ Press LRF button on Control Panel
☐ Obtain fresh range measurement
☐ Check if "LAG" warning clears
```

**Option 3: Disable LAC**
```
☐ If "LAG" persists >5 seconds
☐ Hold Button 3 + Press Button 2 (disable LAC)
☐ Engage using standard methods (no lead compensation)
☐ Accept lower hit probability on moving target
```

**DO NOT:**
- Fire with "LAG" warning unless critically necessary
- Ignore persistent "LAG" warning
- Assume lead calculation is accurate during "LAG"

---

### 9.3.3 "ZOOM OUT" (Red)

**Appearance:** Red warning text on display

**Meaning:**
- ❌ LAC calculation CANNOT be performed
- ❌ Camera Field of View too narrow
- ❌ Tracking gate too small for velocity estimation
- ❌ Lead angle calculation mathematically invalid
- ❌ LAC disabled or providing zero compensation

**Technical Explanation:**

Lead angle calculation requires measuring target motion across the camera sensor. When zoomed in excessively:
- Target occupies large portion of FOV
- Small movements appear as large pixel shifts
- Angular rate calculation becomes unreliable
- System cannot distinguish target motion from tracking error

**Threshold:**
- Typically occurs at >15x zoom (varies by camera)
- Exact threshold depends on target size and range

**Operator Action:**

**Immediate Response:**
```
☐ Press Button 8 (Zoom Out)
☐ Hold until "ZOOM OUT" warning clears
☐ Stop when "LEAD ANGLE ON" (green) appears
☐ Verify "LAC" bracket visible on reticle
☐ Maintain track during zoom adjustment
```

**If Zoom Out Not Acceptable:**
```
If target identification requires high magnification:

Option 1: Disable LAC, Engage Without Lead
☐ Hold Button 3 + Press Button 2 (disable LAC)
☐ Accept manual lead requirement
☐ Aim ahead of target using operator judgment

Option 2: Zoom Out Temporarily for LAC
☐ Zoom out until LAC valid
☐ Enable LAC (green status)
☐ Allow LAC to stabilize (2-3 seconds)
☐ Engage immediately before zooming back in
```

**DO NOT:**
- Attempt engagement with "ZOOM OUT" active (LAC not functioning)
- Ignore red warning
- Assume system is compensating for lead during "ZOOM OUT"

---

### 9.3.4 No LAC Indicator

**Appearance:** No LAC-related text on display

**Meaning:**
- LAC is disabled (OFF state)
- System not calculating lead angles
- CCIP reticle shows boresight aim point only
- Suitable for stationary targets

**Operator Action:**
- Normal operation for stationary targets
- Enable LAC if target begins moving
- No action required if intentionally disabled

---

### 9.3.5 Status Indicator Summary Table

| Indicator | Color | LAC State | Calculation Quality | Operator Action |
|-----------|-------|-----------|---------------------|-----------------|
| **LEAD ANGLE ON** | 🟢 Green | Enabled | Valid | ✓ Clear to fire |
| **LEAD ANGLE LAG** | 🟡 Yellow | Enabled | Degraded | ⚠ Wait or disable |
| **ZOOM OUT** | 🔴 Red | Invalid | Cannot calculate | ❌ Zoom out required |
| *(no indicator)* | - | Disabled | N/A | Enable if needed |

---

## 9.4 CCIP RETICLE WITH LAC

### 9.4.1 Reticle Components Review

**Standard CCIP Reticle Elements:**
- **Pipper (⊙):** Predicted impact point (center circle + dot)
- **FPV (__|__):** Flight Path Vector (platform motion indicator)
- **Range Scale:** Distance reference markers
- **Confidence Bar:** Solution reliability indicator

**Additional Element with LAC Active:**
- **LAC Bracket (┌─):** L-shaped corner brackets framing the reticle

### 9.4.2 LAC Bracket Appearance

**Visual Design:**
```
        ┌─────────────┐
        │             │
        │      ⊙      │  ← Pipper with lead offset
        │             │
        └─────────────┘

        LAC corner brackets
```

**Bracket Characteristics:**
- Four L-shaped corners at reticle edges
- Same color as reticle (typically green or HUD color)
- Only visible when "LEAD ANGLE ON" (green status)
- Disappears if LAC disabled or warnings active

**Purpose:**
- Quick visual confirmation LAC is active
- Distinguishes LAC-compensated aim from standard aim
- Reminds operator that lead is being applied

### 9.4.3 Pipper Position with LAC

**Without LAC (Stationary Target):**
```
        Target
          [X]
           ↓
        Reticle Pipper ⊙

Pipper aligned with target center
```

**With LAC (Moving Target - Rightward Motion):**
```
        Target
          [X] ────→ (moving right)

        Reticle Pipper ⊙
                     ↑
                Lead offset applied

Pipper ahead of target (to the right)
Bullet will intercept target at predicted position
```

**Key Understanding:**
- **Aim pipper at target's CURRENT position, NOT ahead of it**
- System has already calculated and applied the lead
- Pipper appears offset from target center - this is correct
- Do NOT manually lead additional distance

### 9.4.4 Pipper Behavior During Engagement

**Target Moving at Constant Velocity:**
- Pipper maintains steady offset from target
- Offset distance proportional to target speed
- Smooth tracking keeps pipper on target

**Target Accelerating/Decelerating:**
- Pipper offset adjusts dynamically
- May see slight lag during rapid velocity changes
- System updates calculations 30 Hz

**Target Changing Direction:**
- Pipper offset shifts to new lead position
- Brief adjustment period (0.5-1.0 seconds)
- May trigger "LAG" warning during abrupt changes

**Crossing Target (Maximum Lead):**
```
Target moving perpendicular to line of sight:

        [X]
         ↓
         → → → (crossing left to right)

         ⊙ (pipper offset right)

Maximum lead angle required
```

**Approaching Target (Minimum Lead):**
```
Target moving toward you:

        [X]
         ↓
         ↓ (approaching)

        ⊙ (pipper near center)

Minimal lead angle required
```

---

## 9.5 MOVING TARGET ENGAGEMENT PROCEDURES

### 9.5.1 Standard Moving Target Engagement

**Complete Procedure:**

```
PHASE 1: DETECTION & IDENTIFICATION
☐ Detect moving target using wide FOV
☐ Slew gimbal to center target in display
☐ Zoom in for positive identification (Button 6)
☐ Confirm target is hostile/valid
☐ Assess target velocity and direction

PHASE 2: TRACKING ACQUISITION
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 4 (Enter Acquisition)
☐ Adjust tracking box with D-Pad (20-30% larger than target)
☐ Press Button 4 (Request Lock)
☐ Wait for Active Lock confirmation
☐ Release Button 3

PHASE 3: RANGE DETERMINATION
☐ Press LRF button on Control Panel
☐ Verify range displayed on Range Scale
☐ Check range bracket highlights correctly
☐ Re-lase if range data questionable

PHASE 4: LAC ACTIVATION
☐ Assess target velocity (>5 m/s recommended for LAC)
☐ Check camera zoom level (avoid max zoom)
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (Enable LAC)
☐ Release Button 2, then Button 3
☐ Verify "LEAD ANGLE ON" (green) appears
☐ Observe "LAC" bracket on reticle
☐ Watch pipper shift to lead position

PHASE 5: TRACKING STABILIZATION
☐ Maintain smooth tracking on target
☐ Keep target centered in tracking gate
☐ Monitor confidence bar (should be GREEN >70%)
☐ Watch for LAC warnings (LAG or ZOOM OUT)
☐ Allow 2-3 seconds for LAC to stabilize

PHASE 6: FINAL CHECKS
☐ "LEAD ANGLE ON" (green) confirmed
☐ Confidence bar GREEN
☐ No LAC warnings present
☐ Pipper showing stable lead offset
☐ Target still positively identified
☐ No friendlies in area
☐ Clear backstop or safe impact area

PHASE 7: ENGAGEMENT
☐ Hold Button 0 (Master Arm)
☐ Verify "MASTER ARM" indicator
☐ Track smoothly to keep pipper on target
☐ Press Button 5 (Fire)
☐ Hold fire for burst duration
☐ Release Button 5 (Cease Fire)
☐ Release Button 0 (Disarm)

PHASE 8: ASSESSMENT
☐ Observe impact on target
☐ Assess hit/miss and damage
☐ If miss: analyze pipper position during shot
☐ If target still moving: re-engage or re-assess
☐ If target stopped: disable LAC for follow-up shots

PHASE 9: POST-ENGAGEMENT
☐ Hold Button 3 + Press Button 2 (Disable LAC)
☐ Double-click Button 4 (Stop Tracking)
☐ Return to surveillance or home position
☐ Log engagement details
```

### 9.5.2 Rapid Moving Target Engagement

**When to Use:** Time-critical engagement, immediate threat

**Abbreviated Procedure:**

```
☐ Slew to target, initiate tracking (Button 3+4)
☐ Quick box size adjustment (D-Pad, 2-3 presses)
☐ Button 4 (Lock), wait for Active Lock
☐ LRF target immediately
☐ Button 3 + Button 2 (Enable LAC) - simultaneous
☐ Verify green "LEAD ANGLE ON" (1-2 seconds)
☐ Button 0 (Master Arm) + Button 5 (Fire) - immediate
☐ Disengage after target prosecuted
```

**Time Savings:** 5-8 seconds vs. methodical procedure

**Risk:** Less verification, requires high operator proficiency

---

## 9.6 LAC LIMITATIONS AND CONSIDERATIONS

### 9.6.1 Maximum Lead Angle Limit

**System Limit:** 10.0 degrees (configurable)

**Reason for Limit:**
- Prevents unrealistic lead calculations
- Protects against erroneous sensor data
- Indicates target velocity beyond engagement envelope

**What Happens at Limit:**
- LAC calculations capped at 10°
- "LEAD ANGLE LAG" warning may appear
- Hit probability significantly reduced

**Operator Response:**
```
If target requiring >10° lead:
☐ Target is moving too fast for reliable engagement
☐ Consider: Wait for target to slow/stop
☐ Consider: Close range to reduce lead requirement
☐ Consider: Engage with understanding of low hit probability
☐ DO NOT engage if friendly forces downrange
```

### 9.6.2 Field of View Restrictions

**Problem:** Excessive zoom prevents velocity calculation

**Why It Matters:**
- Target motion measured in pixels across sensor
- High zoom = fewer pixels available for motion detection
- System cannot distinguish motion from tracking error

**Practical Guidelines:**
```
Camera Zoom Level → LAC Status
1x - 10x (wide)   → ✓ LAC fully functional
10x - 15x         → ✓ LAC functional (marginal)
15x - 20x (max)   → ❌ "ZOOM OUT" warning likely
```

**Solution:**
- Engage at moderate zoom (5x-12x optimal)
- If max zoom required for ID, zoom out before engagement
- Balance target identification needs vs. LAC functionality

### 9.6.3 Range Limitations

**Short Range (<200m):**
- Very short time-of-flight
- Lead angle minimal even for fast targets
- LAC may provide negligible benefit
- Tracking at close range is challenging

**Optimal Range (200m - 1500m):**
- LAC provides maximum benefit
- TOF significant enough to require lead
- Tracking stable and reliable

**Long Range (>1500m):**
- Long TOF increases lead requirement
- Environmental factors more significant
- Confidence may degrade
- LAC calculations include larger uncertainty

### 9.6.4 Environmental Factors

**Wind Effects:**
- Current system may not account for crosswind
- Operator must assess wind impact manually
- LAC compensates for target motion only, not projectile drift

**Atmospheric Conditions:**
- Fog, rain, snow degrade tracking quality
- LRF may have difficulty at distance
- "LEAD ANGLE LAG" more common in poor weather

**Thermal Conditions:**
- Thermal camera tracking may be superior in darkness
- Thermal tracking may struggle with low-contrast targets
- LUT selection affects tracking quality

### 9.6.5 Target Motion Complexity

**Constant Velocity (Best Case):**
- LAC calculates accurately
- Green "LEAD ANGLE ON" status
- High hit probability

**Accelerating/Deceleating (Moderate):**
- LAC updates continuously
- May show brief "LAG" warnings
- Moderate hit probability

**Erratic Motion (Worst Case):**
- LAC cannot predict unpredictable motion
- Persistent "LEAD ANGLE LAG"
- Low hit probability
- Consider disabling LAC

**Maneuvering Targets:**
- Aircraft, fast vehicles performing evasive maneuvers
- LAC provides aim point for current trajectory
- If target abruptly changes course, LAC prediction invalid
- Operator must anticipate maneuver or wait for stable track

### 9.6.6 System Latency

**Calculation Update Rate:** 30 Hz (every 33 milliseconds)

**Display Update Rate:** 60 FPS (every 16 milliseconds)

**Total System Latency:** ~50-100 milliseconds sensor-to-display

**Impact:**
- For very fast targets, slight lag exists
- System generally compensates well
- Operator should maintain smooth tracking

### 9.6.7 Ammunition Considerations

**High Velocity Ammunition:**
- Shorter time-of-flight
- Less lead required
- LAC more accurate

**Low Velocity Ammunition:**
- Longer time-of-flight
- More lead required
- LAC accuracy depends on good velocity data

**System Assumption:**
- Current muzzle velocity must be accurate
- If ammunition type changed, system may need recalibration

---

## 9.7 TROUBLESHOOTING LAC ISSUES

### 9.7.1 "LAC Won't Enable" (Button 2 No Effect)

**Symptoms:**
- Button 2 pressed, no "LEAD ANGLE ON" appears
- No LAC bracket on reticle
- System logs error message

**Possible Causes & Solutions:**

**Cause 1: Dead Man Switch Not Held**
```
Check: Was Button 3 held while pressing Button 2?
Solution:
☐ Hold Button 3 (Dead Man Switch) firmly
☐ While holding, press Button 2
☐ Verify Dead Man Switch active (indicator on Control Panel)
```

**Cause 2: No Active Track**
```
Check: Is tracking phase "Active Lock"?
Solution:
☐ Verify tracking gate visible on display
☐ Check tracking status: should be "ACTIVE LOCK"
☐ If not tracking, initiate tracking first
☐ Then enable LAC
```

**Cause 3: Station Not Powered**
```
Check: Is station enabled?
Solution:
☐ Check Control Panel power indicator
☐ Verify station ON switch engaged
☐ System must be fully initialized
```

---

### 9.7.2 "Persistent LAG Warning"

**Symptoms:**
- "LEAD ANGLE LAG" (yellow) appears and persists >5 seconds
- LAC enabled but calculation quality poor

**Possible Causes & Solutions:**

**Cause 1: Poor Tracking Quality**
```
Check: Is target clear and well-tracked?
Solution:
☐ Verify target clearly visible in tracking gate
☐ Ensure tracking box properly sized (not too large/small)
☐ Maintain smooth, steady tracking motion
☐ Avoid jerky joystick inputs
☐ If target partially obscured, wait for clear line of sight
```

**Cause 2: Insufficient Range Data**
```
Check: When was last LRF measurement?
Solution:
☐ Press LRF button on Control Panel (re-lase)
☐ Verify range displays on Range Scale
☐ Confirm range bracket highlights
☐ If LRF fails, target may be beyond range or obscured
```

**Cause 3: Target Velocity Too Low**
```
Check: Is target actually moving?
Solution:
☐ Observe target - is motion visible?
☐ If target stopped, disable LAC (not needed)
☐ If target moving slowly (<2 m/s), LAC may struggle
☐ Consider disabling LAC for very slow targets
```

**Cause 4: Target Just Started Moving**
```
Check: Did target recently start moving?
Solution:
☐ Wait 2-3 seconds for system to gather motion data
☐ System needs velocity history for accurate calculation
☐ "LAG" should clear to "ON" within 3 seconds
```

---

### 9.7.3 "Immediate ZOOM OUT Warning"

**Symptoms:**
- Enable LAC, immediately see "ZOOM OUT" (red)
- LAC bracket does not appear

**Possible Causes & Solutions:**

**Cause: Camera Zoomed In Too Far**
```
Check: What is current zoom level?
Solution:
☐ Press Button 8 (Zoom Out)
☐ Hold until "ZOOM OUT" warning clears
☐ When "LEAD ANGLE ON" (green) appears, stop
☐ If target identification requires high zoom:
   - Identify target first at high zoom
   - Zoom out to enable LAC
   - Engage immediately
```

---

### 9.7.4 "Pipper Not Showing Lead"

**Symptoms:**
- "LEAD ANGLE ON" (green) displayed
- But pipper appears centered on target (no offset)

**Possible Causes & Solutions:**

**Cause 1: Target Moving Toward/Away (Not Crossing)**
```
Check: Target direction relative to line of sight
Explanation:
☐ Target approaching/receding requires minimal lead
☐ Pipper offset may be very small (1-2 pixels)
☐ This is normal - lead still being applied
☐ Crossing targets show maximum lead offset
```

**Cause 2: Target Moving Slowly**
```
Check: Target velocity
Explanation:
☐ Slow targets (<5 m/s) require small lead angles
☐ Pipper offset may be barely visible
☐ LAC is functioning, but lead is small
☐ This is correct behavior
```

**Cause 3: Short Range**
```
Check: Range to target
Explanation:
☐ Close range (<300m) = short TOF
☐ Short TOF = minimal lead required
☐ Pipper offset will be small
☐ LAC functioning correctly
```

---

### 9.7.5 "LAC Bracket Flickers On/Off"

**Symptoms:**
- LAC bracket appears and disappears rapidly
- Status alternates between "ON" and "LAG"

**Possible Causes & Solutions:**

**Cause 1: Marginal Tracking**
```
Check: Tracking stability
Solution:
☐ Improve tracking smoothness
☐ Ensure target well-centered in gate
☐ Avoid excessive zoom
☐ Resize tracking box if needed (abort and restart)
```

**Cause 2: Target Velocity Near Threshold**
```
Check: Target speed
Solution:
☐ Target may be speeding up / slowing down
☐ System detects motion, then no motion, repeatedly
☐ If flickering persists, disable LAC
☐ Engage manually without lead compensation
```

---

### 9.7.6 "Hit Miss Despite Green LAC Status"

**Symptoms:**
- "LEAD ANGLE ON" (green), confidence GREEN
- Fired with pipper on target
- Projectile missed target

**Analysis & Solutions:**

**Cause 1: Target Maneuvered During TOF**
```
Explanation:
☐ LAC predicted straight-line motion
☐ Target changed course after firing
☐ Not a system error - unpredictable target behavior
Solution:
☐ Re-engage if target stabilizes
☐ Anticipate maneuvers if possible
```

**Cause 2: Wind Drift Not Compensated**
```
Explanation:
☐ LAC compensates target motion, not projectile drift
☐ Strong crosswind may cause miss
Solution:
☐ Assess wind conditions
☐ Consider windage correction (Windage menu)
☐ Apply manual compensation if trained
```

**Cause 3: Ammunition Mismatch**
```
Explanation:
☐ System calibrated for specific ammunition type
☐ Different ammunition = different muzzle velocity
☐ TOF calculation incorrect, lead angle wrong
Solution:
☐ Verify correct ammunition loaded
☐ System may require ballistic recalibration
☐ Consult armorer if ammunition changed
```

**Cause 4: Operator Tracking Error**
```
Explanation:
☐ Operator moved joystick during firing
☐ Pipper drifted off target at moment of fire
Solution:
☐ Practice smooth tracking
☐ Hold stick steady during trigger press
☐ Use gentle stick inputs to maintain pipper position
```

---

## 9.8 LAC DECISION MATRIX

### 9.8.1 When to Enable LAC

**DEFINITELY ENABLE LAC:**
- ✓ Target moving at >10 m/s
- ✓ Target crossing line of sight
- ✓ Range >500m (longer TOF)
- ✓ High-value target requiring precision hit
- ✓ Stable track established
- ✓ Good visibility conditions

**CONSIDER ENABLING LAC:**
- ⚠ Target moving 5-10 m/s
- ⚠ Target moving at oblique angle
- ⚠ Range 200-500m
- ⚠ Moderate tracking quality
- ⚠ Acceptable weather conditions

**DO NOT ENABLE LAC:**
- ❌ Target stationary or <5 m/s
- ❌ Target approaching/receding (minimal lead needed)
- ❌ Range <200m (TOF too short)
- ❌ Maximum zoom required for engagement
- ❌ Poor tracking quality (persistent "LAG")
- ❌ Erratic target motion (unpredictable)

### 9.8.2 When to Disable LAC

**DISABLE IMMEDIATELY:**
- ❌ "ZOOM OUT" warning appears and cannot zoom out
- ❌ Persistent "LAG" warning (>5 seconds)
- ❌ Target stopped moving
- ❌ Lost track of target
- ❌ Confidence bar drops to YELLOW or RED
- ❌ Switching to different target (stationary)

**CONSIDER DISABLING:**
- ⚠ Engagement time-critical and LAC not stabilizing quickly
- ⚠ Environmental conditions degrading (fog, rain)
- ⚠ Operator more confident in manual lead estimation

---

## 9.9 ADVANCED LAC TECHNIQUES

### 9.9.1 Pre-Emptive LAC Activation

**Technique:** Enable LAC before target starts moving

**Scenario:** Target currently stationary but expected to move

**Procedure:**
```
☐ Establish track on stationary target
☐ Enable LAC (will show "LAC" bracket but minimal offset)
☐ When target begins moving, LAC already active
☐ System immediately begins calculating lead
☐ No delay for LAC activation during engagement
```

**Advantage:** Faster reaction when target moves unexpectedly

**Risk:** May distract operator if target remains stationary

---

### 9.9.2 LAC with Windage Correction

**Technique:** Combine LAC with manual windage offset

**Scenario:** Moving target in strong crosswind conditions

**Procedure:**
```
☐ Establish track and enable LAC (compensates target motion)
☐ Access Main Menu → Windage
☐ Apply windage correction for crosswind
☐ System combines LAC (target motion) + Windage (wind drift)
☐ CCIP reticle shows total combined offset
☐ Engage normally, aiming at pipper
```

**Advanced Skill:** Requires understanding of both LAC and windage

---

### 9.9.3 Predictive Engagement

**Technique:** Fire just before target enters open area

**Scenario:** Target moving behind cover, about to emerge

**Procedure:**
```
☐ Track target while visible, enable LAC
☐ Target moves behind cover (brief loss of visual)
☐ System maintains track in "Coast" mode
☐ Predict when target will emerge
☐ Fire just before emergence
☐ Projectile intercepts target as it exits cover
```

**Expert Skill:** Requires excellent timing and tracking proficiency

**Risk:** High miss probability, only use when tactically necessary

---

## 9.10 PROFICIENCY EXERCISES

### Exercise 1: LAC Activation Drill (15 min)

**Objective:** Develop muscle memory for LAC enable/disable

**Procedure:**
```
☐ Track target (moving or stationary)
☐ Instructor calls "ENABLE LAC"
☐ Student: Hold Button 3, Press Button 2, Release both
☐ Verify "LEAD ANGLE ON" within 2 seconds
☐ Instructor calls "DISABLE LAC"
☐ Student: Hold Button 3, Press Button 2, Release both
☐ Verify indicator disappears within 1 second
☐ Repeat 20 times until reflexive
```

**Performance Standard:** <2 seconds activation/deactivation time

---

### Exercise 2: Status Indicator Recognition (15 min)

**Objective:** Instantly recognize LAC status and respond appropriately

**Procedure:**
```
☐ Instructor simulates various LAC states on display
☐ Shows: "LEAD ANGLE ON" (green)
☐ Student responds: "Green status, cleared to fire"
☐ Shows: "LEAD ANGLE LAG" (yellow)
☐ Student responds: "Yellow LAG, wait or disable"
☐ Shows: "ZOOM OUT" (red)
☐ Student responds: "Red ZOOM OUT, must zoom out"
☐ Shows: No indicator
☐ Student responds: "LAC disabled, enable if target moving"
☐ Repeat with randomized sequence
```

**Performance Standard:** 100% correct responses, <1 second reaction

---

### Exercise 3: Moving Target Engagement (30 min)

**Objective:** Engage moving targets using LAC

**Setup:**
- Simulated targets moving at 10 m/s crossing left-to-right
- Range: 800m
- Weather: Clear

**Procedure:**
```
☐ Acquire and track moving target
☐ Enable LAC per standard procedure
☐ Verify "LEAD ANGLE ON" (green)
☐ Observe pipper lead offset (should be ahead of target)
☐ Engage when confidence GREEN
☐ Assess hit/miss
☐ Repeat with 10 different targets
```

**Performance Standard:**
- Expert: >80% hit rate
- Proficient: 60-80% hit rate
- Needs Practice: <60% hit rate

---

### Exercise 4: LAC Warning Response (20 min)

**Objective:** Correctly respond to LAC warnings

**Procedure:**
```
Scenario 1: "LAG" Warning
☐ Enable LAC on moving target
☐ Instructor degrades tracking (simulates poor conditions)
☐ "LEAD ANGLE LAG" appears
☐ Student response: Wait 3 seconds, re-lase, or disable
☐ Evaluate student decision

Scenario 2: "ZOOM OUT" Warning
☐ Enable LAC on moving target
☐ Zoom in to maximum magnification (Button 6)
☐ "ZOOM OUT" appears
☐ Student response: Zoom out (Button 8) until warning clears
☐ Verify green status returns

Scenario 3: Status Changes During Engagement
☐ Engage moving target with LAC
☐ Mid-engagement, warning appears
☐ Student response: Abort or continue based on situation
☐ Evaluate decision-making
```

**Performance Standard:** Correct response to all warnings within 2 seconds

---

### Exercise 5: LAC Decision-Making (25 min)

**Objective:** Determine when LAC should/should not be used

**Procedure:**
```
Instructor presents scenarios, student decides: Enable LAC or Not

Scenario 1: Target moving 15 m/s, crossing, 1000m, clear weather
Student: ENABLE LAC (fast crossing target, good conditions)

Scenario 2: Target moving 3 m/s, approaching, 300m
Student: DO NOT ENABLE LAC (slow, approaching, short range)

Scenario 3: Target moving 12 m/s, 1500m, heavy fog
Student: CONSIDER or NOT (moving fast, but poor visibility)

Scenario 4: Target moving 8 m/s, 700m, max zoom required for ID
Student: DO NOT ENABLE LAC (zoom restriction)

Scenario 5: Target stationary now, but previously moving 20 m/s
Student: DO NOT ENABLE LAC (target currently stationary)

☐ Present 15 scenarios
☐ Student justifies each decision
☐ Instructor evaluates reasoning
```

**Performance Standard:** >90% correct decisions with sound reasoning

---

### Exercise 6: Combined Skills Integration (45 min)

**Objective:** Full engagement sequence with all skills

**Complex Scenarios:**

**Scenario A: Vehicle Convoy**
```
☐ Multiple vehicles moving 25 m/s at 1200m
☐ Prioritize lead vehicle
☐ Track, enable LAC, engage
☐ Rapidly re-target second vehicle
☐ Keep LAC enabled, re-engage
☐ Assess both vehicles
```

**Scenario B: Evasive Target**
```
☐ Target moving 18 m/s, then stops, then moves again
☐ Enable LAC while moving
☐ Target stops - consider disabling LAC
☐ Target moves again - re-enable LAC
☐ Engage during stable motion period
```

**Scenario C: Long Range Moving Target**
```
☐ Target at 1800m moving 10 m/s
☐ Enable LAC
☐ "LAG" warning appears (range/visibility)
☐ Decide: Wait for improvement or close range
☐ Execute decision
```

**Performance Evaluation:**
- Correct LAC usage: 40 points
- Appropriate responses to warnings: 30 points
- Successful target hits: 20 points
- Decision-making quality: 10 points

**Passing Score:** 70/100 points

---

## 9.11 LESSON SUMMARY

### Key Points

1. **Lead Angle Compensation** automatically calculates aim offset for moving targets based on target velocity, range, and projectile time-of-flight.

2. **Activation Requires:** Active track, valid range data, Dead Man Switch (Button 3) held while pressing LAC Toggle (Button 2).

3. **Three Status Indicators:**
   - "LEAD ANGLE ON" (green) = LAC functioning correctly
   - "LEAD ANGLE LAG" (yellow) = Degraded calculation quality
   - "ZOOM OUT" (red) = FOV insufficient for LAC

4. **LAC Bracket** (L-shaped corners) appears on CCIP reticle when LAC active, providing visual confirmation.

5. **Operator Aims at Pipper** - system has already applied lead offset; do NOT manually lead additional distance.

6. **Enable LAC When:** Target moving >5 m/s, crossing line of sight, range >500m, stable track, good conditions.

7. **Disable LAC When:** Target stationary, persistent warnings, max zoom required, poor tracking, engagement complete.

8. **Limitations:** 10° maximum lead angle, FOV restrictions at high zoom, environmental degradation, unpredictable target motion.

9. **Troubleshooting:** Verify Dead Man Switch held, check tracking status, re-lase for fresh range, zoom out for "ZOOM OUT" warning.

10. **Decision-Making:** Balance target velocity, range, tracking quality, and conditions to determine appropriate LAC usage.

### Practical Application

Students should be able to:
- Activate/deactivate LAC with correct safety procedures (<2 seconds)
- Recognize all LAC status indicators instantly and respond appropriately
- Engage moving targets using LAC with >70% hit rate
- Troubleshoot LAC warnings and errors effectively
- Make sound tactical decisions about when LAC should be used
- Integrate LAC into complete engagement sequences smoothly

### Next Lesson Preview

**Lesson 10: Boresight / Zeroing Procedures** will cover:
- Gun-camera boresight alignment fundamentals
- Zeroing procedure to correct ballistic offset
- Multi-range zero validation
- Zeroing with different ammunition types
- Troubleshooting zeroing errors
- Clear Active Zero function
- Maintaining zeroing accuracy over time

---

## APPENDIX A: LAC QUICK REFERENCE

### Activation Checklist
```
☐ Active track (Active Lock status)
☐ Valid range (LRF measurement)
☐ Target moving (>5 m/s recommended)
☐ Moderate zoom (not maximum)
☐ Hold Button 3 (Dead Man Switch)
☐ Press Button 2 (LAC Toggle)
☐ Verify "LEAD ANGLE ON" (green)
```

### Status Indicator Guide
```
🟢 "LEAD ANGLE ON"   → Clear to fire
🟡 "LEAD ANGLE LAG"  → Wait or disable
🔴 "ZOOM OUT"        → Zoom out required
```

### When to Use LAC
```
✓ Target moving >10 m/s
✓ Crossing line of sight
✓ Range >500m
✓ Stable track
✓ Good visibility

❌ Target stationary or <5 m/s
❌ Approaching/receding
❌ Range <200m
❌ Maximum zoom
❌ Poor track
```

### Troubleshooting
```
Problem: Can't enable
→ Hold Button 3 (Dead Man Switch)

Problem: "LAG" warning
→ Wait 3 sec, re-lase, or disable

Problem: "ZOOM OUT" warning
→ Press Button 8 (Zoom Out)

Problem: No lead offset visible
→ Normal if target slow/close/approaching
```

---

**END OF LESSON 9**
