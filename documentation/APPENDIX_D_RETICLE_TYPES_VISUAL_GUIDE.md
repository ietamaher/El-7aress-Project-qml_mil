# APPENDIX D: RETICLE TYPES VISUAL GUIDE

**Purpose:** Visual reference for all 5 reticle types with selection guidance

---

## D.1 RETICLE OVERVIEW

### D.1.1 Available Reticle Types

The system offers **5 reticle types** selectable via Main Menu:
1. Circle-Dot Reticle
2. Box Crosshair
3. Tactical Crosshair
4. CCIP Fire Control
5. Mil-Dot Ranging

**Selection Path:** MENU → Personalize Reticle → Select Type

---

## D.2 RETICLE TYPE DESCRIPTIONS

### D.2.1 Circle-Dot Reticle (Default)

**Appearance:**
```
          ___
        /     \
       |   ●   |
        \ ___ /

```

**Description:**
- Circle with center dot
- Circle diameter: 30-40 pixels
- Center dot marks precise aim point
- Clean, uncluttered design

**Best For:**
- General purpose operations
- New operators (simple design)
- Fast target acquisition
- Moving target estimation

**Advantages:**
- ✓ Simple and intuitive
- ✓ Circle aids target size estimation
- ✓ Good for leading moving targets
- ✓ Center dot maintains precision
- ✓ Minimal visual clutter

**Disadvantages:**
- ❌ Circle may obscure target edges at high zoom

---

### D.2.2 Box Crosshair

**Appearance:**
```
    ┌───────┐
    |       |
    |   ●   |
    |       |
    └───────┘
```

**Description:**
- Rectangular box with center point
- Corner markers define box
- Center point marks aim point
- Clean reference frame

**Best For:**
- Precision aiming at medium/long range
- Stationary targets
- Clear FOV awareness
- Range card reference

**Advantages:**
- ✓ Clear reference frame
- ✓ Box provides spatial awareness
- ✓ Good for stationary targets
- ✓ Professional military appearance

**Disadvantages:**
- ❌ Box corners may distract
- ❌ More complex than simple dot

---

### D.2.3 Tactical Crosshair

**Appearance:**
```
           |
           |
    -------+-------
           |
           |
```

**Description:**
- Traditional military crosshair
- Fine center lines (1-2 pixel width)
- Extended to edge of FOV
- Center intersection marks aim point
- Standard military reticle design

**Best For:**
- Precision fire at all ranges
- Trained operators
- Horizontal/vertical alignment reference
- Standard military operations

**Advantages:**
- ✓ Familiar to military-trained operators
- ✓ High precision at center
- ✓ Clear horizontal/vertical reference
- ✓ Works well at all zoom levels
- ✓ Proven military standard

**Disadvantages:**
- ❌ Lines may obscure small targets
- ❌ Busy appearance with LAC bracket active

---

### D.2.4 CCIP Fire Control

**Appearance:**
```
        ▼

    ----◉----

        ▲
```

**Description:**
- Advanced fire control reticle
- Center circle with direction markers
- Optimized for CCIP pipper display
- High-precision aiming markings
- Integrated ballistic reference

**Best For:**
- Advanced fire control operations
- Long range precision fire
- Integration with LAC and CCIP
- Experienced operators

**Advantages:**
- ✓ Designed for fire control system integration
- ✓ Clear CCIP pipper visibility
- ✓ Direction markers aid orientation
- ✓ Professional appearance
- ✓ Works well with LAC bracket

**Disadvantages:**
- ❌ More complex (requires training)
- ❌ May be overwhelming for new operators

---

### D.2.5 Mil-Dot Ranging

**Appearance:**
```
        •  |  •
           |
    • -----+----- •
           |
        •  |  •
```

**Description:**
- Crosshair with ranging dots
- Dots spaced at 1 mil intervals
- Center intersection marks aim point
- 4 dots per quadrant
- Professional sniper/marksman reticle

**Best For:**
- Range estimation
- Long range precision fire
- Trained marksmen/snipers
- Known target sizes
- Advanced ballistic operations

**Advantages:**
- ✓ Allows manual range estimation (mil formula)
- ✓ Provides holdover/holdunder reference
- ✓ Professional military standard
- ✓ Multiple aiming references
- ✓ Excellent for long range

**Disadvantages:**
- ❌ Requires training to use effectively
- ❌ Mil formula requires target size knowledge
- ❌ More complex appearance
- ❌ Dots may obscure small targets

**Mil-Dot Formula:**
```
Range (meters) = Target Size (meters) × 1000 / Mils
```

