# APPENDIX B: ZONE CONFIGURATION FILE FORMAT

**Purpose:** Technical specification for zone configuration file format (JSON)

---

## B.1 FILE OVERVIEW

### B.1.1 File Format

**Format:** JSON (JavaScript Object Notation)
**Extension:** .json
**Encoding:** UTF-8
**Location:** `/config/zones/` or user-specified

### B.1.2 File Purpose

- Store zone definitions (No-Fire, No-Traverse, Scan Zones, TRPs)
- Allow save/load of mission-specific zone configurations
- Enable zone sharing between systems
- Provide backup of zone data

---

## B.2 FILE STRUCTURE

### B.2.1 Root Object

```json
{
  "version": "1.0",
  "createdDate": "2025-10-30T14:30:00Z",
  "modifiedDate": "2025-10-30T15:45:00Z",
  "description": "Mission: Convoy Defense, Location: Grid 38SMB",
  "noFireZones": [ ... ],
  "noTraverseZones": [ ... ],
  "autoSectorScanZones": [ ... ],
  "targetReferencePoints": [ ... ]
}
```

### B.2.2 Field Descriptions

**version:** File format version (string)
- Current version: "1.0"
- For compatibility checking

**createdDate:** ISO 8601 timestamp of file creation

**modifiedDate:** ISO 8601 timestamp of last modification

**description:** User-defined description (string, optional)
- Mission name, location, date, etc.

---

## B.3 NO-FIRE ZONES

### B.3.1 Structure

```json
"noFireZones": [
  {
    "id": 1,
    "description": "Civilian Village North",
    "azimuthStart": 350.0,
    "azimuthStop": 10.0,
    "elevationMin": -5.0,
    "elevationMax": 20.0,
    "rangeMin": 0.0,
    "rangeMax": 2000.0,
    "enabled": true
  },
  {
    "id": 2,
    "description": "Friendly Position East",
    "azimuthStart": 85.0,
    "azimuthStop": 95.0,
    "elevationMin": -10.0,
    "elevationMax": 15.0,
    "rangeMin": 0.0,
    "rangeMax": 1500.0,
    "enabled": true
  }
]
```

### B.3.2 Field Definitions

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| **id** | integer | 1-99 | Unique zone identifier |
| **description** | string | max 50 chars | User-friendly zone name |
| **azimuthStart** | float | 0.0-360.0 | Starting azimuth (degrees) |
| **azimuthStop** | float | 0.0-360.0 | Ending azimuth (degrees) |
| **elevationMin** | float | -20.0 to +60.0 | Minimum elevation (degrees) |
| **elevationMax** | float | -20.0 to +60.0 | Maximum elevation (degrees) |
| **rangeMin** | float | 0.0-4000.0 | Minimum range (meters) |
| **rangeMax** | float | 0.0-4000.0 | Maximum range (meters) |
| **enabled** | boolean | true/false | Zone active status |

### B.3.3 Special Cases

**Azimuth Crossing 0°:**
```json
{
  "azimuthStart": 350.0,
  "azimuthStop": 10.0
}
```
This defines a 20° zone crossing North (350°→360°→0°→10°)

**Omnidirectional Zone:**
```json
{
  "azimuthStart": 0.0,
  "azimuthStop": 360.0
}
```
Covers all azimuth angles (full 360°)

---

## B.4 NO-TRAVERSE ZONES

### B.4.1 Structure

```json
"noTraverseZones": [
  {
    "id": 1,
    "description": "Ship Mast",
    "azimuthStart": 178.0,
    "azimuthStop": 182.0,
    "elevationMin": 30.0,
    "elevationMax": 60.0,
    "enabled": true
  }
]
```

### B.4.2 Field Definitions

**Same as No-Fire Zones except:**
- No `rangeMin` or `rangeMax` fields (traverse restriction independent of range)

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| **id** | integer | 1-99 | Unique zone identifier |
| **description** | string | max 50 chars | Zone name (e.g., "Mast", "Antenna") |
| **azimuthStart** | float | 0.0-360.0 | Starting azimuth (degrees) |
| **azimuthStop** | float | 0.0-360.0 | Ending azimuth (degrees) |
| **elevationMin** | float | -20.0 to +60.0 | Minimum elevation (degrees) |
| **elevationMax** | float | -20.0 to +60.0 | Maximum elevation (degrees) |
| **enabled** | boolean | true/false | Zone active status |

