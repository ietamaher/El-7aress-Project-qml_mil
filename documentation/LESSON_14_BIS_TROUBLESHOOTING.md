# LESSON 14-BIS: TROUBLESHOOTING

**Duration:** 3 hours (2 hours instruction, 1 hour practical)

**Method:** Classroom instruction with hands-on troubleshooting exercises

---

## TERMINAL LEARNING OBJECTIVE

**Action:** Diagnose and resolve common RCWS system faults

**Condition:** Given an RCWS system with simulated or actual faults, troubleshooting guides, and operator-level tools

**Standard:** Correctly diagnose 80% of common faults, apply appropriate operator-level fixes, and properly escalate issues requiring maintenance support

---

## ENABLING LEARNING OBJECTIVES

**ELO 14.1:** Diagnose common joystick issues
**ELO 14.2:** Troubleshoot camera problems
**ELO 14.3:** Resolve tracking failures
**ELO 14.4:** Address gimbal movement issues
**ELO 14.5:** Identify when to escalate to maintenance personnel

---

## 14.1 TROUBLESHOOTING METHODOLOGY

### 14.1.1 Systematic Approach

**STOP-LOOK-ASSESS-FIX (SLAF) Method:**

**S - STOP:**
- Don't rush to "fix" without understanding
- Take E-Stop if safety concern
- Document the symptoms

**L - LOOK:**
- Observe all symptoms carefully
- Check system status display
- Review recent actions (what changed?)

**A - ASSESS:**
- Narrow down to specific subsystem
- Check simplest causes first
- Consult troubleshooting charts

**F - FIX:**
- Apply operator-level fix if authorized
- Escalate to maintenance if needed
- Verify fix resolved the issue

### 14.1.2 First Principles

**Check the Obvious First:**
1. Power (is it plugged in? switched on?)
2. Connections (cables secure?)
3. Switches (correct position?)
4. E-Stop (is it engaged?)
5. Interlocks (safety conditions met?)

**"Changed Recently" Rule:**
- If it worked yesterday and doesn't today, what changed?
- New zone file loaded?
- Settings changed?
- Cable moved?

**Isolate the Problem:**
- One subsystem at a time
- Swap known-good components if available
- Test with simplified configuration

---

## 14.2 JOYSTICK TROUBLESHOOTING

### 14.2.1 Joystick Not Detected

**Symptoms:**
- "Joystick not connected" message
- No response to button presses
- Gimbal doesn't respond to stick movement

**Diagnosis Steps:**
```
1. Check USB cable connection (both ends)
2. Look for joystick power LED (if equipped)
3. Try different USB port on processor
4. Check system logs: journalctl | grep -i joystick
5. Verify USB enumeration: lsusb (should show device)
```

**Operator-Level Fixes:**
```
☐ Firmly reconnect USB cable
☐ Power cycle system (shutdown, wait 10 sec, restart)
☐ Try known-good USB cable (from spares)
☐ Verify joystick not damaged (visual inspection)
```

**Escalate If:**
- Joystick detected but buttons don't work (firmware issue)
- Physical damage to joystick
- Different joystick also fails (processor USB problem)

### 14.2.2 Buttons Not Responding

**Symptoms:**
- Some buttons work, others don't
- Button presses not registered

**Diagnosis Steps:**
```
1. Test ALL buttons systematically (0-19)
2. Check if specific buttons always fail
3. Test with button mapping screen (if available)
4. Check for stuck buttons (press and release feel normal?)
```

**Operator-Level Fixes:**
```
☐ Clean around button (compressed air)
☐ Press button 10-20 times (may unstick)
☐ Check for debris under button
☐ Verify Dead Man Switch active (some buttons require it)
```

**Escalate If:**
- Multiple buttons permanently failed
- Button physically broken
- Intermittent failures (electrical issue)

### 14.2.3 Stick Drift or Jitter