**Example:**
- Target: 2.5m tall vehicle
- Target fills 5 mil-dots (top to bottom)
- Range = 2.5 × 1000 / 5 = **500 meters**

---

## D.3 RETICLE SELECTION GUIDE

### D.3.1 By Mission Type

| Mission Type | Recommended Reticle | Alternative |
|--------------|---------------------|-------------|
| General patrol | Circle-Dot | Tactical Crosshair |
| Close protection | Circle-Dot | Box Crosshair |
| Long range overwatch | Mil-Dot | CCIP Fire Control |
| Convoy escort | Box Crosshair | Circle-Dot |
| Area defense | Tactical Crosshair | CCIP Fire Control |
| Precision fire | CCIP Fire Control | Mil-Dot |
| Urban operations | Circle-Dot | Tactical Crosshair |
| Open terrain | Mil-Dot | Tactical Crosshair |

### D.3.2 By Target Type

| Target Type | Recommended Reticle | Reason |
|-------------|---------------------|--------|
| Stationary vehicle | Box Crosshair | Clear reference frame |
| Moving vehicle | Circle-Dot | Size reference, lead estimation |
| Personnel | Circle-Dot | Fast acquisition, clear center |
| Long range targets | Mil-Dot | Range estimation capability |
| Multiple targets | Box Crosshair | Frame-by-frame engagement |
| Small/distant targets | CCIP Fire Control | High precision markings |

### D.3.3 By Operator Experience

**New Operators (0-10 hours):**
- Primary: **Circle-Dot** (simplest, most intuitive)
- Practice with: Tactical Crosshair

**Intermediate Operators (10-50 hours):**
- Primary: **Box Crosshair or Tactical Crosshair**
- Practice with: CCIP Fire Control

**Advanced Operators (>50 hours):**
- Mission-dependent selection
- Comfortable with all 5 types
- May prefer: **Mil-Dot or CCIP Fire Control** for specialized roles

---

## D.4 RETICLE WITH FIRE CONTROL ELEMENTS

### D.4.1 Reticle + CCIP Pipper

**When LRF Range Active:**

All reticles display **CCIP pipper** (Continuously Computed Impact Point):

```
    [Any Reticle Type]

             ⊕  ← CCIP Pipper (computed impact point)

```

**Aim Method:**
- Place CCIP pipper on target (NOT the reticle center)
- CCIP pipper shows actual impact point
- Reticle center is reference only (after ranging)

### D.4.2 Reticle + LAC Bracket

**When LAC Active (Tracking + LAC ON):**

```
    [Any Reticle Type]

             ⊕  ← CCIP Pipper
                        ┐
                        │ LAC Bracket
                        │ (lead angle)
                        ┘
```

**Aim Method:**
- Keep reticle center on target
- LAC bracket shows computed lead
- CCIP pipper auto-adjusted for lead
- Follow CCIP pipper for firing solution

**Visual Aid:** LAC bracket size indicates lead angle magnitude

---

## D.5 CUSTOMIZATION OPTIONS

### D.5.1 Reticle Color

**Available Colors (Personalize Colors menu):**
- Green (default, best night visibility)
- Red (less eye strain, desert conditions)
- White (high contrast, overcast/fog)
- Yellow (fog/haze penetration)
- Cyan (thermal camera operations)
- Magenta
- Orange
- Blue

**Color Selection Guide:**
```
Lighting Condition     Recommended Color
─────────────────────────────────────────
Bright daylight        White or Yellow
Overcast/fog           Yellow or White
Twilight/dusk          Green
Night operations       Green or Red
Thermal camera         Cyan or Green
Desert (bright)        Red
```

### D.5.2 Reticle Brightness

**Note:** Brightness option removed from Main Menu (system-controlled)

**Auto-Brightness:**
- System adjusts based on camera exposure
- High ambient light → Brighter reticle
- Low ambient light → Dimmer reticle
- Automatic optimization for visibility

---

## D.6 RETICLE SELECTION PROCEDURE

### D.6.1 Changing Reticle Type

```
STEP 1: Access Menu
☐ Press MENU button (Control Panel)

STEP 2: Navigate to Personalize Reticle
☐ Press DOWN button to highlight "Personalize Reticle"
☐ Press VAL button to enter

STEP 3: Select Reticle Type
☐ Current reticle shown (highlighted)
☐ Press UP/DOWN to cycle through 5 types:
   • Circle-Dot Reticle
   • Box Crosshair
   • Tactical Crosshair
   • CCIP Fire Control
   • Mil-Dot Ranging
☐ Preview displayed in real-time

STEP 4: Apply Selection
☐ Press VAL button to apply
☐ Reticle changes immediately

STEP 5: Exit Menu
☐ Highlight "Return ..."
☐ Press VAL or press MENU to exit
☐ New reticle active immediately
```

