# APPENDIX F: SYSTEM SPECIFICATIONS

**Purpose:** Complete technical specifications for RCWS system and components

---

## F.1 SYSTEM OVERVIEW

### F.1.1 System Classification

**System Type:** Remote Controlled Weapon Station (RCWS)
**Configuration:** Single-operator station
**Weapon Class:** Medium caliber (7.62mm - 12.7mm compatible)
**Mounting:** Vehicle turret or fixed platform
**Operation:** Day/night, all-weather capable

---

## F.2 GIMBAL SYSTEM

### F.2.1 Motion Specifications

| Parameter | Specification |
|-----------|---------------|
| **Azimuth Range** | 360° continuous rotation |
| **Elevation Range** | -20° to +60° |
| **Azimuth Speed (Manual)** | 0.1° to 60°/sec variable |
| **Elevation Speed (Manual)** | 0.1° to 30°/sec variable |
| **Azimuth Acceleration** | 100°/sec² max |
| **Elevation Acceleration** | 50°/sec² max |
| **Positioning Accuracy** | ±0.1° |
| **Repeatability** | ±0.05° |

### F.2.2 Azimuth Servo Drive

| Parameter | Specification |
|-----------|---------------|
| **Motor Type** | Brushless AC servo |
| **Power** | 400W continuous, 800W peak |
| **Torque** | 5 Nm continuous, 10 Nm peak |
| **Operating Voltage** | 24-48 VDC |
| **Encoder Resolution** | 20-bit absolute (1,048,576 counts/rev) |
| **Operating Temperature** | Motor: -40°C to +85°C |
| **Driver Temperature** | -20°C to +60°C |
| **Protection** | IP54 (dust and splash resistant) |

### F.2.3 Elevation Servo Drive

| Parameter | Specification |
|-----------|---------------|
| **Motor Type** | Brushless AC servo |
| **Power** | 300W continuous, 600W peak |
| **Torque** | 4 Nm continuous, 8 Nm peak |
| **Operating Voltage** | 24-48 VDC |
| **Encoder Resolution** | 20-bit absolute |
| **Operating Temperature** | Motor: -40°C to +85°C |
| **Driver Temperature** | -20°C to +60°C |
| **Protection** | IP54 |

---

## F.3 CAMERA SYSTEMS

### F.3.1 Day Camera

| Parameter | Specification |
|-----------|---------------|
| **Sensor Type** | 1/2.8" CMOS, progressive scan |
| **Resolution** | 1920×1080 (Full HD) |
| **Frame Rate** | 30 fps (60 fps max) |
| **Optical Zoom** | 20× continuous |
| **Focal Length** | 4.3mm - 86mm (35mm equivalent) |
| **Horizontal FOV** | 60.7° (wide) to 3.3° (tele) |
| **Focus** | Auto/manual, minimum 1m |
| **Aperture** | F1.6 (wide) to F3.5 (tele) |
| **Video Output** | H.264 via RTSP (Ethernet) |
| **Control Interface** | HTTP API (Ethernet) |
| **Operating Temp** | -30°C to +60°C |
| **Power** | 12V DC, 5W max |

### F.3.2 Thermal Camera

| Parameter | Specification |
|-----------|---------------|
| **Detector Type** | Uncooled microbolometer |
| **Resolution** | 640×512 (VGA) |
| **Pixel Pitch** | 12 μm |
| **Spectral Range** | 8-14 μm (LWIR) |
| **Frame Rate** | 60 Hz |
| **NETD** | <50 mK @ 30°C |
| **Horizontal FOV** | 18° (wide) to 6° (narrow) |
| **Digital Zoom** | 2×, 4×, 8× |
| **LUT Palettes** | 13 (White Hot, Black Hot, Rainbow, etc.) |
| **Video Output** | H.264 via UDP (Ethernet) |
| **Control Interface** | Binary protocol via TCP |
| **Operating Temp** | -40°C to +60°C |
| **Power** | 5V DC, 1.5W typical |

---

## F.4 LASER RANGE FINDER

