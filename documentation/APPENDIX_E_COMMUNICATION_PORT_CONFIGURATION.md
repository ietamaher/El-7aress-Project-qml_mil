# APPENDIX E: COMMUNICATION PORT CONFIGURATION

**Purpose:** Technical reference for system communication ports and device connections

---

## E.1 OVERVIEW

### E.1.1 System Architecture

The RCWS uses a **distributed architecture** with multiple communication ports connecting hardware devices to the main processor:

```
┌──────────────────────────────────────┐
│      MAIN PROCESSOR (Linux PC)       │
│   Qt/QML Application                 │
└──────────────────────────────────────┘
            │
            │ Multiple serial/network ports
            │
    ┌───────┴───────┬────────────┬──────────┬──────────┐
    │               │            │          │          │
┌───▼───┐   ┌───────▼────┐  ┌────▼────┐ ┌──▼───┐  ┌──▼───┐
│ PLC21 │   │ PLC42      │  │ Cameras │ │Servos│  │ LRF  │
│Control│   │Gimbal Stn  │  │Day/Night│ │Az/El │  │      │
│ Panel │   │            │  │         │ │      │  │      │
└───────┘   └────────────┘  └─────────┘ └──────┘  └──────┘
```

**Port Types:**
- Serial (RS-232, RS-485)
- Ethernet (TCP/IP, UDP)
- CAN Bus
- Video (MIPI CSI-2, H.264 stream)

---

## E.2 PORT ASSIGNMENTS

### E.2.1 Serial Ports

| Port | Device | Baud Rate | Data Bits | Stop Bits | Parity | Protocol |
|------|--------|-----------|-----------|-----------|--------|----------|
| **/dev/ttyUSB0** | PLC21 (Control Panel) | 115200 | 8 | 1 | None | Modbus RTU |
| **/dev/ttyUSB1** | PLC42 (Gimbal Station) | 115200 | 8 | 1 | None | Modbus RTU |
| **/dev/ttyUSB2** | Azimuth Servo Drive | 115200 | 8 | 1 | None | Custom |
| **/dev/ttyUSB3** | Elevation Servo Drive | 115200 | 8 | 1 | None | Custom |
| **/dev/ttyUSB4** | Laser Range Finder | 38400 | 8 | 1 | None | ASCII |
| **/dev/ttyUSB5** | IMU/Gyroscope | 115200 | 8 | 1 | None | Binary |
| **/dev/ttyUSB6** | Weapon Actuator | 115200 | 8 | 1 | Even | Modbus RTU |

**Notes:**
- All USB-to-Serial converters use FTDI chipset
- Ports may vary depending on USB enumeration order
- Use udev rules for persistent device naming (see Section E.5)

### E.2.2 Network Ports

| IP Address | Device | Port | Protocol | Purpose |
|------------|--------|------|----------|---------|
| **192.168.1.10** | Day Camera | 80 | HTTP | Control API |
| **192.168.1.10** | Day Camera | 554 | RTSP | Video stream |
| **192.168.1.11** | Thermal Camera | 9876 | UDP | Video stream (H.264) |
| **192.168.1.11** | Thermal Camera | 9877 | TCP | Control commands |
| **192.168.1.20** | Radar (optional) | 5000 | UDP | Plot data |
| **192.168.1.100** | Main Processor | - | - | System host |

**Network Configuration:**
- Subnet: 192.168.1.0/24
- Gateway: 192.168.1.1 (if external network needed)
- DNS: Not required (isolated network)

### E.2.3 CAN Bus

| CAN ID | Device | Bitrate | Purpose |
|--------|--------|---------|---------|
| **0x100** | Azimuth Servo | 500 kbps | Position/velocity commands |
| **0x200** | Elevation Servo | 500 kbps | Position/velocity commands |
| **0x300** | Weapon Actuator | 500 kbps | Position control |

**CAN Interface:** can0 (SocketCAN)

---

## E.3 DEVICE CONNECTION DETAILS

### E.3.1 Control Panel (PLC21)

**Connection Type:** Serial (Modbus RTU)
**Port:** /dev/ttyUSB0
**Settings:** 115200 8N1
**Modbus Address:** 1

**Communication Pattern:**
- Polling interval: 50ms
- Read coils (buttons/switches): Function Code 01
- Write coils (indicators): Function Code 05
- Read input registers (analog): Function Code 04

**Typical Modbus Frames:**
```
Read buttons (10 coils from address 0):
Request:  01 01 00 00 00 0A [CRC]
Response: 01 01 02 xx xx [CRC]  (xx = button states)

Set indicator LED (address 10, ON):
Request:  01 05 00 0A FF 00 [CRC]
Response: 01 05 00 0A FF 00 [CRC]  (echo)
```

### E.3.2 Gimbal Station Hardware (PLC42)

