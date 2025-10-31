# APPENDIX C: BUTTON MAPPING QUICK REFERENCE CARD

**Purpose:** Quick reference for all joystick button functions

---

## C.1 JOYSTICK BUTTON MAP

### Primary Controls

| Button | Function | Safety Interlock | Notes |
|--------|----------|------------------|-------|
| **0** | Master Arm | Station ON | Hold to arm weapon |
| **2** | LAC Toggle | Dead Man Switch (3) | Enable/disable lead compensation |
| **3** | Dead Man Switch | None | Safety interlock for critical ops |
| **4** | Track Select/Abort | Dead Man (recommended) | Single=advance phase, Double=abort |
| **5** | Fire Weapon | Station ON | Hold to fire |
| **6** | Zoom In | None | Day and thermal cameras |
| **7** | Next LUT | None | Thermal camera only |
| **8** | Zoom Out | None | Day and thermal cameras |
| **9** | Previous LUT | None | Thermal camera only |
| **11** | Cycle Motion Mode | Station ON | Manual→Auto→TRP→Radar |
| **13** | Cycle Motion Mode | Station ON | Same as Button 11 |
| **14** | Up/Next Selector | None | Context-sensitive |
| **16** | Down/Prev Selector | None | Context-sensitive |

### Analog Controls

| Control | Function | Mode | Range |
|---------|----------|------|-------|
| **Stick X-Axis** | Azimuth (L/R) | Manual | ±10 deg/s |
| **Stick Y-Axis** | Elevation (U/D) | Manual | ±10 deg/s (inverted) |
| **D-Pad Up** | Decrease box height | Acquisition | -4 pixels |
| **D-Pad Down** | Increase box height | Acquisition | +4 pixels |
| **D-Pad Left** | Decrease box width | Acquisition | -4 pixels |
| **D-Pad Right** | Increase box width | Acquisition | +4 pixels |

---

## C.2 BUTTON FUNCTIONS BY CONTEXT

### C.2.1 During Normal Operations

**Button 0:** Hold for Master Arm (weapon hot)
**Button 3:** Hold for safety-critical operations
**Button 4:** Single press enters tracking acquisition
**Button 5:** Hold to fire weapon
**Button 6/8:** Zoom in/out
**Button 7/9:** Cycle thermal LUT
**Button 11/13:** Cycle motion modes
**Stick:** Gimbal control (Manual mode)

### C.2.2 During Tracking Acquisition

**D-Pad:** Resize tracking box (±4 pixels per press)
**Button 4:** Press again to request lock-on
**Button 4 (double-click):** Abort tracking
**Stick:** Fine adjustments to box position

### C.2.3 During Active Tracking

**Button 4 (single):** No effect (already tracking)
**Button 4 (double-click):** Emergency abort
**Stick:** Gentle tracking assist
**Button 11/13:** BLOCKED (cannot cycle during tracking)

### C.2.4 Button 14/16 Context Modes

| Operational Mode | Motion Mode | Button 14 (Up) | Button 16 (Down) |
|------------------|-------------|----------------|------------------|
| Idle | Any | setUpSw() | setDownSw() |
| Tracking | Any | setUpTrack() | setDownTrack() |
| Surveillance | TRPScan | Next TRP | Previous TRP |
| Surveillance | AutoSectorScan | Next Scan Zone | Previous Scan Zone |

---

## C.3 EMERGENCY BUTTONS

### E-Stop (Control Panel)
**Location:** Red mushroom button on PLC21
**Function:** Immediate all-stop
**Use:** Any immediate danger
**Reset:** Twist clockwise to unlatch

### Button 4 Double-Click
**Function:** Emergency tracking abort
**Timing:** <1000ms between clicks
**Use:** Wrong target, unsafe tracking
**Effect:** Instant tracking termination

---

## C.4 SAFETY INTERLOCKS SUMMARY