| Parameter | Specification |
|-----------|---------------|
| **Technology** | Class 1 eye-safe laser |
| **Wavelength** | 905 nm (invisible infrared) |
| **Range (Min)** | 50 meters |
| **Range (Max)** | 4000 meters (vehicle-sized target) |
| **Accuracy** | ±1 meter |
| **Measurement Rate** | Single shot or 5 Hz continuous |
| **Beam Divergence** | 3 mrad |
| **Operating Temp** | -40°C to +60°C |
| **Power** | 5V DC, 2W (idle), 5W (lasing) |
| **Interface** | RS-232 serial, 38400 baud |

---

## F.5 INERTIAL MEASUREMENT UNIT (IMU)

| Parameter | Specification |
|-----------|---------------|
| **Sensor Type** | 6-axis (3-axis gyro + 3-axis accel) |
| **Gyroscope Range** | ±2000°/sec |
| **Gyroscope Resolution** | 0.01°/sec |
| **Accelerometer Range** | ±16 g |
| **Accelerometer Resolution** | 0.001 g |
| **Update Rate** | 100 Hz |
| **Orientation Accuracy** | ±0.5° (roll/pitch), ±2° (yaw) |
| **Operating Temp** | -40°C to +85°C |
| **Power** | 5V DC, 150 mW |
| **Interface** | RS-232 serial, 115200 baud |

---

## F.6 WEAPON ACTUATOR

| Parameter | Specification |
|-----------|---------------|
| **Actuator Type** | Linear electric servo |
| **Stroke Length** | 100 mm |
| **Speed** | 10 mm/sec @ full load |
| **Force** | 500 N continuous, 1000 N peak |
| **Position Accuracy** | ±0.5 mm |
| **Encoder** | Absolute, 12-bit |
| **Operating Voltage** | 24V DC |
| **Power** | 50W continuous, 120W peak |
| **Operating Temp** | -20°C to +60°C |
| **Protection** | IP65 (dust-tight, water jet resistant) |

---

## F.7 CONTROL PANEL (PLC21)

| Parameter | Specification |
|-----------|---------------|
| **Type** | Industrial PLC with HMI buttons |
| **Inputs** | 12 digital (buttons/switches) |
| **Outputs** | 8 digital (indicator LEDs) |
| **Display** | None (indicators only) |
| **Interface** | Modbus RTU via RS-485 |
| **Operating Voltage** | 24V DC |
| **Power Consumption** | 5W |
| **Operating Temp** | -10°C to +50°C |
| **Protection** | IP54 |

### F.7.1 Control Panel Buttons/Switches

| Control | Type | Function |
|---------|------|----------|
| **MENU** | Momentary button | Open/close main menu |
| **UP** | Momentary button | Navigate menu up |
| **DOWN** | Momentary button | Navigate menu down |
| **VAL** | Momentary button | Validate/select menu option |
| **STATION ON/OFF** | Toggle switch | Enable/disable weapon station |
| **MASTER ARM** | Toggle switch | Arm weapon system |
| **AMMO LOADED** | Indicator switch | Ammunition loaded status |
| **FIRE MODE** | Rotary switch | Single/Burst selection |
| **SPEED** | Rotary switch | Gimbal speed: Slow/Medium/Fast |
| **STABILIZATION** | Toggle switch | Enable/disable gyro stabilization |
| **E-STOP** | Mushroom button | Emergency stop (latching) |

---

## F.8 GIMBAL STATION HARDWARE (PLC42)

| Parameter | Specification |
|-----------|---------------|
| **Type** | Industrial I/O module |
| **Digital Inputs** | 8 (limit sensors, E-Stop echo) |
| **Analog Inputs** | 4 (temperature, pressure sensors) |
| **Interface** | Modbus RTU via RS-485 |
| **Operating Voltage** | 24V DC |
| **Power Consumption** | 3W |
| **Operating Temp** | -20°C to +60°C |
| **Protection** | IP65 |

---

## F.9 JOYSTICK

