# LESSON 10: BORESIGHT / ZEROING PROCEDURES

**Duration:** 3 hours
**Type:** Practical Skills Training
**Prerequisites:** Lessons 1-9

---

## LESSON OVERVIEW

This lesson covers weapon zeroing (boresight alignment) to ensure the weapon impact point matches the reticle aim point. Zeroing corrects the physical offset between the camera and weapon barrel.

### TERMINAL LEARNING OBJECTIVE (TLO)

Upon completion, the student will be able to perform weapon zeroing procedures, verify zero accuracy at multiple ranges, and maintain zeroing over time.

### ENABLING LEARNING OBJECTIVES (ELO)

1. Explain the boresight offset problem and zeroing solution
2. Perform complete zeroing procedure using joystick adjustment
3. Verify zero at multiple ranges
4. Clear active zero when required
5. Maintain and validate zeroing accuracy

---

## 10.1 BORESIGHT FUNDAMENTALS

### 10.1.1 The Boresight Offset Problem

**Physical Reality:**
- Camera and weapon barrel are physically separated (typically 15-30cm)
- Camera points at reticle center
- Weapon points at different location
- Without correction: weapon hits below/beside reticle aim point

**Diagram:**
```
        Camera → ⊙ (reticle center)
                 ↓

        Weapon → • (actual impact point)

        Offset = Distance between aim and impact
```

### 10.1.2 Zeroing Solution

**Zeroing** applies angular offsets to compensate for physical separation:
- **Azimuth Offset:** Horizontal correction (left/right)
- **Elevation Offset:** Vertical correction (up/down)

**After Zeroing:**
```
        Camera → ⊙ (reticle center with offset applied)
                 ↓
        Weapon → • (impact matches reticle)

        Result: Reticle shows where weapon will hit
```

### 10.1.3 System Storage

**SystemStateData Fields:**
- `zeroingAzimuthOffset` (float, degrees)
- `zeroingElevationOffset` (float, degrees)
- `zeroingAppliedToBallistics` (bool)
- `zeroingModeActive` (bool)

**Persistence:** Zero values saved to configuration file, loaded on startup

---

## 10.2 ZEROING PROCEDURE

### 10.2.1 Pre-Zeroing Requirements

**Environmental Conditions:**
- ✓ Calm wind (<5 knots)
- ✓ Good visibility (daylight preferred)
- ✓ Stable platform (no movement)
- ✓ Temperature moderate (not extreme heat/cold)

**Range Setup:**
- ✓ Known range to target (recommended 100-300m)
- ✓ Fixed target (large, visible, safe backstop)
- ✓ No obstructions between weapon and target

**System Status:**
- ✓ Station powered and initialized
- ✓ Camera operational (day camera for initial zero)
- ✓ Weapon loaded and ready
- ✓ No active tracking or motion modes

### 10.2.2 Complete Zeroing Procedure

**Step 1: Access Zeroing Menu**
```
☐ Press MENU button on Control Panel
☐ Navigate to "Zeroing" option
☐ Press VAL button to enter
☐ Zeroing screen appears with instructions
```

**Step 2: Aim at Target**
```
☐ Use joystick to center reticle on target center
☐ Ensure stable aim
☐ Target should be large (minimum 30cm × 30cm)
```

**Step 3: Fire Test Shot(s)**
```
☐ Hold Button 0 (Master Arm)
☐ Press Button 5 (Fire) - single shot or short burst
☐ Observe impact point on target
☐ Note offset from reticle center (direction and distance)
```

**Step 4: Adjust Reticle to Impact**
```
☐ System displays: "Use JOYSTICK to move main RETICLE to ACTUAL IMPACT POINT"
☐ Use joystick to move reticle from target center to impact location
☐ Reticle now positioned where weapon actually hit
☐ This movement is captured as zeroing offset
```

**Step 5: Apply Zero**
```
☐ Press MENU/VAL button to apply
☐ System calculates offsets (Az, El)
☐ Completion screen shows: "Zeroing Adjustment Applied!"
☐ Displays final offsets (e.g., "Az 0.85°, El -1.23°")
☐ "Z" indicator appears on OSD (confirms zero active)
```

**Step 6: Verify Zero**
```
☐ Return reticle to target center
☐ Fire verification shot
☐ Impact should now match reticle center
☐ If offset remains, repeat procedure
```

### 10.2.3 Zeroing States

**Controller State Machine:**
1. **Idle:** No zeroing active
2. **Instruct_MoveReticleToImpact:** User adjusting reticle to impact
3. **Completed:** Zero applied, ready to return to menu

---

## 10.3 MULTI-RANGE ZERO VALIDATION

### 10.3.1 Why Multiple Ranges Matter

**Ballistic Arc Reality:**
- Zero at 100m may not be accurate at 500m
- Bullet trajectory is curved, not straight line
- Single-range zero is a compromise