**Symptoms:**
- Gimbal moves when stick centered
- Erratic gimbal motion
- Stick position reading jumps

**Diagnosis Steps:**
```
1. Center stick and observe gimbal
2. Check stick physically centered (not off to side)
3. Watch raw axis values (if display available)
4. Test in different motion modes
```

**Operator-Level Fixes:**
```
☐ Recalibrate joystick (if option in menu)
☐ Clean joystick base (dust can cause drift)
☐ Increase deadzone in settings (if available)
☐ Power cycle system
```

**Escalate If:**
- Drift exceeds acceptable limits (>5% of range)
- Physical damage to stick mechanism
- Calibration doesn't help

### 14.2.4 HAT Switch (D-Pad) Not Working

**Symptoms:**
- Can't resize tracking gate
- D-Pad presses ignored

**Diagnosis Steps:**
```
1. Verify in Acquisition phase (D-Pad only works here)
2. Test all 8 directions (up, down, left, right, diagonals)
3. Check for stuck position
```

**Operator-Level Fixes:**
```
☐ Clean around HAT switch
☐ Verify correct tracking phase
☐ Test in different tracking scenarios
```

**Escalate If:**
- HAT switch physically broken
- Works intermittently (electrical fault)

---

## 14.3 CAMERA TROUBLESHOOTING

### 14.3.1 No Video Feed

**Symptoms:**
- Black screen
- "Camera not connected" message
- Frozen image

**Diagnosis - Day Camera:**
```
1. Check Ethernet cable (both ends secure)
2. Verify camera power LED (if visible)
3. Ping camera: ping 192.168.1.10
4. Check network interface: ip addr show
5. Try accessing camera HTTP API: curl http://192.168.1.10/api/v1/camera/status
```

**Diagnosis - Thermal Camera:**
```
1. Check Ethernet cable
2. Ping camera: ping 192.168.1.11
3. Check video port open: nc -zv 192.168.1.11 9876
4. Verify camera powered (5V supply)
```

**Operator-Level Fixes:**
```
☐ Firmly reconnect Ethernet cable
☐ Power cycle camera (if switch accessible)
☐ Power cycle main system
☐ Try known-good Ethernet cable
☐ Check IP address configuration (correct subnet?)
☐ Remove and reinstall lens cap (may be on!)
```

**Escalate If:**
- Camera doesn't respond to ping (hardware fault)
- Ethernet cable tested good but no connection
- Camera shows in network but no video

### 14.3.2 Poor Image Quality

**Symptoms:**
- Blurry image
- Washed out colors
- Dark image

**Diagnosis - Day Camera:**
```
1. Check lens for dirt, smudges, fingerprints
2. Verify lens cap removed
3. Check focus setting (auto vs. manual)
4. Check exposure/brightness
5. Test zoom (blur at one zoom but not others?)
```

**Diagnosis - Thermal Camera:**
```
1. Perform FFC (Flat Field Correction) if option available
2. Try different LUT palettes
3. Check for lens obstruction
4. Verify camera not overheated
```

**Operator-Level Fixes:**
```
☐ Clean lens (proper technique, microfiber cloth)
☐ Enable autofocus (day camera)
☐ Trigger FFC manually (thermal)
☐ Change LUT palette (thermal)
☐ Adjust digital gain (if available)
☐ Allow thermal camera to cool (if overheated)
```

**Escalate If:**
- Cleaning doesn't improve image
- Focus mechanism broken
- Thermal image has dead pixels or stripes
- Camera overheats repeatedly

### 14.3.3 Zoom Not Working

**Symptoms:**
- Zoom buttons do nothing
- Zoom stutters or stops mid-range
- Stuck at one zoom level

**Diagnosis - Day Camera:**
```
1. Test both Zoom In (Button 6) and Zoom Out (Button 8)
2. Check if zoom at hard limit (can't zoom out if fully wide)
3. Listen for zoom motor sound
4. Check camera status via HTTP API
```