### Dead Man Switch (Button 3) Required For:
- ✓ LAC enable/disable (Button 2)
- ✓ Track initiation (Button 4) - recommended
- ✓ Critical fire control functions

### Master Arm (Button 0) Required For:
- ✓ Weapon fire (Button 5)

### Station Power Required For:
- ✓ Motion mode cycling (Button 11/13)
- ✓ Master Arm function
- ✓ Weapon fire

---

## C.5 COMMON BUTTON SEQUENCES

### Complete Engagement Sequence
```
1. Hold Button 3 (Dead Man)
2. Press Button 4 (Acquisition)
3. D-Pad: Size box
4. Press Button 4 (Lock)
5. Release Button 3
6. [Wait for Active Lock]
7. Hold Button 3 + Press Button 2 (Enable LAC)
8. Release Button 3
9. Hold Button 0 (Master Arm)
10. Press Button 5 (Fire)
11. Release Button 5
12. Release Button 0
13. Hold Button 3 + Press Button 2 (Disable LAC)
14. Double-click Button 4 (Stop tracking)
```

### Quick Tracking Abort
```
Double-click Button 4 (<1000ms)
```

### Mode Cycling
```
Press Button 11 or 13 repeatedly:
Manual → AutoSectorScan → TRPScan → RadarSlew → (repeat)
```

### Camera Operations
```
Zoom In: Hold Button 6
Zoom Out: Hold Button 8
Next LUT: Press Button 7 (thermal only)
Prev LUT: Press Button 9 (thermal only)
```

---

## C.6 UNASSIGNED BUTTONS

**Available for Future Use:**
- Button 1
- Button 10
- Button 12
- Button 15
- Button 17
- Button 18
- Button 19

**Do NOT use** - No current function assigned

---

## C.7 TROUBLESHOOTING

### Button Not Responding

**Button 2 (LAC):**
- Check: Is Button 3 (Dead Man) held?

**Button 4 (Track):**
- Check: Is Dead Man Switch held? (recommended)
- Check: Already in tracking? (use double-click to abort first)

**Button 11/13 (Mode Cycle):**
- Check: Station powered ON?
- Check: Not in Acquisition phase?

**Button 14/16 (Selectors):**
- Check: Correct mode for expected function?
- Function changes based on operation mode

### Double-Click Not Working

**If tracking won't abort:**
- Press faster (<1000ms between clicks)
- Ensure both clicks registered
- As emergency: Use E-Stop

---

## C.8 FIELD REFERENCE CARD

**Print and laminate this section for field use:**

```
═══════════════════════════════════════════════════════
          RCWS JOYSTICK QUICK REFERENCE
═══════════════════════════════════════════════════════

WEAPON CONTROLS:
  Btn 0: Master Arm (hold)
  Btn 5: Fire (hold)
  Btn 3: Dead Man Switch (safety)

TRACKING:
  Btn 4 (1x):  Enter acquisition / Request lock
  Btn 4 (2x):  ABORT (<500ms)
  D-Pad:       Resize box (Acquisition phase only)

CAMERA:
  Btn 6:  Zoom In
  Btn 8:  Zoom Out
  Btn 7:  Next LUT (thermal)
  Btn 9:  Prev LUT (thermal)

FIRE CONTROL:
  Btn 2:  LAC Toggle (requires Btn 3)
  Btn 3:  Dead Man Switch

MODES:
  Btn 11/13:  Cycle Motion Modes
  Btn 14:     Up/Next
  Btn 16:     Down/Previous

STICK:
  X-Axis:  Azimuth (Left/Right)
  Y-Axis:  Elevation (Up/Down)

EMERGENCY:
  E-STOP:  Red button on Control Panel
  Btn 4 Double-Click:  Abort tracking

INTERLOCKS:
  LAC:     Needs Btn 3 (Dead Man)
  Fire:    Needs Btn 0 (Master Arm)
  Modes:   Needs Station ON

═══════════════════════════════════════════════════════
```

---

**END OF APPENDIX C**
