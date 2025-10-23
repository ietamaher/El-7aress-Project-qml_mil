File 9: README.md
markdown# El 7arress RCWS - Remote Controlled Weapon Station

![Version](https://img.shields.io/badge/version-4.5-blue)
![Qt](https://img.shields.io/badge/Qt-6.x-green)
![License](https://img.shields.io/badge/license-Proprietary-red)

## 📋 Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Software Features](#software-features)
- [Build Instructions](#build-instructions)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [Development](#development)
- [License](#license)

---

## 🎯 Overview

**El 7arress RCWS** is a sophisticated Remote Controlled Weapon Station system developed for the Tunisian Ministry of Defense. The system provides advanced targeting, tracking, and fire control capabilities with a modern Qt/QML-based user interface.

### Key Capabilities

- **Dual Camera System**: Day (Sony) and Thermal (FLIR) cameras with automatic switching
- **Advanced Tracking**: VPI-based DCF (Discriminative Correlation Filter) object tracking
- **Object Detection**: YOLOv8 integration for automatic target detection
- **Precision Gimbal Control**: High-precision azimuth/elevation servo control
- **Ballistics Compensation**: Zeroing, windage, and lead angle compensation
- **Zone Management**: No-fire zones and no-traverse zones for safety
- **Multi-Reticle System**: 5 reticle types (Basic, Box Crosshair, Standard, Precision, Mil-Dot)

---

## 🏗️ System Architecture

### Software Architecture
```
┌─────────────────────────────────────────────────────────────┐
│                        QML UI Layer                         │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ OSD Overlay │  │ Zone Manager │  │  System Status   │  │
│  └─────────────┘  └──────────────┘  └──────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Controller Layer                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐ │
│  │OsdController │  │ZoneDefinition│  │StatusController  │ │
│  │              │  │  Controller  │  │                  │ │
│  └──────────────┘  └──────────────┘  └──────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  ViewModel Layer (MVVM)                     │
│                                                             │
│  ViewModels expose data to QML via Q_PROPERTY              │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  SystemStateModel                           │
│                                                             │
│  Central data aggregator - combines all device data        │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              Hardware Controllers                           │
│  ┌───────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│  │  Gimbal   │  │  Camera  │  │  Weapon  │  │Joystick  │  │
│  │Controller │  │Controller│  │Controller│  │Controller│  │
│  └───────────┘  └──────────┘  └──────────┘  └──────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Device Layer                             │
│  ┌──────────┐  ┌─────────┐  ┌─────┐  ┌─────┐  ┌────────┐  │
│  │ Cameras  │  │ Servos  │  │ IMU │  │ LRF │  │  PLCs  │  │
│  │(GStreamer│  │(Modbus) │  │     │  │     │  │        │  │
│  │   VPI)   │  │         │  │     │  │     │  │        │  │
│  └──────────┘  └─────────┘  └─────┘  └─────┘  └────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Design Patterns Used

- **MVVM (Model-View-ViewModel)**: Clean separation of UI and business logic
- **Observer Pattern**: Qt signals/slots for event-driven communication
- **Dependency Injection**: Controllers receive dependencies via setters
- **Repository Pattern**: Device classes encapsulate hardware access
- **State Machine**: Tracking phases (Acquisition → Lock Pending → Active Lock → Firing)

---

## 🔧 Hardware Components

### Sensors & Actuators

| Component | Type | Interface | Description |
|-----------|------|-----------|-------------|
| **Day Camera** | Sony FCB-EV7520A | Serial RS-232 | Visible spectrum camera, motorized zoom |
| **Night Camera** | FLIR Boson 640 | Serial RS-232 | Thermal imaging camera, 640×512 resolution |
| **IMU** | SST810 Inclinometer | Modbus RTU | 2-axis tilt sensor (roll/pitch) |
| **LRF** | Custom Laser Rangefinder | Serial RS-232 | 50m-4000m range |
| **Azimuth Servo** | Custom AC Servo | Modbus RTU | 360° continuous rotation |
| **Elevation Servo** | Custom AC Servo | Modbus RTU | -20° to +60° elevation |
| **Gun Actuator** | Linear Actuator | Serial RS-232 | Trigger control |
| **PLC (21 I/O)** | Custom PLC | Modbus RTU | Panel controls & indicators |
| **PLC (42 I/O)** | Custom PLC | Modbus RTU | System interlocks & safety |

### Video Processing

- **GStreamer Pipeline**: YUY2 capture → Crop → Scale → 1024×768 output
- **VPI (NVIDIA Vision Programming Interface)**: GPU-accelerated tracking
- **OpenCV**: Image format conversion (YUY2 → BGRA → BGR)
- **YOLO v8**: Object detection (optional, configurable)

---

## ✨ Software Features

### 1. **On-Screen Display (OSD)**
- Real-time gimbal position (azimuth/elevation)
- Camera FOV and zoom level
- System status (armed/safe, ammo, ready state)
- LRF distance
- Tracking status
- Reticle with offset for zeroing/lead angle
- Active alarms and warnings

### 2. **Tracking System**
- **Acquisition Mode**: User-adjustable box for target selection
- **Lock-On**: VPI DCF tracker initialization
- **Active Tracking**: Real-time target localization (30 FPS)
- **Coast Mode**: Prediction during temporary target loss
- **Firing Mode**: Hold position during weapon discharge

### 3. **Ballistics**
- **Zeroing**: Gun-camera boresight offset correction (±10° range)
- **Windage**: Crosswind compensation based on estimated wind speed
- **Lead Angle Compensation**: Moving target prediction (future implementation)

### 4. **Zone Definition**
- **No-Fire Zones**: Areas where weapon discharge is prohibited
- **No-Traverse Zones**: Areas gimbal cannot enter
- **Sector Scans**: Automated azimuth scanning patterns
- **TRP (Target Reference Points)**: Preset aim points

### 5. **Safety Features**
- Emergency stop button (hardware + software)
- Interlocks (station enabled, gun armed, etc.)
- Zone violation warnings
- Temperature monitoring (motors/drivers)
- Connection status for all devices

---

## 🛠️ Build Instructions

### Prerequisites

**Operating System**: Ubuntu 22.04 LTS (or compatible Linux)

**Required Packages**:
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-serialbus-dev \
    qt6-serialport-dev \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libopencv-dev \
    libyaml-cpp-dev
```

**NVIDIA Components** (for VPI tracking):
```bash
# Install CUDA Toolkit (if not already installed)
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt update
sudo apt install -y cuda

# Install VPI (Vision Programming Interface)
# Download from NVIDIA Developer website: https://developer.nvidia.com/embedded/vpi
sudo dpkg -i vpi-lib-2.3-cuda11-aarch64-l4t.deb
sudo dpkg -i vpi-dev-2.3-cuda11-aarch64-l4t.deb
```

**YOLOv8** (optional, for object detection):
```bash
# Download pre-trained ONNX model
wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8s.onnx
# Place in project root or specify path in code
```

### Build Steps

1. **Clone Repository** (if applicable):
```bash
git clone 
cd rcws-project
```

2. **Create Build Directory**:
```bash
mkdir build
cd build
```

3. **Configure CMake**:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

4. **Build**:
```bash
make -j$(nproc)
```

5. **Verify Build**:
```bash
./rcws_app --version
```

### Alternative: Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure project with Kit (Qt 6.x, GCC)
3. Build → Build All (Ctrl+B)
4. Run → Run (Ctrl+R)

---

## 📦 Installation

### System Installation
```bash
# Install built binary
sudo cp build/rcws_app /usr/local/bin/

# Install resources
sudo mkdir -p /usr/local/share/rcws
sudo cp -r resources/* /usr/local/share/rcws/

# Create configuration directory
sudo mkdir -p /etc/rcws
sudo cp config/default.yaml /etc/rcws/config.yaml

# Set permissions
sudo chmod +x /usr/local/bin/rcws_app
sudo chown root:root /usr/local/bin/rcws_app
```

### Systemd Service (Auto-start on boot)

Create `/etc/systemd/system/rcws.service`:
```ini
[Unit]
Description=RCWS Control System
After=network.target

[Service]
Type=simple
User=rcws
Group=rcws
ExecStart=/usr/local/bin/rcws_app
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
```

Enable service:
```bash
sudo systemctl daemon-reload
sudo systemctl enable rcws.service
sudo systemctl start rcws.service
```

---

## ⚙️ Configuration

### Main Configuration File: `/etc/rcws/config.yaml`
```yaml
system:
  log_level: info
  log_file: /var/log/rcws/system.log
  
hardware:
  cameras:
    day:
      device: /dev/video0
      width: 1280
      height: 720
      format: YUY2
      framerate: 30
    night:
      device: /dev/video1
      width: 1280
      height: 720
      format: YUY2
      framerate: 30
  
  servos:
    azimuth:
      port: /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if04
      baudrate: 230400
      slave_id: 2
      limits:
        min: -180.0
        max: 180.0
    elevation:
      port: /dev/serial/by-id/usb-WCH.CN_USB_Quad_Serial_BC046FABCD-if06
      baudrate: 230400
      slave_id: 1
      limits:
        min: -20.0
        max: 60.0
  
  imu:
    port: /dev/ttyUSB2
    baudrate: 115200
    slave_id: 1
  
  lrf:
    port: /dev/ttyUSB1
    baudrate: 115200
  
tracking:
  enabled: true
  backend: VPI_BACKEND_CUDA
  confidence_threshold: 0.25
  
detection:
  enabled: false
  model_path: /usr/local/share/rcws/models/yolov8s.onnx
  confidence_threshold: 0.5
  
ballistics:
  zeroing:
    max_azimuth_offset: 10.0  # degrees
    max_elevation_offset: 10.0  # degrees
  windage:
    max_wind_speed: 50.0  # knots
```

---

## 🎮 Usage

### Starting the Application

**Desktop Mode** (with window manager):
```bash
rcws_app
```

**Fullscreen Mode** (kiosk mode):
```bash
rcws_app --fullscreen
```

**Debug Mode** (verbose logging):
```bash
rcws_app --log-level=debug
```

### Joystick Controls

| Control | Function |
|---------|----------|
| **Left Stick** | Gimbal slew (azimuth/elevation) |
| **Right Stick** | Camera zoom (up/down) |
| **Trigger** | Fire weapon (when armed) |
| **Button 1** | Camera switch (Day/Night) |
| **Button 2** | Track select (initiate tracking) |
| **Button 3** | Laser range finder |
| **D-Pad Up/Down** | Menu navigation |
| **D-Pad Left/Right** | Reticle type selection |
| **Start Button** | Main menu |
| **Select Button** | System status |

### GUI Menus

**Main Menu** (accessible via joystick Start button):
- System Status
- Zone Definition
- Ballistics Settings
- Reticle Selection
- Color Theme
- About
- Shutdown

### Tracking Workflow

1. **Position gimbal** over area of interest
2. **Press Track Select** button (or menu)
3. **Adjust acquisition box** using joystick (if needed)
4. **Press Track Select again** to lock-on
5. **System tracks target** automatically
6. **Press Track Select** to release tracking

### Zeroing Procedure

1. Open **Ballistics Settings** → **Zeroing**
2. Aim at a known target (e.g., 100m)
3. Fire a test shot
4. **Adjust azimuth/elevation offsets** to compensate for impact point
5. Repeat until point of aim = point of impact
6. **Save zeroing** settings

---

## 🐛 Troubleshooting

### Common Issues

#### 1. No Video Display

**Symptoms**: Black screen, "Waiting for video signal..." message

**Possible Causes**:
- Camera not connected
- Wrong device path
- GStreamer pipeline error

**Solutions**:
```bash
# Check camera devices
ls -l /dev/video*

# Test GStreamer manually
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink

# Check permissions
sudo usermod -aG video $USER
# Log out and back in
```

#### 2. Tracking Not Working

**Symptoms**: "Target lost" immediately after lock-on

**Possible Causes**:
- VPI not installed
- CUDA not available
- Insufficient contrast in target area

**Solutions**:
```bash
# Verify VPI installation
ldconfig -p | grep vpi

# Check CUDA
nvidia-smi

# Check system logs
journalctl -u rcws -n 50
```

#### 3. Servo Communication Errors

**Symptoms**: "Servo timeout", gimbal not responding

**Possible Causes**:
- Serial port not accessible
- Wrong baud rate
- Modbus slave ID mismatch

**Solutions**:
```bash
# Check serial ports
ls -l /dev/serial/by-id/

# Test with modbus tool
mbpoll -m rtu -b 230400 -p none -a 2 -t 3 -r 0 -c 1 /dev/ttyUSB0

# Add user to dialout group
sudo usermod -aG dialout $USER
```

#### 4. High CPU/GPU Usage

**Symptoms**: System lag, high temperatures

**Solutions**:
- Disable object detection (set `detection.enabled: false` in config)
- Reduce camera framerate
- Lower video resolution
- Check for background processes

---

## 👨‍💻 Development

### Project Structure
```
rcws-project/
├── src/
│   ├── main.cpp                    # Entry point
│   ├── core/
│   │   └── systemcontroller.cpp   # Hardware lifecycle manager
│   ├── devices/                    # Hardware device classes
│   │   ├── cameravideostreamdevice.cpp
│   │   ├── servodriverdevice.cpp
│   │   ├── imudevice.cpp
│   │   └── ...
│   ├── models/
│   │   ├── domain/
│   │   │   └── systemstatemodel.cpp  # Central data aggregator
│   │   └── viewmodels/
│   │       ├── osdviewmodel.cpp
│   │       ├── zonedefinitionviewmodel.cpp
│   │       └── ...
│   ├── controllers/
│   │   ├── gimbalcontroller.cpp
│   │   ├── cameracontroller.cpp
│   │   ├── osdcontroller.cpp
│   │   └── ...
│   ├── video/
│   │   └── videoimageprovider.cpp  # QML image provider
│   └── ui/ (legacy QWidget code - being phased out)
├── qml/
│   ├── views/
│   │   └── main.qml
│   └── components/
│       ├── OsdOverlay.qml
│       ├── ZoneDefinitionOverlay.qml
│       ├── SystemStatusOverlay.qml
│       └── ...
├── resources/
│   ├── icons/
│   ├── fonts/
│   └── qml.qrc
├── config/
│   └── default.yaml
├── CMakeLists.txt
└── README.md
```

### Adding a New Feature

1. **Define data in SystemStateData** (`systemstatedata.h`)
2. **Update SystemStateModel** to process device data
3. **Create ViewModel** if UI is needed
4. **Create Controller** to manage ViewModel
5. **Create QML UI** component
6. **Register in SystemController** (`systemcontroller.cpp`)
7. **Expose to QML** via context property
8. **Test** thoroughly

### Coding Standards

- **C++17** standard
- **Qt naming conventions**: camelCase for methods, m_prefix for members
- **QML**: Use declarative syntax, avoid imperative JavaScript
- **Comments**: Doxygen-style for public APIs
- **Error handling**: Use qWarning/qCritical for logging
- **Thread safety**: Use mutexes for shared data

### Testing
```bash
# Unit tests (if implemented)
cd build
ctest

# Integration test
./rcws_app --test-mode

# Hardware-in-the-loop test
# (Manual test with actual hardware)
```

---

## 📄 License

**Proprietary Software**

Copyright © 2022-2025 Tunisian Ministry of Defense. All rights reserved.

This software is proprietary and confidential. Unauthorized copying, distribution, modification, or use is strictly prohibited. This software is intended solely for use by authorized personnel of the Tunisian Ministry of Defense.

---

## 📞 Contact

**Lead Developer**: Captain Maher BOUZAIEN  
**Organization**: Tunisian Ministry of Defense  
**Department**: EMAM - CRM

For technical support or inquiries, please contact through official military channels.

---

## 🙏 Acknowledgments

- **NVIDIA** - VPI library for GPU-accelerated computer vision
- **Qt Company** - Qt framework
- **GStreamer Community** - Video processing pipeline
- **OpenCV Contributors** - Computer vision algorithms
- **Ultralytics** - YOLOv8 object detection model

---

*Last Updated: January 2025*  
*Document Version: 1.0*

✅ Integration Checklist
SystemController Updates
Add to systemcontroller.h:
cpp// Forward declares
class SystemStatusViewModel;
class AboutViewModel;
class SystemStatusController;

// Private members
SystemStatusViewModel* m_systemStatusViewModel = nullptr;
AboutViewModel* m_aboutViewModel = nullptr;
SystemStatusController* m_systemStatusController = nullptr;
Add to systemcontroller.cpp in initializeQmlSystem():
cpp// Create ViewModels
m_systemStatusViewModel = new SystemStatusViewModel(this);
m_aboutViewModel = new AboutViewModel(this);

// Create Controllers
m_systemStatusController = new SystemStatusController(this);
m_systemStatusController->setViewModel(m_systemStatusViewModel);
m_systemStatusController->setStateModel(m_systemStateModel);
m_systemStatusController->initialize();

// Expose to QML
rootContext->setContextProperty("systemStatusViewModel", m_systemStatusViewModel);
rootContext->setContextProperty("aboutViewModel", m_aboutViewModel);
Add to main.qml
qml// Add after ZoneDefinitionOverlay
SystemStatusOverlay {
    id: systemStatusOverlay
    anchors.fill: parent
    z: 100
}

AboutDialog {
    id: aboutDialog
    anchors.fill: parent
    z: 100
}