**Diagnosis - Thermal Camera:**
```
1. Check digital zoom level (2x, 4x, 8x)
2. Verify correct camera selected (thermal vs. day)
3. Test zoom cycle through all levels
```

**Operator-Level Fixes:**
```
☐ Try opposite zoom button (may be at limit)
☐ Power cycle camera
☐ Switch to other camera and back
☐ Check zoom motor not obstructed (day camera)
```

**Escalate If:**
- Zoom motor makes grinding noise (mechanical fault)
- Zoom commands sent but no response
- Zoom moves but image doesn't change

### 14.3.4 Camera Switching Not Working

**Symptoms:**
- Stuck on day or thermal camera
- Camera doesn't switch when button pressed

**Diagnosis:**
```
1. Identify which button switches cameras (check manual)
2. Verify both cameras connected
3. Check System Status for camera connection
4. Try switching from menu (if option available)
```

**Operator-Level Fixes:**
```
☐ Check both cameras have video feed individually
☐ Power cycle system
☐ Verify camera switch button not failed
☐ Check interlocks (some systems require specific mode)
```

**Escalate If:**
- One camera completely failed (always switches to other)
- Video routing problem (software issue)

---

## 14.4 TRACKING TROUBLESHOOTING

### 14.4.1 Cannot Enter Acquisition Phase

**Symptoms:**
- Press Track button (Button 4), nothing happens
- Tracking stays in "Off" phase

**Diagnosis:**
```
1. Check Dead Man Switch held (Button 3)
2. Verify Station Enabled (switch ON)
3. Check System Status display
4. Verify not in automated scan mode
5. Check E-Stop not engaged
```

**Operator-Level Fixes:**
```
☐ Hold Dead Man Switch before pressing Track
☐ Enable Station (switch to ON)
☐ Release E-Stop (twist to unlock)
☐ Exit automated scan mode (Button 11/13 to Manual)
☐ Power cycle system
```

**Escalate If:**
- All interlocks met but tracking won't start
- Track button failed (test other buttons)
- Software fault (tracking module not responding)

### 14.4.2 Cannot Achieve Lock-On

**Symptoms:**
- Stuck in "Lock Pending" (yellow box)
- Never transitions to "Active Lock"

**Diagnosis:**
```
1. Check target size (too small? too large?)
2. Verify target has contrast (thermal or visual)
3. Check tracking gate positioned on target center
4. Observe target movement (too fast?)
5. Check camera image quality (adequate for tracking?)
```

**Operator-Level Fixes:**
```
☐ Resize tracking gate (use D-Pad)
☐ Position gate on high-contrast part of target
☐ Try different camera (day vs. thermal)
☐ Ensure target fills at least 20% of gate
☐ Zoom in slightly (improves target resolution)
☐ Slow vehicle/gimbal motion (reduce jitter)
```

**Escalate If:**
- Tracking never locks on any target
- Tracker software not running
- Video processor module failed

### 14.4.3 Tracking Lost Frequently

**Symptoms:**
- Locks briefly then enters Coast mode
- "Track Lost" errors
- Gimbal doesn't follow target smoothly

**Diagnosis:**
```
1. Observe when track is lost (pattern?)
2. Check for obstructions (trees, buildings)
3. Verify stabilization enabled
4. Check camera exposure (target too dark/bright?)
5. Observe target speed and trajectory
```

**Operator-Level Fixes:**
```
☐ Enable stabilization (reduces platform jitter)
☐ Increase tracking gate size slightly
☐ Use thermal camera (better in low light)
☐ Adjust camera exposure for target
☐ Avoid tracking in heavy FOV clutter
☐ Re-initiate track if lost
```

**Escalate If:**
- Tracking performance significantly degraded
- IMU fault (stabilization not working)
- Camera intermittent failures
- Video processor overloaded

### 14.4.4 LAC Not Working

**Symptoms:**
- "LEAD ANGLE ON" doesn't show
- LAC bracket not displayed
- Target moving but no lead calculated