**Recommended Zero Range:**
- **100-300m:** Best for general-purpose zero
- Provides acceptable accuracy from 50m to 600m

### 10.3.2 Multi-Range Validation Procedure

**After Initial Zero at 200m:**

**Test 1: Short Range (100m)**
```
☐ Position target at 100m
☐ Aim at center, fire test shot
☐ Note impact point (may be slightly high/low)
☐ Acceptable error: ±5cm at 100m
```

**Test 2: Medium Range (300m)**
```
☐ Position target at 300m
☐ Aim at center, fire test shot
☐ Note impact point
☐ Acceptable error: ±10cm at 300m
```

**Test 3: Long Range (500m)**
```
☐ Position target at 500m
☐ Aim at center, fire test shot
☐ Note impact point (likely low due to drop)
☐ Acceptable error: ±20cm at 500m
```

**If Errors Exceed Acceptable:**
- Re-zero at primary engagement range (e.g., 250m)
- Compromise between short and long range accuracy

---

## 10.4 CLEAR ACTIVE ZERO

### 10.4.1 When to Clear Zero

**Clear Zero When:**
- ❌ Weapon system physically moved/re-mounted
- ❌ Camera alignment changed
- ❌ Different ammunition type loaded (different ballistics)
- ❌ Zero validation shows significant error
- ❌ System maintenance performed on weapon/camera mount

### 10.4.2 Clear Zero Procedure

**Access:**
```
☐ Press MENU button
☐ Navigate to "Clear Active Zero"
☐ Press VAL button
☐ System clears zeroing offsets to 0.0°
☐ "Z" indicator disappears from OSD
☐ Return to Main Menu
```

**Result:**
- `zeroingAzimuthOffset = 0.0`
- `zeroingElevationOffset = 0.0`
- `zeroingAppliedToBallistics = false`
- Weapon returns to uncorrected aim point

**Must Re-Zero:**
After clearing, perform complete zeroing procedure again before engaging targets.

---

## 10.5 ZEROING WITH THERMAL CAMERA

### 10.5.1 Thermal Zero Considerations

**Challenges:**
- Lower resolution than day camera
- Target visibility depends on thermal signature
- Difficult to see small impact points

**Recommended Approach:**
1. Zero using day camera first (preferred)
2. Validate thermal camera zero separately
3. Offsets should be similar (camera alignment)

### 10.5.2 Thermal Zeroing Procedure

**Setup:**
- Use warm target (e.g., heated metal plate, warm vehicle)
- Nighttime or low-light conditions
- Larger target recommended (50cm × 50cm minimum)

**Procedure:**
```
☐ Switch to thermal camera
☐ Select appropriate LUT (White Hot or Black Hot)
☐ Follow standard zeroing procedure (Section 10.2)
☐ Fire tracer rounds (if available) for easier impact observation
☐ Adjust reticle to impact point
☐ Apply zero
```

**Note:** If day zero and thermal zero differ significantly, suspect camera misalignment (notify maintenance).

---

## 10.6 AMMUNITION CONSIDERATIONS

### 10.6.1 Zero Per Ammunition Type

**Different Ammunition = Different Ballistics:**
- Muzzle velocity varies by ammunition type
- Bullet weight affects trajectory
- Projectile shape impacts drag

**Best Practice:**
- Zero for most commonly used ammunition
- If switching ammunition, re-verify zero
- Log which ammunition type was used for zero

### 10.6.2 Ammunition Change Impact

**Example Scenario:**
```
Original Zero: Standard Ball, 920 m/s muzzle velocity
New Ammunition: Armor Piercing, 880 m/s muzzle velocity

Result:
- Slower velocity = more bullet drop
- Impacts will be LOW at all ranges
- Must re-zero or accept reduced accuracy
```

**Options:**
1. Re-zero for new ammunition (preferred)
2. Apply mental correction (e.g., aim 10cm high at 300m)
3. Use multiple zero profiles (if system supports)

---

## 10.7 ZERO VERIFICATION SCHEDULE

### 10.7.1 Routine Verification

**Daily (Before Operations):**
- Visual inspection of camera/weapon mount (no looseness)
- Check "Z" indicator on OSD (confirms zero active)

**Weekly:**
- Fire verification shot at known range (100-200m)
- Confirm impact within acceptable tolerance

**Monthly:**
- Full multi-range validation (100m, 300m, 500m)
- Re-zero if any range exceeds tolerance

**After Maintenance:**
- Always verify zero after any work on:
  - Camera system
  - Weapon mount
  - Gimbal assembly
  - Platform installation

### 10.7.2 Field Expedient Verification

**Quick Check (When Full Zero Not Possible):**
```
☐ Fire 3-shot group at 200m
☐ Measure group center offset from aim point
☐ Acceptable: <5cm offset
☐ If >5cm: Plan to re-zero at next opportunity
```