**Time Required:** 10-15 seconds

### D.6.2 Testing New Reticle

**After changing reticle:**
```
☐ Aim at known reference point
☐ Test at multiple zoom levels
☐ Test with CCIP pipper (range a target with LRF)
☐ Test with LAC active (if tracking available)
☐ Verify visibility in current lighting
☐ Adjust reticle color if needed
```

---

## D.7 TROUBLESHOOTING RETICLE ISSUES

### Issue 1: Reticle Not Visible

**Possible Causes:**
- Reticle color blends with background
- Camera overexposed (reticle too dim)
- Display issue

**Solutions:**
- Change reticle color (try White or Yellow)
- Adjust camera exposure if available
- Switch camera mode (day/thermal)

### Issue 2: Reticle Obscuring Target

**Possible Causes:**
- Wrong reticle type for target size
- Zoom level too high
- Reticle too complex for situation

**Solutions:**
- Switch to Circle-Dot (least obstruction)
- Zoom out slightly
- Use simpler reticle type

### Issue 3: Hard to Aim with Current Reticle

**Possible Causes:**
- Unfamiliar reticle type
- Reticle not suited for target/mission type
- Insufficient training

**Solutions:**
- Return to Circle-Dot or Tactical Crosshair (most familiar)
- Practice with new reticle on non-critical targets
- Choose reticle based on Section D.3 guide
- Get training on advanced reticles (Mil-Dot, CCIP)

---

## D.8 FIELD QUICK REFERENCE

**RETICLE SELECTION QUICK GUIDE:**

```
═══════════════════════════════════════════════════════
RETICLE TYPE          BEST FOR
═══════════════════════════════════════════════════════
Circle-Dot:           General ops, new operators
Box Crosshair:        Precision, stationary targets
Tactical Crosshair:   Military standard, all ranges
CCIP Fire Control:    Advanced fire control, LAC
Mil-Dot Ranging:      Long range, range estimation
═══════════════════════════════════════════════════════

TO CHANGE RETICLE:
1. MENU → Personalize Reticle
2. UP/DOWN to select
3. VAL to apply
4. Return ... to exit

REMEMBER:
- CCIP pipper = Actual impact point (aim here!)
- Reticle = Reference only (after ranging)
═══════════════════════════════════════════════════════
```

---

## D.9 RETICLE COMPARISON TABLE

| Feature | Circle-Dot | Box | Tactical | CCIP | Mil-Dot |
|---------|------------|-----|----------|------|---------|
| **Precision** | Medium | High | High | Very High | Very High |
| **Acquisition Speed** | Very Fast | Fast | Medium | Medium | Medium |
| **Target Obstruction** | Low | Medium | Medium | Medium | High |
| **Learning Curve** | Very Easy | Easy | Easy | Medium | Hard |
| **Long Range** | Fair | Good | Good | Excellent | Excellent |
| **Moving Targets** | Excellent | Good | Fair | Good | Fair |
| **With LAC** | Excellent | Good | Fair | Excellent | Poor |
| **Familiarity** | High | Medium | Very High | Low | Low |

**Rating Scale:** Poor < Fair < Medium < Good < High < Very High < Excellent

---

## D.10 TRAINING RECOMMENDATIONS

### D.10.1 Reticle Familiarization Drill

**Objective:** Become proficient with all 5 reticle types

**Procedure:**
```
1. Start with CIRCLE-DOT RETICLE (5 minutes)
   - Engage 5 stationary targets
   - Note acquisition speed

2. Switch to BOX CROSSHAIR (5 minutes)
   - Engage same 5 targets
   - Compare precision vs Circle-Dot

3. Switch to TACTICAL CROSSHAIR (5 minutes)
   - Engage mixed targets
   - Practice with traditional military reticle

4. Switch to CCIP FIRE CONTROL (10 minutes)
   - Engage with CCIP pipper
   - Practice with LAC if available
   - Note integration with fire control

5. Switch to MIL-DOT RANGING (10 minutes)
   - Practice range estimation
   - Engage targets at known ranges
   - Practice mil-dot formula

Total Time: 35 minutes
```

### D.10.2 Recommended Training Progression

**Week 1-2:** Circle-Dot and Tactical Crosshair only
**Week 3-4:** Add Box Crosshair and CCIP Fire Control
**Week 5+:** Introduce Mil-Dot Ranging for qualified operators

**Master 2-3 reticle types thoroughly rather than being mediocre with all 5**

---

**END OF APPENDIX D**