---

## B.5 AUTO SECTOR SCAN ZONES

### B.5.1 Structure

```json
"autoSectorScanZones": [
  {
    "id": 1,
    "description": "Perimeter North",
    "azimuthStart": 315.0,
    "azimuthStop": 45.0,
    "elevation": 10.0,
    "scanRate": 5.0,
    "enabled": true
  },
  {
    "id": 2,
    "description": "Harbor Watch",
    "azimuthStart": 120.0,
    "azimuthStop": 240.0,
    "elevation": 5.0,
    "scanRate": 8.0,
    "enabled": true
  }
]
```

### B.5.2 Field Definitions

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| **id** | integer | 1-99 | Unique zone identifier |
| **description** | string | max 50 chars | Scan zone name |
| **azimuthStart** | float | 0.0-360.0 | Scan start azimuth (degrees) |
| **azimuthStop** | float | 0.0-360.0 | Scan end azimuth (degrees) |
| **elevation** | float | -20.0 to +60.0 | Fixed elevation for scan (degrees) |
| **scanRate** | float | 1.0-20.0 | Scan speed (degrees/second) |
| **enabled** | boolean | true/false | Zone active status |

### B.5.3 Scan Behavior

**Scan Pattern:** Gimbal slews from `azimuthStart` to `azimuthStop` at `scanRate`, then returns to start and repeats.

**Elevation:** Fixed during scan (horizontal scanning only)

---

## B.6 TARGET REFERENCE POINTS (TRPs)

### B.6.1 Structure

```json
"targetReferencePoints": [
  {
    "id": 1,
    "description": "North Building",
    "azimuth": 0.0,
    "elevation": 5.0,
    "enabled": true
  },
  {
    "id": 2,
    "description": "East Tower",
    "azimuth": 90.0,
    "elevation": 15.0,
    "enabled": true
  },
  {
    "id": 3,
    "description": "South Gate",
    "azimuth": 180.0,
    "elevation": 0.0,
    "enabled": true
  }
]
```

### B.6.2 Field Definitions

| Field | Type | Range | Description |
|-------|------|-------|-------------|
| **id** | integer | 1-99 | Unique TRP identifier |
| **description** | string | max 50 chars | TRP name (landmark, etc.) |
| **azimuth** | float | 0.0-360.0 | TRP azimuth (degrees) |
| **elevation** | float | -20.0 to +60.0 | TRP elevation (degrees) |
| **enabled** | boolean | true/false | TRP active status |

### B.6.3 TRP Scan Behavior

When TRPScan motion mode active:
- Gimbal slews to each enabled TRP in sequence (ID order)
- Operator cycles with Button 14 (next) / Button 16 (previous)

---

## B.7 COMPLETE EXAMPLE FILE

```json
{
  "version": "1.0",
  "createdDate": "2025-10-30T14:30:00Z",
  "modifiedDate": "2025-10-30T15:45:00Z",
  "description": "Mission: Convoy Defense, Grid 38SMB, Date: 2025-10-30",

  "noFireZones": [
    {
      "id": 1,
      "description": "Civilian Village North",
      "azimuthStart": 350.0,
      "azimuthStop": 10.0,
      "elevationMin": -5.0,
      "elevationMax": 20.0,
      "rangeMin": 0.0,
      "rangeMax": 2000.0,
      "enabled": true
    }
  ],

  "noTraverseZones": [
    {
      "id": 1,
      "description": "Ship Mast",
      "azimuthStart": 178.0,
      "azimuthStop": 182.0,
      "elevationMin": 30.0,
      "elevationMax": 60.0,
      "enabled": true
    }
  ],

  "autoSectorScanZones": [
    {
      "id": 1,
      "description": "Perimeter North",
      "azimuthStart": 315.0,
      "azimuthStop": 45.0,
      "elevation": 10.0,
      "scanRate": 5.0,
      "enabled": true
    },
    {
      "id": 2,
      "description": "Harbor Watch",
      "azimuthStart": 120.0,
      "azimuthStop": 240.0,
      "elevation": 5.0,
      "scanRate": 8.0,
      "enabled": true
    }
  ],

  "targetReferencePoints": [
    {
      "id": 1,
      "description": "North Building",
      "azimuth": 0.0,
      "elevation": 5.0,
      "enabled": true
    },
    {
      "id": 2,
      "description": "East Tower",
      "azimuth": 90.0,
      "elevation": 15.0,
      "enabled": true
    },
    {
      "id": 3,
      "description": "South Gate",
      "azimuth": 180.0,
      "elevation": 0.0,
      "enabled": true
    }
  ]
}
```

