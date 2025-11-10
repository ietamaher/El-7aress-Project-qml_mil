# Hardware Test Scripts - El 7arress RCWS

This directory contains Python test scripts for all serial and Modbus devices used in the RCWS system. Each script automatically loads configuration from `../config/devices.json`.

## 📋 Requirements

### Python Packages

```bash
# Install required packages
pip3 install pyserial pymodbus

# Or use requirements.txt (if provided)
pip3 install -r requirements.txt
```

### Permissions

```bash
# Add your user to the dialout group for serial port access
sudo usermod -aG dialout $USER

# Log out and log back in for changes to take effect
# Or use: sudo chmod 666 /dev/ttyUSB* (temporary, not recommended)
```

## 📝 Test Scripts

### 1. IMU Tester - `imu_tester.py`

Tests the MicroStrain 3DM-GX3-25 IMU via serial communication.

**Reads:**
- Roll, Pitch, Yaw (Euler angles)
- Angular rates (X, Y, Z)

**Configuration:** `config/devices.json` → `imu`

**Usage:**
```bash
cd hardware_tests
python3 imu_tester.py
```

**Expected Output:**
```
Roll:    -0.25° | Pitch:     2.13° | Yaw:   180.45° | RateX:     0.12°/s | RateY:     0.05°/s | RateZ:    -0.03°/s
```

---

### 2. LRF Tester - `lrf_tester.py`

Tests the Laser Range Finder via serial communication.

**Reads:**
- Distance measurements (50m - 4000m range)

**Configuration:** `config/devices.json` → `lrf`

**Usage:**
```bash
python3 lrf_tester.py
```

**Note:** You may need to adjust `COMMAND_GET_DISTANCE` and `parse_distance()` based on your specific LRF model.

---

### 3. PLC21 Tester - `plc21_tester.py`

Tests the PLC21 control panel via Modbus RTU.

**Reads:**
- Discrete inputs: 10 switches (ARM_GUN, LOAD_AMMO, ENABLE_STATION, HOME_POS, STAB, AUTH, CAM_SW, UP, DOWN, VAL)
- Holding registers: Speed, Fire Mode, Temperature

**Configuration:** `config/devices.json` → `plc.plc21`

**Usage:**
```bash
python3 plc21_tester.py
```

**Expected Output:**
```
Switches: 0010000100 | Pressed: [2, 6] | Registers: Speed=50 FireMode=0 Temp=25
```

---

### 4. PLC42 Tester - `plc42_tester.py`

Tests the PLC42 system PLC via Modbus RTU.

**Reads:**
- Discrete inputs: 8 sensors (UPPER_SENSOR, LOWER_SENSOR, EMERG_STOP, AMMO, IN1-3, SOLENOID)
- Holding registers: Solenoid mode, gimbal mode, speeds, directions

**Configuration:** `config/devices.json` → `plc.plc42`

**Usage:**
```bash
python3 plc42_tester.py
```

---

### 5. Servo Azimuth Tester - `servo_azimuth_tester.py`

Tests the azimuth servo driver via Modbus RTU.

**Reads:**
- Position, Velocity, Status, Alarms
- Motor temperature, Driver temperature

**Configuration:** `config/devices.json` → `servo.azimuth`

**Usage:**
```bash
python3 servo_azimuth_tester.py
```

**Note:** Adjust register addresses (`ADDR_POSITION`, etc.) based on your servo driver manual.

---

### 6. Servo Elevation Tester - `servo_elevation_tester.py`

Tests the elevation servo driver via Modbus RTU (same protocol as azimuth).

**Configuration:** `config/devices.json` → `servo.elevation`

**Usage:**
```bash
python3 servo_elevation_tester.py
```

---

### 7. Actuator Tester - `actuator_tester.py`

Tests the gun actuator/trigger control via serial communication.

**⚠️ WARNING:** DO NOT TEST WITH LIVE AMMUNITION

**Configuration:** `config/devices.json` → `actuator`

**Usage:**
```bash
python3 actuator_tester.py
```

---

### 8. Day Camera Tester - `day_camera_tester.py`

Tests the Sony FCB-EV7520A day camera via VISCA protocol.

**Tests:**
- Power on/off
- Zoom in/out
- Zoom position query

**Configuration:** `config/devices.json` → `video.dayCamera`

**Usage:**
```bash
python3 day_camera_tester.py
```

---

### 9. Night Camera Tester - `night_camera_tester.py`

Tests the FLIR Boson 640 thermal camera via TAU2/Boson protocol.

**Tests:**
- Serial number query
- FFC (Flat Field Correction)
- Digital zoom

**Configuration:** `config/devices.json` → `video.nightCamera`