**Connection Type:** Serial (Modbus RTU)
**Port:** /dev/ttyUSB1
**Settings:** 115200 8N1
**Modbus Address:** 2

**Communication Pattern:**
- Polling interval: 100ms
- Read sensors (temperature, pressure): Function Code 03
- Read digital inputs (limit switches, E-Stop): Function Code 02

### E.3.3 Day Camera

**Connection Type:** Ethernet (HTTP/RTSP)
**IP Address:** 192.168.1.10
**Control API:** HTTP on port 80
**Video Stream:** RTSP on port 554

**Control Commands (HTTP):**
```
Set Zoom Position:
GET http://192.168.1.10/api/v1/camera/zoom?position=0.5

Enable Autofocus:
GET http://192.168.1.10/api/v1/camera/autofocus?enable=true

Get Camera Status:
GET http://192.168.1.10/api/v1/camera/status
Response: {"zoom":0.5,"focus":2048,"connected":true}
```

**Video Stream (RTSP):**
```
Stream URL: rtsp://192.168.1.10:554/stream
Codec: H.264
Resolution: 1920x1080
Frame Rate: 30 fps
```

### E.3.4 Thermal Camera

**Connection Type:** Ethernet (UDP for video, TCP for control)
**IP Address:** 192.168.1.11
**Video Port:** 9876 (UDP)
**Control Port:** 9877 (TCP)

**Control Protocol:** Binary commands over TCP

**Video Stream:** Raw H.264 NAL units over UDP

### E.3.5 Laser Range Finder

**Connection Type:** Serial (ASCII protocol)
**Port:** /dev/ttyUSB4
**Settings:** 38400 8N1

**Command Format:**
```
Trigger range measurement:
Send: "TRIG\r\n"
Receive: "DIST:1234.5\r\n"  (distance in meters)

Get status:
Send: "STAT\r\n"
Receive: "OK:READY:TEMP:25.3\r\n"
```

### E.3.6 Servo Drives (Azimuth & Elevation)

**Connection Type:** Serial + CAN Bus (dual interface)
**Serial Ports:** /dev/ttyUSB2, /dev/ttyUSB3
**Serial Settings:** 115200 8N1
**CAN IDs:** 0x100 (Az), 0x200 (El)
**CAN Bitrate:** 500 kbps

**Primary Communication:** CAN Bus (position/velocity control)
**Secondary Communication:** Serial (configuration, diagnostics)

---

## E.4 TROUBLESHOOTING PORT ISSUES

### E.4.1 Serial Port Not Found

**Symptoms:**
- Device shows as "Not Connected"
- Error: "Failed to open /dev/ttyUSBx"

**Diagnosis:**
```bash
# List all USB serial devices
ls -l /dev/ttyUSB*

# Check dmesg for USB events
dmesg | grep tty

# Verify FTDI driver loaded
lsmod | grep ftdi
```

**Solutions:**
1. Check USB cable connection
2. Verify USB-to-Serial converter power LED
3. Check USB port on PC (try different port)
4. Verify udev permissions (see Section E.5.1)
5. Replug USB device (may re-enumerate)

### E.4.2 Network Device Unreachable

**Symptoms:**
- Camera shows "Not Connected"
- Timeout errors

**Diagnosis:**
```bash
# Ping device
ping -c 4 192.168.1.10

# Check network interface status
ip addr show

# Verify port open
nc -zv 192.168.1.10 80

# Check for firewall rules
iptables -L
```

**Solutions:**
1. Check Ethernet cable connection
2. Verify device power (POE or separate)
3. Check IP configuration (static IP assigned?)
4. Verify network interface UP
5. Check for IP conflicts (duplicate IPs)

### E.4.3 Modbus Communication Errors

**Symptoms:**
- CRC errors in logs
- Timeout waiting for response
- Invalid data received

**Diagnosis:**
```bash
# Monitor Modbus traffic (requires modbus tools)
modbus_client -a 1 -r /dev/ttyUSB0 -b 115200 -t 01 -s 0 -n 10

# Check serial port settings
stty -F /dev/ttyUSB0 -a
```

**Common Causes:**
- Wrong baud rate
- Cable too long (>15m for RS-485)
- Electrical noise on line
- Multiple devices same Modbus address
- Missing termination resistors (RS-485)

**Solutions:**
1. Verify baud rate matches device
2. Use shielded cable for long runs
3. Add termination resistors (120Ω at each end for RS-485)
4. Check ground connections
5. Reduce polling rate if bus overloaded

### E.4.4 CAN Bus Issues

**Symptoms:**
- CAN error frames
- Servo commands ignored
- "Bus off" errors

**Diagnosis:**
```bash
# Check CAN interface status
ip -details link show can0

# Monitor CAN traffic
candump can0

# Check error counters
ip -s link show can0
```