**Diagnosis:**
```
1. Verify Dead Man Switch held when toggling LAC (Button 2)
2. Check Active Lock achieved (LAC requires lock)
3. Verify LRF range acquired (LAC needs range)
4. Check LAC status message on OSD
5. Observe if target velocity detected
```

**Operator-Level Fixes:**
```
☐ Hold Dead Man Switch (Button 3) + press LAC Toggle (Button 2)
☐ Ensure tracking in Active Lock phase
☐ Range target with LRF before enabling LAC
☐ Verify target actually moving (LAC needs motion)
☐ Check zoom not too high (LAC may show "ZOOM OUT")
```

**Escalate If:**
- LAC enabled but no calculations shown
- LAC bracket displayed incorrectly
- LAC performance significantly inaccurate
- Software module fault

---

## 14.5 GIMBAL MOVEMENT TROUBLESHOOTING

### 14.5.1 Gimbal Not Moving

**Symptoms:**
- Joystick input has no effect
- Gimbal frozen in position
- No response to any motion commands

**Diagnosis:**
```
1. Check E-Stop engaged (most common cause!)
2. Verify Station Enabled
3. Check motion mode (Manual vs. auto modes)
4. Listen for servo motor hum (are they powered?)
5. Check System Status for servo connection
6. Look for alarm indicators on Control Panel
```

**Operator-Level Fixes:**
```
☐ Release E-Stop (twist clockwise)
☐ Enable Station (switch to ON)
☐ Verify in Manual mode (auto modes may appear frozen)
☐ Check Dead Man Switch for Manual Track mode
☐ Power cycle system (last resort)
☐ Check for No-Traverse zone restriction
```