---

## B.8 FILE OPERATIONS

### B.8.1 Save Configuration

**Via Menu:**
1. Main Menu → Zone Definitions
2. Select "Save Configuration"
3. Enter filename (e.g., "mission_convoy_defense")
4. System saves to: `/config/zones/mission_convoy_defense.json`

**File automatically includes:**
- Current timestamp
- All defined zones (all types)
- Enabled/disabled status

### B.8.2 Load Configuration

**Via Menu:**
1. Main Menu → Zone Definitions
2. Select "Load Configuration"
3. Browse available .json files
4. Select file
5. System loads zones and overwrites current configuration

**Warning:** Load operation replaces all current zones

### B.8.3 File Naming Conventions

**Recommended Format:**
```
mission_[name]_[date].json

Examples:
mission_convoy_defense_20251030.json
mission_harbor_patrol_20251031.json
training_scenario_a.json
```

**Avoid:**
- Spaces in filenames (use underscores)
- Special characters (#, $, %, etc.)
- Extremely long names (>50 characters)

---

## B.9 FILE VALIDATION

### B.9.1 System Validation on Load

**System checks:**
- ✓ Valid JSON syntax
- ✓ Required fields present
- ✓ Values within valid ranges
- ✓ No duplicate zone IDs
- ✓ Version compatibility

**If validation fails:**
- Error message displayed
- File not loaded
- Current configuration retained

### B.9.2 Manual Validation Checklist

**Before loading file on different system:**

```
☐ JSON syntax valid (use validator tool)
☐ Version field present and correct
☐ All zone IDs unique within type
☐ Azimuth values: 0.0-360.0
☐ Elevation values: -20.0 to +60.0
☐ Range values: 0.0-4000.0 (if present)
☐ Scan rate values: 1.0-20.0 (if present)
☐ Description strings <50 characters
☐ Boolean fields: true or false (lowercase)
```

---

## B.10 BACKUP AND VERSIONING

### B.10.1 Backup Strategy

**Recommended:**
- Save configuration after each mission planning session
- Keep mission-specific configs separate
- Backup configs to external storage weekly
- Document which config used for each mission

**File Organization:**
```
/config/zones/
  ├── mission_20251030_convoy.json
  ├── mission_20251031_patrol.json
  ├── training_basic.json
  └── backup/
      ├── mission_20251030_convoy_backup.json
      └── mission_20251031_patrol_backup.json
```

### B.10.2 Version Control

**When creating new version:**
```json
{
  "version": "1.0",
  "description": "Mission Alpha - Version 2 (updated TRPs)",
  ...
}
```

Update `modifiedDate` and description to track changes

---

## B.11 TROUBLESHOOTING

### Issue: File Won't Load

**Check:**
- ☐ File extension is .json
- ☐ File is in correct directory
- ☐ JSON syntax valid (use online validator)
- ☐ No missing commas or brackets
- ☐ All quotes paired properly

### Issue: Zones Not Appearing After Load

**Check:**
- ☐ Zones have `"enabled": true`
- ☐ Zone IDs are unique
- ☐ Azimuth/elevation values within range
- ☐ File actually loaded (check confirmation message)

### Issue: Scan Zone Not Working

**Check:**
- ☐ `azimuthStart` and `azimuthStop` are different
- ☐ `scanRate` is reasonable (1.0-20.0)
- ☐ `elevation` within -20 to +60
- ☐ Motion mode set to AutoSectorScan
- ☐ Zone selected as active scan zone

---

**END OF APPENDIX B**