**Solutions:**
1. Verify CAN bitrate (must match all devices: 500 kbps)
2. Check CAN termination (120Ω at each end)
3. Verify CAN high/low not swapped
4. Check for loose connectors
5. Reduce bus load (slower update rate)

---

## E.5 SYSTEM CONFIGURATION

### E.5.1 Udev Rules for Persistent Device Names

**Problem:** USB serial ports may enumerate in different order after reboot

**Solution:** Create udev rules based on serial number

**File:** `/etc/udev/rules.d/99-rcws-devices.rules`

```bash
# PLC21 Control Panel
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="PLC21001", SYMLINK+="tty_plc21"

# PLC42 Gimbal Station
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="PLC42001", SYMLINK+="tty_plc42"

# Azimuth Servo
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="SERVO_AZ", SYMLINK+="tty_servo_az"

# Elevation Servo
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="SERVO_EL", SYMLINK+="tty_servo_el"

# Laser Range Finder
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="LRF_001", SYMLINK+="tty_lrf"

# IMU
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="IMU_001", SYMLINK+="tty_imu"

# Weapon Actuator
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="ACTUATOR", SYMLINK+="tty_actuator"

# Set permissions (allow user access)
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", MODE="0666", GROUP="dialout"
```

**Apply rules:**
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

**Usage in application:**
```cpp
// Use persistent device names
QString plc21Port = "/dev/tty_plc21";  // Instead of /dev/ttyUSB0
```

### E.5.2 Network Configuration

**File:** `/etc/network/interfaces` (or NetworkManager config)

```bash
# Ethernet interface for RCWS devices
auto eth0
iface eth0 inet static
    address 192.168.1.100
    netmask 255.255.255.0
    # No gateway (isolated network)
```

**Apply:**
```bash
sudo systemctl restart networking
```

### E.5.3 CAN Bus Configuration

**File:** `/etc/network/interfaces.d/can0`

```bash
auto can0
iface can0 inet manual
    pre-up /sbin/ip link set can0 type can bitrate 500000
    up /sbin/ifconfig can0 up
    down /sbin/ifconfig can0 down
```

**Manual setup:**
```bash
sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up
```

---

## E.6 PORT MONITORING AND DIAGNOSTICS

### E.6.1 Serial Port Monitoring

**Monitor all traffic on a serial port:**
```bash
# Hex dump of serial port
xxd /dev/ttyUSB0

# Or use minicom
minicom -D /dev/ttyUSB0 -b 115200
```

### E.6.2 Network Traffic Capture

**Capture camera RTSP stream:**
```bash
tcpdump -i eth0 -w camera_traffic.pcap port 554

# Analyze with Wireshark
wireshark camera_traffic.pcap
```

### E.6.3 CAN Bus Logging

**Log all CAN traffic:**
```bash
candump -l can0

# Creates log file: candump-YYYY-MM-DD_HHMMSS.log
```

**Replay CAN traffic:**
```bash
canplayer -I candump-2023-10-15_143022.log
```

---

## E.7 PORT SECURITY

### E.7.1 Access Control

**Serial Ports:**
- Only authorized user should be in `dialout` group
- Application runs with minimal permissions
- No direct user access to serial terminals

**Network Ports:**
- Isolated network (no internet access)
- Firewall blocks external connections
- No remote access to camera/device web interfaces

### E.7.2 Firewall Rules

**File:** `/etc/iptables/rules.v4`

```bash
# Allow only RCWS device subnet
-A INPUT -s 192.168.1.0/24 -j ACCEPT

# Block all other incoming
-A INPUT -j DROP

# Allow outgoing to RCWS devices
-A OUTPUT -d 192.168.1.0/24 -j ACCEPT

# Block all other outgoing (no internet)
-A OUTPUT -j DROP
```

---

## E.8 QUICK REFERENCE

### E.8.1 Essential Commands

```bash
# List serial ports
ls -l /dev/ttyUSB*

# Test serial port
echo "test" > /dev/ttyUSB0

# Ping camera
ping 192.168.1.10

# Check CAN status
ip -s link show can0

# Restart CAN bus
sudo ip link set can0 down && sudo ip link set can0 up

# Monitor system logs
journalctl -u rcws-application -f

# Check device enumeration
dmesg | tail -50
```

### E.8.2 Common Port Settings

| Device Type | Typical Baud Rate | Data Format |
|-------------|-------------------|-------------|
| Modbus RTU | 9600, 19200, 115200 | 8N1 or 8E1 |
| GPS/NMEA | 4800, 9600 | 8N1 |
| Servo Drives | 115200, 1000000 | 8N1 |
| Industrial PLCs | 9600, 115200 | 8N1 or 8E1 |

---

**END OF APPENDIX E**