---

## 10.8 TROUBLESHOOTING ZEROING ISSUES

### Problem: Large Zero Offset Required (>3 degrees)

**Possible Causes:**
- Weapon mount loose or misaligned
- Camera mount loose or misaligned
- Gimbal mechanical issue

**Solution:**
```
☐ Do NOT apply large zero (may mask mechanical problem)
☐ Notify maintenance immediately
☐ Inspect mounts for looseness
☐ Check alignment marks
☐ Re-tighten mounting hardware
☐ After repair, perform fresh zero
```

---

### Problem: Zero Drifts Over Time

**Possible Causes:**
- Temperature changes affecting mounts
- Vibration loosening hardware
- Mount material fatigue

**Solution:**
```
☐ Increase verification frequency
☐ Inspect mounting hardware daily
☐ Apply thread-locking compound to mount bolts
☐ Monitor zero drift pattern
☐ If persistent, notify maintenance
```

---

### Problem: Cannot See Impact Point

**Possible Causes:**
- Target too small
- Poor lighting conditions
- Camera resolution insufficient at range

**Solution:**
```
☐ Use larger target (1m × 1m recommended)
☐ Use high-contrast target (white background, dark aiming point)
☐ Close range to 100m for initial zero
☐ Use tracer ammunition if available
☐ Have observer with spotting scope
```

---

### Problem: Zero Different Day vs. Thermal

**Possible Causes:**
- Camera optical axis misalignment
- Thermal camera shift (thermal expansion)

**Solution:**
```
☐ Compare offsets (should be within 0.5 degrees)
☐ If different >1 degree, notify maintenance
☐ Zero to primary camera (usually day camera)
☐ Accept minor difference on secondary camera
☐ Or zero each camera separately (advanced)
```

---

## 10.9 INTEGRATION WITH OTHER SYSTEMS

### 10.9.1 Zeroing + Windage

**Combined Correction:**
- Zeroing: Corrects gun-camera offset (permanent)
- Windage: Corrects wind drift (temporary, changes with conditions)

**System applies both:**
```
Total Correction = Zeroing Offset + Windage Offset

Example:
Zeroing Azimuth: +0.85°
Windage Azimuth: -0.30° (compensating left crosswind)
Total Azimuth Correction: +0.55°
```

**CCIP reticle shows combined correction**

### 10.9.2 Zeroing + LAC

**Combined Correction:**
- Zeroing: Gun-camera offset
- LAC: Moving target lead angle

**System applies both:**
```
Total Correction = Zeroing Offset + LAC Lead Angle

Example:
Zeroing Azimuth: +0.85°
LAC Azimuth: +2.50° (leading moving target)
Total Azimuth Correction: +3.35°
```

**Operator sees single CCIP pipper with all corrections applied**

---

## 10.10 PROFICIENCY EXERCISES

### Exercise 1: Complete Zeroing (45 min)

**Objective:** Perform full zeroing procedure

**Procedure:**
```
☐ Set up target at 200m
☐ Access zeroing menu
☐ Fire test shot
☐ Adjust reticle to impact
☐ Apply zero
☐ Fire verification shot (impact on reticle)
☐ Record final offset values
```

**Performance Standard:** Verification shot within 5cm of aim point

---

### Exercise 2: Multi-Range Validation (60 min)

**Objective:** Validate zero at 100m, 300m, 500m

**Procedure:**
```
☐ After initial zero at 200m
☐ Test at 100m (record error)
☐ Test at 300m (record error)
☐ Test at 500m (record error)
☐ Evaluate if re-zero needed
```

**Performance Standard:** All ranges within acceptable tolerance

---

### Exercise 3: Zero Verification Drill (15 min)

**Objective:** Quickly verify zero status

**Procedure:**
```
☐ Check "Z" indicator on OSD
☐ Fire single shot at 200m
☐ Assess impact vs. aim point
☐ Determine if re-zero required
```

**Performance Standard:** Complete assessment in <15 minutes

---

## 10.11 LESSON SUMMARY

### Key Points

1. **Zeroing corrects gun-camera boresight offset** - physical separation between camera and weapon barrel

2. **Procedure:** Fire test shot, move reticle to impact point, apply zero

3. **System stores:** Azimuth offset and Elevation offset (degrees)

4. **"Z" indicator** appears on OSD when zero is active

5. **Validate zero** at multiple ranges (100m, 300m, 500m)

6. **Clear zero when:** Weapon moved, ammunition changed, maintenance performed

7. **Verify regularly:** Weekly test shots, monthly full validation

8. **Combine with other systems:** Zeroing + Windage + LAC all applied together

9. **Troubleshoot large offsets:** May indicate mechanical problem, inspect hardware

10. **Different ammunition** may require re-zeroing due to ballistic differences

---

**END OF LESSON 10**