| Parameter | Specification |
|-----------|---------------|
| **Type** | Industrial flight stick (HOTAS-style) |
| **Axes** | 2 (X/Y analog stick) |
| **Analog Resolution** | 12-bit (4096 levels per axis) |
| **Buttons** | 20 programmable |
| **Hat Switch (D-Pad)** | 1 (8-direction) |
| **Interface** | USB HID |
| **Force Feedback** | None (passive spring return) |
| **Operating Temp** | 0°C to +50°C |

---

## F.10 MAIN PROCESSOR

| Parameter | Specification |
|-----------|---------------|
| **Platform** | Embedded Linux PC (industrial) |
| **CPU** | Intel Core i5 or equivalent (quad-core, 2.5+ GHz) |
| **RAM** | 8 GB DDR4 |
| **Storage** | 128 GB SSD (industrial-grade) |
| **Operating System** | Linux (Ubuntu 20.04 LTS or similar) |
| **Display Output** | HDMI 1.4 (1920×1080 @ 60Hz) |
| **Network Interfaces** | 2× Gigabit Ethernet |
| **Serial Ports** | 6× USB-to-Serial (FTDI chipset) |
| **CAN Bus** | 1× SocketCAN interface |
| **USB Ports** | 4× USB 3.0 |
| **Operating Temp** | -20°C to +60°C |
| **Power** | 24V DC input, 60W typical, 100W max |
| **Protection** | Fanless, IP50 (enclosed chassis) |

---

## F.11 POWER SYSTEM

### F.11.1 Power Distribution

| Component | Voltage | Current (Typ) | Current (Max) | Power (Max) |
|-----------|---------|---------------|---------------|-------------|
| Main Processor | 24V DC | 2.5 A | 4.2 A | 100 W |
| Azimuth Servo | 48V DC | 8 A | 17 A | 800 W |
| Elevation Servo | 48V DC | 6 A | 13 A | 600 W |
| Weapon Actuator | 24V DC | 2 A | 5 A | 120 W |
| Day Camera | 12V DC | 0.4 A | 0.5 A | 6 W |
| Thermal Camera | 5V DC | 0.3 A | 0.4 A | 2 W |
| Laser Range Finder | 5V DC | 0.4 A | 1 A | 5 W |
| IMU | 5V DC | 0.03 A | 0.05 A | 0.25 W |
| PLC21 (Control Panel) | 24V DC | 0.2 A | 0.3 A | 7 W |
| PLC42 (Gimbal Station) | 24V DC | 0.15 A | 0.2 A | 5 W |
| **TOTAL** | - | - | - | **~1650 W** |

### F.11.2 Power Input

| Parameter | Specification |
|-----------|---------------|
| **Primary Input** | 24V DC nominal (vehicle power) |
| **Voltage Range** | 18V - 32V DC |
| **Peak Current** | 70 A @ 24V |
| **Protection** | Reverse polarity, overcurrent, overvoltage |
| **Connector** | NATO slave receptacle or equivalent |

### F.11.3 Battery Backup (Optional)

| Parameter | Specification |
|-----------|---------------|
| **Type** | Lithium-ion battery pack |
| **Capacity** | 500 Wh (20 Ah @ 24V) |
| **Runtime** | 30 minutes @ full load, 2 hours @ idle |
| **Charge Time** | 4 hours |

---

## F.12 ENVIRONMENTAL SPECIFICATIONS

### F.12.1 Operating Environment

| Parameter | Specification |
|-----------|---------------|
| **Operating Temperature** | -30°C to +55°C |
| **Storage Temperature** | -40°C to +70°C |
| **Humidity** | 5% to 95% RH (non-condensing) |
| **Altitude** | Sea level to 4000m |
| **Shock** | MIL-STD-810G, Method 516.6 |
| **Vibration** | MIL-STD-810G, Method 514.6 |
| **IP Rating** | IP54 (system enclosure) |
| **EMI/EMC** | MIL-STD-461F |

### F.12.2 Operational Conditions