**Escalate If:**
- Servos not powered (check power supply)
- Servo drive faults (check status LEDs)
- Communication fault (servos don't respond to commands)
- Mechanical jam (gimbal physically stuck)

### 14.5.2 Gimbal Moves Erratically

**Symptoms:**
- Jerky motion
- Unexpected sudden movements
- Oscillations or hunting

**Diagnosis:**
```
1. Check stabilization setting (ON or OFF?)
2. Verify joystick not drifting (see 14.2.3)
3. Observe if happens in all modes or just one
4. Check servo temperatures (overheating?)
5. Listen for unusual noises (grinding, clicking)
```

**Operator-Level Fixes:**
```
☐ Disable stabilization temporarily (test if IMU issue)
☐ Reduce gimbal speed setting (Control Panel switch)
☐ Recalibrate joystick if drifting
☐ Allow servos to cool if overheated
☐ Check no loose cables interfering with motion
```

**Escalate If:**
- Servo tuning parameters wrong (maintenance task)
- Servo drive fault
- IMU providing bad data
- Mechanical binding or wear

### 14.5.3 Gimbal Hits Limits Unexpectedly

**Symptoms:**
- Motion stops before expected limit
- "Limit switch activated" message
- Can't reach full range

**Diagnosis:**
```
1. Check for No-Traverse zones active
2. Verify software limits set correctly
3. Check physical limit switches (if accessible)
4. Test full range slowly in Manual mode
5. Check gimbal orientation (upside down mount?)
```

**Operator-Level Fixes:**
```
☐ Review zone definitions (disable test zone if blocking)
☐ Check limit switch not stuck (visual inspection)
☐ Verify gimbal home position calibrated correctly
☐ Clear any software limits if set incorrectly
```

**Escalate If:**
- Physical limit switch failed
- Encoder zero position lost (needs recalibration)
- Software configuration error (maintenance)

### 14.5.4 Gimbal Runaway

**Symptoms:**
- Gimbal moves on its own
- Cannot stop with joystick
- Uncontrolled motion

**IMMEDIATE ACTION:**
```
1. PRESS E-STOP IMMEDIATELY
2. If E-Stop fails, remove main power
3. Do not attempt to physically stop gimbal
```

**After Stopped:**
```
☐ Do NOT reset E-Stop or reapply power
☐ Tag system "DO NOT OPERATE"
☐ Report immediately to supervisor
☐ Document exact behavior observed
```

**Escalate Immediately - DO NOT ATTEMPT OPERATOR-LEVEL FIX**

**Possible Causes (for maintenance):**
- Servo drive fault
- Loss of encoder feedback
- Software control loop failure
- Command signal noise

---

## 14.6 LASER RANGE FINDER TROUBLESHOOTING

### 14.6.1 LRF Not Ranging

**Symptoms:**
- No distance displayed
- "No echo" message
- LRF button does nothing

**Diagnosis:**
```
1. Check LRF connected (System Status)
2. Verify target is rangeable (not sky/empty space)
3. Check range within limits (50m - 4000m)
4. Observe if target too close or too far
5. Check for fog, rain, or smoke (degrades LRF)
```

**Operator-Level Fixes:**
```
☐ Point at solid target (vehicle, building, terrain)
☐ Avoid glass, water, or shiny surfaces
☐ Wait for weather to clear (if heavy fog/rain)
☐ Try closer target (may be beyond 4000m range)
☐ Clean LRF lens window (may be dirty)
☐ Power cycle system
```

**Escalate If:**
- LRF not communicating (serial connection fault)
- Laser not firing (safety interlock or hardware fault)
- Consistent "no echo" on known-good targets

### 14.6.2 LRF Inaccurate

**Symptoms:**
- Ranges clearly wrong
- Inconsistent readings (fluctuating wildly)

**Diagnosis:**
```
1. Range known distance (e.g., 100m target marker)
2. Compare multiple readings (are they consistent?)
3. Check environmental conditions
4. Verify LRF lens clean
```

**Operator-Level Fixes:**
```
☐ Clean LRF lens window
☐ Take multiple readings, use average
☐ Account for weather (rain/fog reduces accuracy)
☐ Verify target is solid (not smoke, dust, vegetation)
```

**Escalate If:**
- Consistent error >5 meters at known range
- LRF requires calibration (maintenance task)
- Internal fault

---

## 14.7 COMMUNICATION TROUBLESHOOTING

### 14.7.1 Device Not Connected

**Symptoms:**
- System Status shows "Not Connected"
- Subsystem not responding

**General Diagnosis:**
```
1. Identify which device (PLC21, PLC42, camera, servo, etc.)
2. Check physical connection (cable plugged in?)
3. Check power to device (LED indicators?)
4. Verify correct port/interface (USB, Ethernet, serial)
```

**Serial Devices (PLC21, PLC42, LRF, IMU):**
```
☐ Check USB-to-Serial adapter connected
☐ Try different USB port
☐ Check serial cable not damaged
☐ Power cycle device if possible
```

**Network Devices (Cameras):**
```
☐ Check Ethernet cable (both ends)
☐ Verify switch powered (if used)
☐ Ping device IP address
☐ Check IP configuration correct
```

**CAN Bus Devices (Servos):**
```
☐ Check CAN bus terminated (120Ω resistors)
☐ Verify CAN H/L not swapped
☐ Check CAN interface up: ip link show can0
```

**Escalate If:**
- Multiple devices not connecting (system-wide issue)
- Known-good cable doesn't fix
- Device responds to ping but no data

### 14.7.2 Intermittent Connection

**Symptoms:**
- Device connects and disconnects randomly
- Timeout errors in logs

**Diagnosis:**
```
1. Check for loose cable (wiggle gently while observing)
2. Check connector corrosion (green residue?)
3. Verify cable routing (not pinched or strained?)
4. Check electrical noise sources (motors, radios)
```

**Operator-Level Fixes:**
```
☐ Reseat all connectors firmly
☐ Secure loose cables (cable ties)
☐ Reroute cable away from motors/power cables
☐ Clean connectors (contact cleaner if available)
☐ Replace suspect cable with known-good spare
```

**Escalate If:**
- Intermittent fault persists after cable check
- Connector damaged (bent pins, broken housing)
- Electrical interference (requires shielding/filtering)

---

## 14.8 WHEN TO ESCALATE

### 14.8.1 Operator vs. Maintenance Responsibilities

**Operator-Level (You Can Fix):**
```
✓ Cable connections (reseat, swap)
✓ Power cycling
✓ Cleaning (lenses, connectors)
✓ Software settings (zones, reticle, modes)
✓ Consumable replacement (fuses if authorized)
✓ Simple adjustments (joystick calibration)
```

**Maintenance-Level (Must Escalate):**
```
✗ Opening sealed components
✗ Servo tuning or calibration
✗ Software updates/reinstall
✗ Component replacement (cameras, servos, PLCs)
✗ Electrical troubleshooting (voltages, wiring)
✗ Mechanical repair (bearings, gears, actuators)
```

### 14.8.2 Escalation Criteria

**Escalate Immediately If:**
- Safety hazard (runaway, fire, smoke, burning smell)
- E-Stop doesn't stop motion
- Multiple systems failed simultaneously
- Physical damage (broken, bent, cracked components)
- Unauthorized behavior (system acts on its own)

**Escalate After Operator Troubleshooting If:**
- Operator-level fixes don't resolve
- Issue beyond operator scope
- Requires tools/parts not available
- Intermittent fault (hard to diagnose)
- Performance degraded significantly

**Can Defer (Non-Critical):**
- Cosmetic issues (scratches, worn labels)
- Single faulty button (if workaround exists)
- Minor image quality degradation
- Slow degradation over time (document trend)

### 14.8.3 Escalation Documentation

**When Reporting Fault:**
```
1. Fault description (specific symptoms)
2. When fault started (date/time)
3. What you were doing (operation in progress)
4. Troubleshooting steps YOU tried
5. Current system status (operational? safe?)
6. Urgency (mission-critical? can wait?)
```

**Use Fault Report Template (Appendix I, Section I.5.1)**

---

## 14.9 COMMON FAULT SCENARIOS

### Scenario 1: "Nothing Works"

**Symptoms:** System completely unresponsive after power-up

**Most Likely:**
```
☐ E-Stop engaged (check first!)
☐ Main power not applied
☐ Power supply fault (check voltages)
☐ Main processor didn't boot (check display)
```

### Scenario 2: "Worked Yesterday, Broken Today"

**Symptoms:** System was fine, now has fault

**Most Likely:**
```
☐ Cable disconnected (check recent maintenance)
☐ Settings changed (restore from backup)
☐ Zone file loaded (check definitions)
☐ Software update applied (check version)
☐ Environmental change (extreme temp, moisture)
```

### Scenario 3: "Intermittent Problem"

**Symptoms:** Fault comes and goes

**Most Likely:**
```
☐ Loose cable (wiggle test)
☐ Connector corrosion (clean contacts)
☐ Thermal issue (works cold, fails hot)
☐ Software race condition (rare, escalate)
```

### Scenario 4: "Everything Slow"

**Symptoms:** System responds but sluggish

**Most Likely:**
```
☐ Processor overloaded (too many apps running?)
☐ Thermal throttling (check temps)
☐ Network congestion (camera streaming issues)
☐ Low voltage (check power supply)
```

---

## 14.10 PRACTICAL EXERCISES

### Exercise 1: Simulated Faults (20 minutes)

**Objective:** Diagnose and fix common faults

Instructor will introduce faults (one at a time):
```
1. Disconnect joystick USB cable → Operator finds and fixes
2. Engage E-Stop → Operator identifies and releases
3. Set gimbal speed to 0 → Operator finds setting
4. Load zone with no-traverse covering center → Operator reviews zones
5. Disconnect Ethernet to day camera → Operator tests connection
```

**Standards:** Diagnose and fix 4/5 faults within time limit

### Exercise 2: Troubleshooting Decision Tree (15 minutes)

**Scenario:** Gimbal won't move

Work through diagnosis:
```
1. Is E-Stop engaged? → Yes/No
2. Is Station Enabled? → Yes/No
3. Is joystick connected? → Yes/No
4. Are servos powered? → Yes/No
5. Is gimbal in No-Traverse zone? → Yes/No
```

Practice on 3 different scenarios

### Exercise 3: Escalation Judgment (15 minutes)

**Evaluate these situations - Fix or Escalate?**

```
1. Lens slightly dirty → OPERATOR (clean lens)
2. Lens cracked → ESCALATE (replacement needed)
3. Tracking won't lock → OPERATOR (try troubleshooting steps)
4. Servo making grinding noise → ESCALATE (mechanical fault)
5. Button doesn't respond → OPERATOR (check interlock, clean)
6. Multiple buttons failed → ESCALATE (joystick replacement)
7. Image too dark → OPERATOR (adjust exposure, clean lens)
8. Camera no video, won't ping → ESCALATE (hardware fault)
```

Discuss each scenario, explain reasoning

### Exercise 4: Fault Report Writing (10 minutes)

**Given scenario, write complete fault report:**

"During operation, thermal camera video froze. Attempted to switch to day camera - worked fine. Switched back to thermal - still frozen. Power cycled system - thermal camera now shows 'Not Connected.' Checked Ethernet cable - secure. Pinged camera - no response."

**Complete Appendix I fault report form**

Standards: Report includes all required information, clear description, troubleshooting steps documented

---

## 14.11 TROUBLESHOOTING QUICK REFERENCE

### Decision Matrix

```
SYMPTOM                    → CHECK FIRST           → THEN CHECK        → ESCALATE IF
──────────────────────────────────────────────────────────────────────────────────────
No gimbal motion           → E-Stop                → Station Enable    → Servo fault
Joystick not detected      → USB cable             → Different port    → Joystick broken
No video                   → Cable connected       → Lens cap off      → Camera fault
Tracking won't start       → Dead Man Switch       → Station ON        → Software fault
LRF no range               → Solid target aimed    → LRF lens clean    → LRF fault
Gimbal erratic             → Joystick drift        → Stabilization     → Servo tuning
Camera blurry              → Clean lens            → Autofocus ON      → Focus broken
Button doesn't work        → Interlock met         → Clean button      → Button failed
Device not connected       → Cable secure          → Power cycle       → Device fault
```

### Top 10 Common Faults

1. **E-Stop engaged** (gimbal won't move) → Twist to release
2. **Lens cap on** (no video) → Remove lens cap
3. **Dead Man Switch not held** (tracking won't start) → Hold Button 3
4. **Joystick USB loose** (no response) → Reseat cable
5. **Station disabled** (system locked) → Enable station switch
6. **Wrong motion mode** (gimbal seems frozen) → Cycle to Manual
7. **Dirty lens** (poor image) → Clean with microfiber cloth
8. **LRF pointed at sky** (no range) → Aim at solid target
9. **No-Traverse zone** (gimbal won't move direction) → Check zone definitions
10. **Ethernet cable loose** (camera dropout) → Reseat cable firmly

---

## SUMMARY

Effective troubleshooting requires:
- **Systematic approach** (STOP-LOOK-ASSESS-FIX)
- **Check simple causes first** (cables, switches, interlocks)
- **Understand operator vs. maintenance scope**
- **Know when to escalate** (safety first, don't exceed authority)
- **Document faults properly** (helps maintenance diagnose)

**Key Takeaway:** 80% of faults are simple (cables, switches, settings). Check the obvious before assuming complex failure.

**Next Lesson:** Lesson 15 - Hands-On Training (apply troubleshooting skills in realistic scenarios)

---

**END OF LESSON 14-BIS**