**Usage:**
```bash
python3 night_camera_tester.py
```

**Note:** This is a simplified tester. For full protocol support, use the FLIR Boson SDK.

---

### 10. Radar Tester - `radar_tester.py`

Tests radar target tracking via NMEA RATTM sentences.

**Reads:**
- Target ID, Distance, Bearing, Speed, Course
- CPA (Closest Point of Approach), TCPA (Time to CPA)

**Configuration:** `config/devices.json` → `radar`

**Usage:**
```bash
python3 radar_tester.py
```

---

## 🔧 Troubleshooting

### "Permission denied" on serial port

```bash
# Check current permissions
ls -l /dev/ttyUSB*

# Add yourself to dialout group
sudo usermod -aG dialout $USER

# Log out and back in, or use:
newgrp dialout
```

### "Port not found" error

```bash
# List all serial devices
ls -l /dev/serial/by-id/

# Update config/devices.json with correct port paths
```

### "pymodbus not found"

```bash
pip3 install pymodbus
```

### Communication timeout

- Check baud rate in `config/devices.json`
- Verify correct port path
- Check cable connections
- Ensure device is powered on
- Try different USB port

### Wrong data / garbled output

- Verify baud rate matches device
- Check parity settings (especially for PLCs: even parity)
- Ensure correct serial protocol (8N1, 8E1, etc.)

---

## 📊 Testing Workflow

### 1. Quick Hardware Check (All Devices)

Create a shell script to test all devices sequentially:

```bash
#!/bin/bash
# test_all_devices.sh

echo "=== Testing IMU ==="
python3 imu_tester.py &
IMU_PID=$!
sleep 5
kill $IMU_PID

echo "=== Testing PLC21 ==="
python3 plc21_tester.py &
PLC21_PID=$!
sleep 5
kill $PLC21_PID

# ... continue for other devices
```

### 2. Continuous Monitoring

Run testers in separate terminal windows/tabs:

```bash
# Terminal 1: IMU
python3 imu_tester.py

# Terminal 2: PLC21
python3 plc21_tester.py

# Terminal 3: Servos
python3 servo_azimuth_tester.py

# Terminal 4: LRF
python3 lrf_tester.py
```

### 3. Integration Testing

Test device interactions:
1. Press buttons on PLC21 panel → Monitor with `plc21_tester.py`
2. Move gimbal manually → Monitor with `servo_azimuth_tester.py` and `servo_elevation_tester.py`
3. Point LRF at target → Monitor with `lrf_tester.py`

---

## 📦 Configuration File Structure

All testers read from `../config/devices.json`. Example structure:

```json
{
  "imu": {
    "port": "/dev/serial/by-id/usb-WCH2",
    "baudRate": 115200,
    "samplingRateHz": 100
  },
  "plc": {
    "plc21": {
      "port": "/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if00",
      "baudRate": 115200,
      "slaveId": 31,
      "parity": "even"
    }
  },
  "servo": {
    "azimuth": {
      "port": "/dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04",
      "baudRate": 230400,
      "slaveId": 2,
      "parity": "none"
    }
  }
}
```

---

## 🚀 Making Scripts Executable

```bash
chmod +x *.py
```

Then you can run scripts directly:

```bash
./imu_tester.py
./plc21_tester.py
```

---

## 📝 Protocol Documentation

### Serial Devices
- **IMU**: MicroStrain 3DM-GX3-25 binary protocol
- **LRF**: Custom ASCII protocol (device-specific)
- **Day Camera**: VISCA protocol (Sony standard)
- **Night Camera**: TAU2/Boson protocol (FLIR)
- **Radar**: NMEA 0183 RATTM sentences
- **Actuator**: Custom ASCII protocol (device-specific)

### Modbus RTU Devices
- **PLC21**: Slave ID 31, 115200 baud, even parity
- **PLC42**: Slave ID 31, 115200 baud, even parity
- **Servo Azimuth**: Slave ID 2, 230400 baud, no parity
- **Servo Elevation**: Slave ID 1, 230400 baud, no parity

---

## ⚠️ Safety Notes

1. **Actuator Testing**: NEVER test with live ammunition loaded
2. **Servo Testing**: Ensure gimbal has free range of motion before powering on
3. **Emergency Stop**: Always have PLC42 emergency stop accessible
4. **Thermal Camera**: Avoid pointing at sun or intense heat sources
5. **Electrical Safety**: Disconnect power before connecting/disconnecting devices

---

## 📞 Support

For issues or questions:
- Check device manuals in `documentation/` folder
- Review hardware architecture: `documentation/HARDWARE_ARCHITECTURE.md`
- Check system logs: `./logs/rcws.log`

---

**Last Updated:** 2025-01-10
**Version:** 1.0