**Suitable for:**
- Desert (high temperature, dust, sand)
- Arctic (low temperature, ice, snow)
- Maritime (salt spray, high humidity)
- Urban (electromagnetic interference)
- Tropical (high temperature and humidity)

**Limitations:**
- Thermal camera degrades in extreme heat (>50°C ambient)
- Day camera requires periodic lens cleaning in dusty environments
- LRF range reduced in fog, heavy rain, or smoke
- Servo performance reduced below -20°C (slower acceleration)

---

## F.13 COMMUNICATION SPECIFICATIONS

### F.13.1 Internal Communication

| Bus Type | Devices | Data Rate | Cable Type | Max Length |
|----------|---------|-----------|------------|------------|
| **Modbus RTU** | PLC21, PLC42, Actuator | 115200 baud | Shielded twisted pair | 1200m |
| **Ethernet** | Day Camera, Thermal Camera | 1 Gbps | CAT6 shielded | 100m |
| **CAN Bus** | Servos (Az/El) | 500 kbps | CAN cable (twisted pair) | 100m |
| **RS-232** | LRF, IMU | 38400-115200 baud | Shielded cable | 15m |
| **USB** | Joystick | 480 Mbps (USB 2.0) | USB cable | 5m |

### F.13.2 External Communication (Optional)

| Interface | Purpose | Protocol |
|-----------|---------|----------|
| **Ethernet** | Remote monitoring/control | TCP/IP |
| **Serial (RS-422)** | Command link to vehicle C2 | Custom protocol |
| **Wireless (optional)** | Remote video feed | Encrypted 5 GHz WiFi |

---

## F.14 PHYSICAL SPECIFICATIONS

### F.14.1 Gimbal Assembly

| Parameter | Specification |
|-----------|---------------|
| **Weight (Gimbal Only)** | 45 kg |
| **Weight (with Weapon)** | 65 kg (12.7mm MG) |
| **Dimensions (Stowed)** | 600mm × 500mm × 400mm (L×W×H) |
| **Dimensions (Elevated)** | 600mm × 500mm × 800mm |
| **Mounting Interface** | NATO standard turret ring or custom adapter |
| **Material** | Aluminum alloy frame, steel weapon mount |

### F.14.2 Control Panel

| Parameter | Specification |
|-----------|---------------|
| **Dimensions** | 250mm × 200mm × 80mm |
| **Weight** | 2 kg |
| **Mounting** | Panel mount or console mount |

### F.14.3 Main Processor Unit

| Parameter | Specification |
|-----------|---------------|
| **Dimensions** | 300mm × 200mm × 100mm |
| **Weight** | 5 kg |
| **Mounting** | Rack mount (19") or shelf mount |

---

## F.15 RELIABILITY AND MAINTAINABILITY

| Parameter | Specification |
|-----------|---------------|
| **MTBF (Mean Time Between Failures)** | >2000 hours |
| **MTTR (Mean Time To Repair)** | <2 hours (component replacement) |
| **Service Life** | 10 years (with scheduled maintenance) |
| **Calibration Interval** | 12 months (boresight/zeroing check) |
| **Maintenance Level** | Operator (basic), Field (component swap), Depot (overhaul) |

---

## F.16 COMPLIANCE AND STANDARDS

| Standard | Description |
|----------|-------------|
| **MIL-STD-810G** | Environmental engineering |
| **MIL-STD-461F** | Electromagnetic interference |
| **MIL-STD-1472G** | Human engineering |
| **MIL-STD-1553** | Data bus (if applicable) |
| **ISO 9001** | Quality management |
| **IEC 61508** | Functional safety |
| **EN 55011** | Industrial EMC |

---

## F.17 SAFETY CERTIFICATIONS

| Certification | Component |
|---------------|-----------|
| **CE Mark** | Electronic components (EU) |
| **FCC Part 15** | Radio frequency devices (USA) |
| **IEC 60825-1** | Laser safety (LRF Class 1) |
| **UL 508** | Industrial control equipment |

---

**END OF APPENDIX F**
