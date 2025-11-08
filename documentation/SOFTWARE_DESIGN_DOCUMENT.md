# El 7aress RCWS - Software Design Document (SDD)

**Document Version:** 1.0
**Date:** January 2025
**Classification:** Proprietary - Tunisian Ministry of Defense
**Author:** EMAM - CRM Development Team

---

## Document Information

| Item | Description |
|------|-------------|
| **Project** | El 7aress Remote Controlled Weapon Station |
| **Software Version** | 4.5 |
| **Platform** | NVIDIA Jetson Orin AGX / Ubuntu 22.04 |
| **Framework** | Qt 6.x with QML |
| **Purpose** | Technical reference for software architecture, design patterns, and implementation details |
| **Audience** | Software developers, system architects, maintenance engineers |

---

## Table of Contents

### PART 1: ARCHITECTURE OVERVIEW
1. [Executive Summary](#1-executive-summary)
2. [System Architecture Overview](#2-system-architecture-overview)
3. [Technology Stack](#3-technology-stack)

### PART 2: CORE ARCHITECTURAL PATTERNS
4. [Threading Model & Concurrency](#4-threading-model--concurrency)
5. [Three-Phase Initialization Sequence](#5-three-phase-initialization-sequence)
6. [Manager Pattern Architecture](#6-manager-pattern-architecture)
7. [Hardware Abstraction Layer (MIL-STD)](#7-hardware-abstraction-layer-mil-std)
8. [MVVM Pattern & Data Flow](#8-mvvm-pattern--data-flow)

### PART 3: SUBSYSTEMS
9. [Motion Control System](#9-motion-control-system)
10. [Services Layer Architecture](#10-services-layer-architecture)
11. [Video Processing Pipeline](#11-video-processing-pipeline)
12. [QML UI Architecture](#12-qml-ui-architecture)
13. [Safety & Zone Management](#13-safety--zone-management)

### PART 4: IMPLEMENTATION DETAILS
14. [Error Handling & Logging Strategy](#14-error-handling--logging-strategy)
15. [Performance & Memory Analysis](#15-performance--memory-analysis)
16. [Configuration Management](#16-configuration-management)

### PART 5: DEVELOPER GUIDES
17. [Adding New Features](#17-adding-new-features)
18. [Testing Strategy](#18-testing-strategy)
19. [Code Style & Standards](#19-code-style--standards)

### APPENDICES
- [Appendix A: Class Reference](#appendix-a-class-reference)
- [Appendix B: Signal/Slot Connections](#appendix-b-signalslot-connections)
- [Appendix C: File Structure](#appendix-c-file-structure)

---

# PART 1: ARCHITECTURE OVERVIEW

## 1. Executive Summary

### 1.1 Project Overview

El 7aress RCWS is a **military-grade remote controlled weapon station** designed for the Tunisian Ministry of Defense. The system provides advanced targeting, tracking, and fire control capabilities through a sophisticated software architecture built on Qt6/QML.

**Key Differentiators:**
- ✅ MIL-STD compliant three-layer device architecture
- ✅ Manager pattern for modular subsystem organization
- ✅ Advanced MVVM pattern with unidirectional data flow
- ✅ Real-time performance on embedded Jetson platform
- ✅ Comprehensive telemetry API with JWT authentication
- ✅ GPU-accelerated computer vision (VPI + YOLO)
- ✅ Thread-safe architecture suitable for mission-critical systems

### 1.2 Key Capabilities

| Capability | Description |
|------------|-------------|
| **Dual Camera System** | Day (Sony FCB-EV7520A) and Thermal (FLIR Boson 640) with automatic switching |
| **Advanced Tracking** | VPI-based DCF (Discriminative Correlation Filter) object tracking at 30 FPS |
| **Object Detection** | YOLOv8 integration for automatic target detection (optional) |
| **Precision Gimbal Control** | High-precision azimuth/elevation servo control with PID loops |
| **Ballistics Compensation** | Zeroing, windage, and lead angle compensation |
| **Zone Management** | No-fire zones and no-traverse zones for safety |
| **Multi-Reticle System** | 5 reticle types (Basic, Box Crosshair, Standard, Precision, Mil-Dot) |
| **Telemetry API** | REST + WebSocket APIs with JWT authentication for remote monitoring |
| **Data Logging** | Time-series data logging with 9 categories, ring buffers, SQLite persistence |

### 1.3 System Constraints

| Constraint | Value | Rationale |
|------------|-------|-----------|
| **Real-time Performance** | OSD updates @ 30 FPS, Gimbal control @ 60 Hz | Operator situational awareness |
| **Tracking Latency** | < 100ms from target motion to gimbal command | Target engagement effectiveness |
| **Memory Footprint** | < 2 GB RAM total | Embedded platform limitations |
| **Thread Safety** | All data access must be thread-safe | Multiple concurrent threads |
| **Safety Critical** | No-fire zone violations prevented at software level | Prevent friendly fire |

---

## 2. System Architecture Overview

### 2.1 Layered Architecture

The RCWS software follows a **strict layered architecture** with unidirectional data flow:

```
┌─────────────────────────────────────────────────────────────┐
│                     PRESENTATION LAYER                      │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              QML Views (main.qml)                    │  │
│  │  - OsdOverlay, ReticleRenderer, TrackingBox         │  │
│  │  - ZoneDefinitionOverlay, WindageOverlay            │  │
│  │  - MainMenu, SystemStatusOverlay                    │  │
│  └────────────────────┬─────────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                         │ Q_PROPERTY bindings
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    VIEWMODEL LAYER                          │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  ViewModels (13+ classes)                            │  │
│  │  - OsdViewModel, ZeroingViewModel, WindageViewModel │  │
│  │  - ZoneDefinitionViewModel, MenuViewModel           │  │
│  │  - SystemStatusViewModel, AboutViewModel            │  │
│  │                                                      │  │
│  │  Role: Expose SystemStateModel data to QML          │  │
│  └────────────────────┬─────────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                         │ Qt signals/slots
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   CONTROLLER LAYER                          │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  QML Controllers (9 classes)                         │  │
│  │  - OsdController, ZeroingController                  │  │
│  │  - WindageController, ZoneDefinitionController       │  │
│  │  - MainMenuController, ApplicationController        │  │
│  │                                                      │  │
│  │  Hardware Controllers (4 classes)                    │  │
│  │  - GimbalController (with 5 motion modes)           │  │
│  │  - WeaponController, CameraController               │  │
│  │  - JoystickController                               │  │
│  └────────────────────┬─────────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                         │ Direct calls
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    MODEL LAYER                              │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │         SystemStateModel (Central Data Hub)          │  │
│  │                                                      │  │
│  │  300+ Q_PROPERTY fields aggregating all device data │  │
│  │  Emits dataChanged() signal on every update         │  │
│  └────────────────────┬─────────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                         │ Data aggregation
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                  HARDWARE LAYER                             │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Device Classes (11 devices)                         │  │
│  │  ┌────────────────┬────────────────┬──────────────┐  │  │
│  │  │  ImuDevice     │ ServoActuator  │ Plc21Device  │  │  │
│  │  │  LrfDevice     │ ServoDriver    │ Plc42Device  │  │  │
│  │  │  RadarDevice   │ DayCamera      │ NightCamera  │  │  │
│  │  │  JoystickDev   │ VideoProcessor │              │  │  │
│  │  └────────────────┴────────────────┴──────────────┘  │  │
│  │                                                      │  │
│  │  Each device follows MIL-STD 3-layer architecture:  │  │
│  │  Device → Protocol Parser → Transport               │  │
│  └────────────────────┬─────────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                         │ Serial/Modbus/GStreamer
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                  PHYSICAL HARDWARE                          │
│  Cameras, Servos, IMU, LRF, PLCs, Joystick, etc.          │
└─────────────────────────────────────────────────────────────┘
```

**Key Architectural Principles:**

1. **Unidirectional Data Flow:** Hardware → Device → Model → ViewModel → QML
2. **Separation of Concerns:** Each layer has a single, well-defined responsibility
3. **Dependency Injection:** Devices receive Transport & Parser as constructor parameters
4. **No Circular Dependencies:** Data flows upward only; control flows downward only
5. **Thread Safety:** All data access through TemplatedDevice<T> with QReadWriteLock

### 2.2 Cross-Cutting Concerns: Services Layer

**Services** handle cross-cutting concerns that don't belong to a specific layer:

```
┌─────────────────────────────────────────────────────────────┐
│                    SERVICES LAYER                           │
│  (Accessed by all layers via dependency injection)          │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  SystemDataLogger                                    │  │
│  │  - Time-series data storage (9 categories)          │  │
│  │  - Ring buffers for bounded memory                  │  │
│  │  - Optional SQLite persistence                      │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Telemetry Services                                  │  │
│  │  - TelemetryApiService (REST HTTP on port 8443)     │  │
│  │  - TelemetryAuthService (JWT authentication)        │  │
│  │  - TelemetryWebSocketServer (real-time on 8444)     │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 2.3 Manager Pattern

To avoid a **monolithic SystemController**, the system uses specialized **Manager classes**:

| Manager | Responsibility |
|---------|----------------|
| **HardwareManager** | Create, configure, and connect all hardware devices, parsers, and transports |
| **ViewModelRegistry** | Create and manage all ViewModels, register with QML context |
| **ControllerRegistry** | Create and manage all Controllers, initialize dependencies |

**Benefits:**
- Single Responsibility Principle compliance
- Easier testing (mock individual managers)
- Clear ownership of subsystems
- Reduced coupling

---

## 3. Technology Stack

### 3.1 Core Frameworks

| Technology | Version | Purpose |
|------------|---------|---------|
| **Qt Framework** | 6.x | Application framework, UI, networking, serialization |
| **QML** | Qt 6.x | Declarative UI language for HMI |
| **C++** | C++17 | Core application logic, hardware control |
| **GStreamer** | 1.0 | Video pipeline (capture, processing, scaling) |
| **NVIDIA VPI** | 2.3 | GPU-accelerated computer vision (DCF tracking) |
| **CUDA** | 11.x | GPU computation backend for VPI |
| **OpenCV** | 4.x | Image format conversion, utilities |
| **YOLOv8** | ONNX | Object detection (optional) |

### 3.2 Qt Modules Used

```cpp
QT += core gui qml quick serialport serialbus network httpserver websockets sql
```

- **QtCore:** Core application framework, threading, signals/slots
- **QtGui:** Graphics primitives, image handling
- **QtQml:** QML engine, JavaScript runtime
- **QtQuick:** QML UI components, rendering
- **QtSerialPort:** Serial communication (cameras, LRF)
- **QtSerialBus:** Modbus RTU protocol (servos, IMU, PLCs)
- **QtNetwork:** TCP/IP, HTTP, WebSocket
- **QtHttpServer:** REST API server
- **QtWebSockets:** Real-time telemetry streaming
- **QtSql:** SQLite database for data persistence

### 3.3 Hardware Communication Protocols

| Protocol | Devices | Library |
|----------|---------|---------|
| **Modbus RTU** | Servos, IMU, PLCs | QtSerialBus (QModbusRtuSerialClient) |
| **Serial RS-232** | Day/Night cameras, LRF | QtSerialPort |
| **VISCA** | Sony day camera control | Custom implementation |
| **Custom Binary** | LRF, FLIR camera | Custom parsers |
| **USB HID** | Joystick | QtGamepad (planned) / Linux evdev |
| **GStreamer V4L2** | Video capture | GStreamer v4l2src |

### 3.4 Build System

```cmake
cmake_minimum_required(VERSION 3.16)
project(rcws VERSION 4.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick SerialPort SerialBus Network HttpServer WebSockets Sql)
find_package(OpenCV REQUIRED)
find_package(GStreamer REQUIRED)
find_package(VPI REQUIRED)
```

---

# PART 2: CORE ARCHITECTURAL PATTERNS

## 4. Threading Model & Concurrency

### 4.1 Thread Topology

The RCWS system uses a **multi-threaded architecture** to achieve real-time performance while maintaining responsiveness:

```
┌─────────────────────────────────────────────────────────────┐
│                     MAIN THREAD                             │
│  - Qt Event Loop (QGuiApplication::exec())                  │
│  - QML rendering (30 FPS target)                            │
│  - UI event handling                                        │
│  - Timer-based updates (OSD, viewmodels)                    │
│  - Non-blocking device communication                        │
└───────────────────────┬─────────────────────────────────────┘
                       │
                       ├──────────────────────────────────────┐
                       │                                      │
                       ▼                                      ▼
┌───────────────────────────────────┐  ┌──────────────────────────────────┐
│    AZIMUTH SERVO THREAD           │  │  ELEVATION SERVO THREAD          │
│                                   │  │                                  │
│  - Dedicated QThread              │  │  - Dedicated QThread             │
│  - 60 Hz servo command loop       │  │  - 60 Hz servo command loop      │
│  - Modbus RTU read/write          │  │  - Modbus RTU read/write         │
│  - Position/speed updates         │  │  - Position/speed updates        │
│  - PID control calculations       │  │  - PID control calculations      │
│  - Thread-safe data via mutex     │  │  - Thread-safe data via mutex    │
└───────────────────────────────────┘  └──────────────────────────────────┘

                       ▼                                      ▼
┌───────────────────────────────────┐  ┌──────────────────────────────────┐
│    DAY VIDEO PROCESSOR THREAD     │  │  NIGHT VIDEO PROCESSOR THREAD    │
│                                   │  │                                  │
│  - GStreamer pipeline processing  │  │  - GStreamer pipeline processing │
│  - VPI tracking (GPU-accelerated) │  │  - VPI tracking (GPU-accelerated)│
│  - YOLOv8 detection (optional)    │  │  - Format conversion             │
│  - Frame @ 30 FPS                 │  │  - Frame @ 30 FPS                │
│  - Emit frameDataReady signal     │  │  - Emit frameDataReady signal    │
└───────────────────────────────────┘  └──────────────────────────────────┘

                       ▼
┌─────────────────────────────────────────────────────────────┐
│           DATABASE WRITER THREAD                            │
│  - Background QTimer (every 60 seconds)                     │
│  - Write ring buffer data to SQLite                         │
│  - Incremental writes (only new data)                       │
│  - Non-blocking (doesn't affect real-time performance)      │
└─────────────────────────────────────────────────────────────┘
```

**Thread Count:**
- 1 main thread (Qt event loop + QML)
- 2 servo threads (azimuth + elevation)
- 2 video processing threads (day + night camera)
- 1 database writer thread (optional, background)
- **Total: 6 threads** (5 if database disabled)

### 4.2 Thread-Safe Data Access Pattern

All device data is accessed through the **TemplatedDevice<TData>** base class, which provides thread-safe read/write operations:

```cpp
/**
 * @class TemplatedDevice
 * @brief Base class for thread-safe device data management
 *
 * Provides RAII-based locking for device data access across threads.
 * Used by all hardware devices.
 */
template<typename TData>
class TemplatedDevice : public IDevice {
public:
    /**
     * @brief Thread-safe read access to device data
     * @return Copy of current device data
     */
    TData data() const {
        QReadLocker locker(&m_dataLock);
        return m_data;
    }

    /**
     * @brief Thread-safe write access to device data
     * @param newData Data to write
     */
    void setData(const TData& newData) {
        QWriteLocker locker(&m_dataLock);
        m_data = newData;
    }

protected:
    TData m_data;                      // Device-specific data structure
    mutable QReadWriteLock m_dataLock; // Reader-writer lock for concurrent access
};
```

**Example: IMU Device (100 Hz updates)**

```cpp
class ImuDevice : public TemplatedDevice<ImuDataModel> {
    Q_OBJECT
public:
    // Thread-safe getter (can be called from any thread)
    ImuDataModel getCurrentData() const {
        return data();  // Internally acquires read lock
    }

protected slots:
    // Called from Modbus thread when new data arrives
    void onModbusDataReceived(const QModbusDataUnit& unit) {
        ImuDataModel newData;
        newData.roll = parseRoll(unit);
        newData.pitch = parsePitch(unit);
        newData.yaw = parseYaw(unit);

        setData(newData);  // Internally acquires write lock
        emit dataUpdated(newData);
    }
};
```

**Benefits:**
- ✅ **No data races:** QReadWriteLock prevents concurrent read/write
- ✅ **Multiple readers:** Many threads can read simultaneously
- ✅ **Exclusive writer:** Only one writer at a time
- ✅ **RAII locking:** Locks automatically released on scope exit
- ✅ **Deadlock prevention:** Consistent locking order enforced

### 4.3 Inter-Thread Communication

**Mechanism: Qt Signals & Slots (Queued Connections)**

```cpp
// Device thread emits signal
emit dataUpdated(newData);  // Thread A

// Main thread receives via queued connection (thread-safe)
connect(device, &ImuDevice::dataUpdated,
        model, &SystemStateModel::onImuDataReceived,
        Qt::QueuedConnection);  // Cross-thread signal
```

**Qt automatically:**
1. Marshals data from source thread to destination thread
2. Posts event to destination thread's event queue
3. Invokes slot when destination thread processes event queue
4. Provides thread-safe parameter passing

### 4.4 Deadlock Prevention Strategy

**Rules enforced throughout codebase:**

1. **Consistent Lock Ordering:** Always acquire locks in same order
   - Example: Always lock IMU before Gimbal, never reverse

2. **Hold Locks Briefly:** Minimize lock scope
   ```cpp
   // ❌ BAD: Lock held during slow operation
   QWriteLocker locker(&m_dataLock);
   m_data = newData;
   performSlowCalculation();  // Lock still held!

   // ✅ GOOD: Release lock quickly
   {
       QWriteLocker locker(&m_dataLock);
       m_data = newData;
   }  // Lock released
   performSlowCalculation();  // No lock held
   ```

3. **No Recursive Locking:** Avoid calling methods that acquire same lock
4. **Use Signals for Cross-Thread:** Never directly call methods across threads

### 4.5 Real-Time Performance Considerations

**Critical Thread Priorities:**

```cpp
// Servo threads (highest priority - real-time control)
m_azimuthThread->setPriority(QThread::TimeCriticalPriority);
m_elevationThread->setPriority(QThread::TimeCriticalPriority);

// Video threads (high priority - tracking latency)
m_dayVideoThread->setPriority(QThread::HighPriority);
m_nightVideoThread->setPriority(QThread::HighPriority);

// Database thread (low priority - background work)
m_databaseThread->setPriority(QThread::LowPriority);
```

**Update Frequencies (Target vs Actual):**

| Subsystem | Target Frequency | Actual Measured | Thread |
|-----------|-----------------|-----------------|--------|
| Servo Position Update | 60 Hz | 58-60 Hz | Dedicated servo threads |
| IMU Data Update | 100 Hz | 95-100 Hz | Main thread (Modbus) |
| Video Frame Processing | 30 FPS | 28-30 FPS | Video processor threads |
| VPI Tracking Update | 30 Hz | 25-30 Hz | Video processor threads |
| OSD Rendering | 30 FPS | 28-30 FPS | Main thread (QML) |
| Joystick Polling | 100 Hz | 90-100 Hz | Main thread |

---


## 5. Three-Phase Initialization Sequence

### 5.1 Why Three Phases?

The RCWS initialization follows a **strict three-phase sequence** to ensure proper dependency ordering:

```
PHASE 1: Hardware    →  Create devices, transports, parsers, models
PHASE 2: QML System  →  Create ViewModels, controllers, register with QML
PHASE 3: Start       →  Open connections, start threads, initialize devices
```

**Critical Requirement:** QML views cannot be loaded until ViewModels are registered. ViewModels cannot be created until SystemStateModel exists. SystemStateModel cannot function until devices are created.

**Violating this order causes:**
- ❌ QML binding errors (ViewModel not found)
- ❌ Null pointer dereferenc

es (accessing uninitialized objects)
- ❌ Signal/slot connection failures
- ❌ Application crashes

### 5.2 Phase 1: Hardware Initialization

**File:** `src/controllers/systemcontroller.cpp::initializeHardware()`

**Executed from:** `main.cpp` (line 23)

```cpp
void SystemController::initializeHardware()
{
    qInfo() << "=== PHASE 1: Hardware Initialization ===";

    // 1. Create SystemStateModel (central data hub)
    m_systemStateModel = new SystemStateModel(this);
    
    // 2. Create Data Logger
    createDataLogger();
    
    // 3. Create managers
    createManagers();
    
    // 4. Create hardware using HardwareManager
    m_hardwareManager->createHardware();
    
    // 5. Connect devices to their data models
    m_hardwareManager->connectDevicesToModels();
    
    // 6. Connect models to SystemState
    m_hardwareManager->connectModelsToSystemState();
    
    // 7. Create hardware controllers
    m_controllerRegistry->createHardwareControllers();
    
    qInfo() << "=== PHASE 1 COMPLETE ===\n";
}
```

**What happens in Phase 1:**

| Step | Action | Purpose |
|------|--------|---------|
| 1 | Create `SystemStateModel` | Central data aggregation point (300+ properties) |
| 2 | Create `SystemDataLogger` | Time-series data storage & ring buffers |
| 3 | Create Managers | `HardwareManager`, `ViewModelRegistry`, `ControllerRegistry` |
| 4 | Create Hardware | 11 devices, 11 parsers, transport layers |
| 5 | Connect Devices to Models | Device signals → DataModel slots |
| 6 | Connect Models to SystemState | DataModel signals → SystemStateModel slots |
| 7 | Create Hardware Controllers | Gimbal, Weapon, Camera, Joystick controllers |

**Dependency Graph:**

```
SystemController
    └── SystemStateModel (required by all)
        └── SystemDataLogger (listens to SystemStateModel.dataChanged)
            └── HardwareManager
                ├── Devices (11x)
                │   ├── Protocol Parsers (11x)
                │   └── Transports (Serial/Modbus)
                └── Data Models (11x)
                    └── Connected to SystemStateModel
            └── ControllerRegistry
                └── Hardware Controllers (4x)
                    └── Use devices & SystemStateModel
```

**No communication happens yet!** Transports are not opened, threads not started.

### 5.3 Phase 2: QML System Initialization

**File:** `src/controllers/systemcontroller.cpp::initializeQmlSystem()`

**Executed from:** `main.cpp` (line 29)

```cpp
void SystemController::initializeQmlSystem(QQmlApplicationEngine* engine)
{
    qInfo() << "=== PHASE 2: QML System Initialization ===";

    // 1. Create Video Provider
    m_videoProvider = new VideoImageProvider();
    engine->addImageProvider("video", m_videoProvider);
    
    // 2. Connect Video Streams to Provider
    connectVideoToProvider();
    
    // 3. Create ViewModels using ViewModelRegistry
    m_viewModelRegistry->createViewModels();
    
    // 4. Create QML Controllers using ControllerRegistry
    m_controllerRegistry->createQmlControllers();
    
    // 5. Initialize controllers (set dependencies)
    m_controllerRegistry->initializeControllers();
    
    // 6. Connect video to OSD for frame-synchronized updates
    m_controllerRegistry->connectVideoToOsd();
    
    // 7. Register ViewModels with QML
    QQmlContext* rootContext = engine->rootContext();
    m_viewModelRegistry->registerWithQml(rootContext);
    
    // 8. Register Controllers with QML
    m_controllerRegistry->registerWithQml(rootContext);
    
    qInfo() << "=== PHASE 2 COMPLETE ===\n";
}
```

**What happens in Phase 2:**

| Step | Action | Result |
|------|--------|--------|
| 1 | Create VideoImageProvider | Image provider for `Image { source: "image://video/frame" }` |
| 2 | Connect video streams | Day/night camera frames → VideoImageProvider |
| 3 | Create ViewModels (13+) | OSD, Zeroing, Windage, Zone, Menu, Status, etc. |
| 4 | Create QML Controllers (9) | OSD, Zeroing, Windage, Zone, Menu, Application, etc. |
| 5 | Initialize controllers | Inject ViewModel & SystemStateModel dependencies |
| 6 | Connect video to OSD | Frame arrival triggers OSD update (sync rendering) |
| 7-8 | Register with QML context | Expose as `osdViewModel`, `zeroingController`, etc. |

**Now QML can load!** All properties and methods are accessible.

### 5.4 Phase 3: System Startup

**File:** `src/controllers/systemcontroller.cpp::startSystem()`

**Executed from:** `main.cpp` (line 44)

```cpp
void SystemController::startSystem()
{
    qInfo() << "=== PHASE 3: System Startup ===";

    // 1. Start OSD startup sequence (professional boot messages)
    m_controllerRegistry->osdController()->startStartupSequence();
    
    // 2. Start hardware (open transports, initialize devices)
    m_hardwareManager->startHardware();
    
    // 3. Clear gimbal alarms (via gimbal controller)
    m_controllerRegistry->gimbalController()->clearAlarms();
    
    // 4. Create legacy API server (port 8080)
    createApiServer();
    
    // 5. Create and start telemetry services
    createTelemetryServices();
    
    qInfo() << "=== PHASE 3 COMPLETE - SYSTEM RUNNING ===\n";
}
```

**What happens in Phase 3:**

| Step | Action | Result |
|------|--------|--------|
| 1 | Start OSD sequence | Display "SYSTEM INITIALIZING...", "CAMERAS: OK", etc. |
| 2 | Start hardware | Open serial ports, Modbus connections, start threads |
| 3 | Clear gimbal alarms | Send Modbus commands to reset servo alarms |
| 4 | Start legacy API | HTTP server on port 8080 (deprecated) |
| 5 | Start telemetry services | REST API (8443), WebSocket (8444), auth service |

**System is now operational!** Devices communicate, data flows, UI updates.

### 5.5 Complete Initialization Timeline

```
T=0ms    main() starts
T=10ms   DeviceConfiguration::load("devices.json")
T=20ms   ┌─────────────────────────────────────┐
         │  PHASE 1: initializeHardware()      │
T=50ms   │  - Create SystemStateModel          │
T=80ms   │  - Create DataLogger                │
T=100ms  │  - Create Managers                  │
T=150ms  │  - Create 11 devices                │
T=200ms  │  - Connect signals/slots            │
T=250ms  └─────────────────────────────────────┘
         
T=260ms  ┌─────────────────────────────────────┐
         │  PHASE 2: initializeQmlSystem()     │
T=280ms  │  - Create VideoImageProvider        │
T=300ms  │  - Create 13 ViewModels             │
T=350ms  │  - Create 9 QML Controllers         │
T=400ms  │  - Initialize dependencies          │
T=450ms  │  - Register with QML context        │
T=460ms  └─────────────────────────────────────┘
         
T=470ms  engine.load("qrc:/qml/views/main.qml")
T=500ms  QML parsing & component creation
T=550ms  QML rendering begins (black screen)
         
T=560ms  ┌─────────────────────────────────────┐
         │  PHASE 3: startSystem()             │
T=570ms  │  - Start OSD startup sequence       │
T=600ms  │  - Open serial ports                │
T=650ms  │  - Open Modbus connections          │
T=700ms  │  - Start servo threads              │
T=750ms  │  - Start video threads              │
T=800ms  │  - Clear gimbal alarms              │
T=850ms  │  - Start API servers                │
T=900ms  └─────────────────────────────────────┘

T=1000ms OSD displays "CAMERAS: CONNECTING..."
T=1500ms First video frame displayed
T=2000ms OSD displays "SYSTEM READY"
T=2000ms Operator can begin using system
```

**Total startup time:** ~2 seconds from launch to operational

### 5.6 Error Handling During Initialization

**Philosophy:** **Fail fast, fail loud**

```cpp
// Example: Hardware creation failure
if (!m_hardwareManager->createHardware()) {
    qCritical() << "Failed to create hardware!";
    return;  // Stop initialization, don't proceed to Phase 2
}
```

**Graceful degradation examples:**

| Failure | System Behavior |
|---------|-----------------|
| **IMU not connected** | Warning logged, system continues (no stabilization) |
| **Day camera failed** | Night camera used, warning displayed on OSD |
| **Servo communication timeout** | Emergency stop, gimbal locked, critical alarm |
| **Device config file missing** | Application exits with error code -1 |
| **QML load failure** | Application exits with error code -1 |

---

## 6. Manager Pattern Architecture

### 6.1 Why Managers?

**Problem:** Original `SystemController` was a **monolithic god object**:
- Created 50+ objects directly
- Managed 100+ connections
- 2000+ lines of code
- Difficult to test
- High coupling

**Solution:** **Extract subsystem management into specialized managers**

**Benefits:**
- ✅ **Single Responsibility Principle:** Each manager handles one concern
- ✅ **Testability:** Mock managers independently
- ✅ **Maintainability:** Clear ownership boundaries
- ✅ **Reduced Coupling:** SystemController delegates to managers

### 6.2 HardwareManager

**File:** `src/managers/HardwareManager.h/cpp`

**Responsibility:** Create, configure, connect, and start all hardware devices

**Architecture:**

```cpp
class HardwareManager : public QObject {
    Q_OBJECT
public:
    explicit HardwareManager(SystemStateModel* systemState, QObject* parent);

    // Four-phase hardware initialization
    bool createHardware();                    // Phase 1a: Instantiate devices
    bool connectDevicesToModels();            // Phase 1b: Connect device→model
    bool connectModelsToSystemState();        // Phase 1c: Connect model→systemstate
    bool startHardware();                     // Phase 3: Open connections

    // Device accessors (for controllers and video processing)
    ImuDevice* imuDevice() const;
    ServoActuatorDevice* azimuthServo() const;
    ServoActuatorDevice* elevationServo() const;
    CameraVideoStreamDevice* dayVideoProcessor() const;
    CameraVideoStreamDevice* nightVideoProcessor() const;
    // ... (11 devices total)

private:
    // Device instances (ownership)
    ImuDevice* m_imuDevice = nullptr;
    ServoActuatorDevice* m_azimuthServo = nullptr;
    ServoActuatorDevice* m_elevationServo = nullptr;
    Plc21Device* m_plc21Device = nullptr;
    Plc42Device* m_plc42Device = nullptr;
    LrfDevice* m_lrfDevice = nullptr;
    RadarDevice* m_radarDevice = nullptr;
    DayCameraControlDevice* m_dayCameraControl = nullptr;
    NightCameraControlDevice* m_nightCameraControl = nullptr;
    CameraVideoStreamDevice* m_dayVideoProcessor = nullptr;
    CameraVideoStreamDevice* m_nightVideoProcessor = nullptr;

    // Data models
    ImuDataModel* m_imuModel = nullptr;
    ServoActuatorDataModel* m_azModel = nullptr;
    ServoActuatorDataModel* m_elModel = nullptr;
    // ... (11 models)

    SystemStateModel* m_systemStateModel = nullptr;
};
```

**Example: Creating IMU Device**

```cpp
bool HardwareManager::createHardware()
{
    qInfo() << "HardwareManager: Creating hardware...";

    // Get device config from global configuration
    const auto& config = DeviceConfiguration::imu();

    // 1. Create Transport Layer
    auto* serialTransport = new SerialPortTransport(config.port, config.baudRate);

    // 2. Create Protocol Parser
    auto* imuParser = new ImuProtocolParser();

    // 3. Create Device (inject dependencies)
    m_imuDevice = new ImuDevice(serialTransport, imuParser, this);

    // 4. Create Data Model
    m_imuModel = new ImuDataModel(this);

    qInfo() << "  ✓ IMU device created";
    return true;
}
```

**Connection Flow:**

```
1. connectDevicesToModels():
   Device::dataReceived signal → DataModel::onDataReceived slot

2. connectModelsToSystemState():
   DataModel::dataChanged signal → SystemStateModel::on<Device>DataChanged slot

Result:
   IMU hardware → ImuDevice → ImuDataModel → SystemStateModel → ViewModels → QML
```

### 6.3 ViewModelRegistry

**File:** `src/managers/ViewModelRegistry.h/cpp`

**Responsibility:** Create and manage all ViewModels, register with QML context

```cpp
class ViewModelRegistry : public QObject {
    Q_OBJECT
public:
    explicit ViewModelRegistry(QObject* parent);

    bool createViewModels();
    bool registerWithQml(QQmlContext* context);

    // ViewModel accessors
    OsdViewModel* osdViewModel() const;
    ZeroingViewModel* zeroingViewModel() const;
    WindageViewModel* windageViewModel() const;
    ZoneDefinitionViewModel* zoneViewModel() const;
    MenuViewModel* menuViewModel() const;
    SystemStatusViewModel* systemStatusViewModel() const;
    AboutViewModel* aboutViewModel() const;
    // ... (13+ ViewModels)

private:
    OsdViewModel* m_osdViewModel = nullptr;
    ZeroingViewModel* m_zeroingViewModel = nullptr;
    WindageViewModel* m_windageViewModel = nullptr;
    ZoneDefinitionViewModel* m_zoneViewModel = nullptr;
    ZoneMapViewModel* m_zoneMapViewModel = nullptr;
    AreaZoneParameterViewModel* m_areaZoneParamViewModel = nullptr;
    SectorScanParameterViewModel* m_sectorScanParamViewModel = nullptr;
    TrpParameterViewModel* m_trpParamViewModel = nullptr;
    MenuViewModel* m_menuViewModel = nullptr;
    SystemStatusViewModel* m_systemStatusViewModel = nullptr;
    AboutViewModel* m_aboutViewModel = nullptr;
};
```

**Registration with QML:**

```cpp
bool ViewModelRegistry::registerWithQml(QQmlContext* context)
{
    qInfo() << "ViewModelRegistry: Registering ViewModels with QML...";

    context->setContextProperty("osdViewModel", m_osdViewModel);
    context->setContextProperty("zeroingViewModel", m_zeroingViewModel);
    context->setContextProperty("windageViewModel", m_windageViewModel);
    context->setContextProperty("zoneDefinitionViewModel", m_zoneViewModel);
    context->setContextProperty("menuViewModel", m_menuViewModel);
    // ... (all 13+ ViewModels)

    qInfo() << "  ✓ All ViewModels registered";
    return true;
}
```

**QML can now access:**

```qml
// In OsdOverlay.qml
Text {
    text: osdViewModel.gimbalAzimuth.toFixed(2) + "°"
}

// In ZeroingOverlay.qml
Slider {
    value: zeroingViewModel.azimuthOffset
    onValueChanged: zeroingViewModel.setAzimuthOffset(value)
}
```

### 6.4 ControllerRegistry

**File:** `src/managers/ControllerRegistry.h/cpp`

**Responsibility:** Create, initialize, and manage all controllers (hardware + QML)

```cpp
class ControllerRegistry : public QObject {
    Q_OBJECT
public:
    explicit ControllerRegistry(
        HardwareManager* hardwareManager,
        ViewModelRegistry* viewModelRegistry,
        SystemStateModel* systemStateModel,
        QObject* parent
    );

    // Hardware controllers (Phase 1)
    bool createHardwareControllers();

    // QML controllers (Phase 2)
    bool createQmlControllers();
    bool initializeControllers();
    bool connectVideoToOsd();
    bool registerWithQml(QQmlContext* context);

    // Controller accessors
    GimbalController* gimbalController() const;
    OsdController* osdController() const;
    ZeroingController* zeroingController() const;
    // ... (14 controllers total)

private:
    // Dependencies (injected)
    HardwareManager* m_hardwareManager = nullptr;
    ViewModelRegistry* m_viewModelRegistry = nullptr;
    SystemStateModel* m_systemStateModel = nullptr;

    // Hardware controllers
    GimbalController* m_gimbalController = nullptr;
    WeaponController* m_weaponController = nullptr;
    CameraController* m_cameraController = nullptr;
    JoystickController* m_joystickController = nullptr;

    // QML controllers
    OsdController* m_osdController = nullptr;
    ZeroingController* m_zeroingController = nullptr;
    WindageController* m_windageController = nullptr;
    ZoneDefinitionController* m_zoneController = nullptr;
    MainMenuController* m_menuController = nullptr;
    ApplicationController* m_appController = nullptr;
    SystemStatusController* m_statusController = nullptr;
    AboutController* m_aboutController = nullptr;
    ColorMenuController* m_colorController = nullptr;
    ReticleMenuController* m_reticleController = nullptr;
};
```

**Example: Creating GimbalController**

```cpp
bool ControllerRegistry::createHardwareControllers()
{
    qInfo() << "ControllerRegistry: Creating hardware controllers...";

    // 1. Create GimbalController (with motion modes)
    m_gimbalController = new GimbalController(this);
    m_gimbalController->setSystemStateModel(m_systemStateModel);
    m_gimbalController->setAzimuthServo(m_hardwareManager->azimuthServo());
    m_gimbalController->setElevationServo(m_hardwareManager->elevationServo());
    m_gimbalController->setImuDevice(m_hardwareManager->imuDevice());
    m_gimbalController->initialize();  // Creates 5 motion modes

    qInfo() << "  ✓ GimbalController created with 5 motion modes";
    return true;
}
```

### 6.5 Manager Collaboration

**Sequence diagram for creating a new feature:**

```
Developer adds new sensor (e.g., GPS)
    ↓
1. Add GPS device to HardwareManager
    HardwareManager::createHardware() {
        m_gpsDevice = new GpsDevice(...);
        m_gpsModel = new GpsDataModel();
    }
    ↓
2. Add GPS fields to SystemStateModel
    class SystemStateData {
        float gpsLatitude;
        float gpsLongitude;
    };
    ↓
3. Create GpsViewModel in ViewModelRegistry
    ViewModelRegistry::createViewModels() {
        m_gpsViewModel = new GpsViewModel(this);
    }
    ↓
4. Create GpsController in ControllerRegistry (if needed)
    ControllerRegistry::createQmlControllers() {
        m_gpsController = new GpsController(this);
    }
    ↓
5. Register with QML
    context->setContextProperty("gpsViewModel", m_gpsViewModel);
    ↓
6. Use in QML
    Text { text: "GPS: " + gpsViewModel.latitude }
```

**Manager Dependencies:**

```
SystemController
    ├── HardwareManager (needs SystemStateModel)
    ├── ViewModelRegistry (no dependencies)
    └── ControllerRegistry (needs HardwareManager + ViewModelRegistry + SystemStateModel)
```

---


## 10. Services Layer Architecture

### 10.1 Service Overview

The RCWS system implements a **Services Layer** for cross-cutting concerns that don't belong to hardware, controllers, or UI. Services are typically stateless or have minimal state and provide reusable functionality across the application.

**Implemented Services:**

| Service | Type | Purpose |
|---------|------|---------|
| **SystemDataLogger** | Data Management | Time-series data storage with ring buffers |
| **TelemetryApiService** | External API | REST HTTP server for telemetry access |
| **TelemetryAuthService** | Security | JWT authentication & user management |
| **TelemetryWebSocketServer** | Real-time Streaming | WebSocket server for live telemetry |
| **TelemetryConfig** | Configuration | Centralized config structures for telemetry |

### 10.2 SystemDataLogger - Time-Series Data Management

**File:** `src/logger/systemdatalogger.h/cpp`

**Purpose:** Efficient time-series data storage and retrieval for post-mission analysis, debugging, and telemetry export

#### 10.2.1 Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│           SystemStateModel (Data Source)                    │
│  Emits dataChanged(SystemStateData) on every update         │
└─────────────────────┬───────────────────────────────────────┘
                      │ Signal (Qt::DirectConnection)
                      ▼
┌─────────────────────────────────────────────────────────────┐
│              SystemDataLogger                               │
│  ┌────────────────────────────────────────────────────┐    │
│  │  onSystemStateChanged(SystemStateData)             │    │
│  │  - Categorizes data into 9 logical groups          │    │
│  │  - Calls 9 extract*() methods                      │    │
│  └───────────┬────────────────────────────────────────┘    │
│              ▼                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Ring Buffers (Thread-Safe Circular Buffers)        │  │
│  │                                                      │  │
│  │  1. DeviceStatusBuffer   (3,600 samples)  @ 1 Hz   │  │
│  │  2. GimbalMotionBuffer   (36,000 samples) @ 60 Hz  │  │
│  │  3. ImuDataBuffer        (60,000 samples) @ 100 Hz │  │
│  │  4. TrackingDataBuffer   (18,000 samples) @ 30 Hz  │  │
│  │  5. WeaponStatusBuffer   (3,600 samples)  @ 1 Hz   │  │
│  │  6. CameraStatusBuffer   (1,800 samples)  @ 1 Hz   │  │
│  │  7. SensorDataBuffer     (6,000 samples)  @ 10 Hz  │  │
│  │  8. BallisticDataBuffer  (1,800 samples)  @ 1 Hz   │  │
│  │  9. UserInputBuffer      (6,000 samples)  @ 10 Hz  │  │
│  │                                                      │  │
│  │  Each buffer: QMutex + QVector<TData>               │  │
│  └───────────┬──────────────────────────────────────────┘  │
│              ▼                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Query Engine (Public API)                           │  │
│  │  - getGimbalMotionHistory(startTime, endTime)        │  │
│  │  - getImuHistory(startTime, endTime)                 │  │
│  │  - exportToCSV(category, filePath, start, end)       │  │
│  │  - getMemoryUsage(), getSampleCount()                │  │
│  └───────────┬──────────────────────────────────────────┘  │
│              ▼                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Optional SQLite Persistence (Background Thread)     │  │
│  │  - QTimer triggers every 60 seconds                  │  │
│  │  - Incremental writes (only new data since last)     │  │
│  │  - Database path: ./rcws_history.db                  │  │
│  │  - Long-term storage (months/years)                  │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

#### 10.2.2 Data Categories

The logger organizes data into **9 logical categories** based on update frequency and usage:

| # | Category | Update Freq | Buffer Size | Data Retention | Memory Usage |
|---|----------|-------------|-------------|----------------|--------------|
| 1 | **DeviceStatus** | 1 Hz | 3,600 samples | 1 hour | ~360 KB |
| 2 | **GimbalMotion** | 60 Hz | 36,000 samples | 10 minutes | ~1.7 MB |
| 3 | **ImuData** | 100 Hz | 60,000 samples | 10 minutes | ~3.4 MB |
| 4 | **TrackingData** | 30 Hz | 18,000 samples | 10 minutes | ~1.4 MB |
| 5 | **WeaponStatus** | 1 Hz | 3,600 samples | 1 hour | ~180 KB |
| 6 | **CameraStatus** | 1 Hz | 1,800 samples | 30 minutes | ~72 KB |
| 7 | **SensorData** | 10 Hz | 6,000 samples | 10 minutes | ~240 KB |
| 8 | **BallisticData** | 1 Hz | 1,800 samples | 30 minutes | ~108 KB |
| 9 | **UserInput** | 10 Hz | 6,000 samples | 10 minutes | ~288 KB |
| | **TOTAL** | | **~138,000** | | **~8 MB** |

**Why separate categories?**
- ✅ Different update frequencies (1 Hz - 100 Hz)
- ✅ Different retention requirements
- ✅ Efficient memory usage (only store what's needed)
- ✅ Targeted queries (e.g., "gimbal motion during last engagement")

#### 10.2.3 Ring Buffer Implementation

**Thread-safe circular buffer with automatic overflow handling:**

```cpp
template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(int maxSize) : m_maxSize(maxSize) {
        m_data.reserve(maxSize);  // Pre-allocate for performance
    }

    // Thread-safe append (called from main thread)
    void append(const T& item) {
        QMutexLocker locker(&m_mutex);
        if (m_data.size() >= m_maxSize) {
            m_data.removeFirst();  // Discard oldest sample
        }
        m_data.append(item);
    }

    // Thread-safe time-range query (called from API threads)
    QVector<T> getRange(qint64 startMs, qint64 endMs) const {
        QMutexLocker locker(&m_mutex);
        QVector<T> result;
        for (const auto& item : m_data) {
            if (item.timestampMs >= startMs && item.timestampMs <= endMs) {
                result.append(item);
            }
        }
        return result;  // Returns copy (safe for cross-thread)
    }

private:
    QVector<T> m_data;
    int m_maxSize;
    mutable QMutex m_mutex;  // Thread-safe access
};
```

**Benefits:**
- ✅ **Bounded memory:** Never exceeds configured buffer size
- ✅ **No allocation during operation:** Pre-reserved capacity
- ✅ **Thread-safe:** QMutex protects concurrent access
- ✅ **Automatic cleanup:** Oldest data discarded when full

#### 10.2.4 Data Flow Example: Gimbal Motion

```
T=0ms    Servo sends position update via Modbus
         ↓
T=1ms    ServoActuatorDevice::onModbusDataReceived()
         ↓
T=2ms    ServoDataModel::onDeviceDataChanged()
         ↓
T=3ms    SystemStateModel::onServoDataChanged()
         SystemStateModel.gimbalAz = 45.3°
         SystemStateModel.gimbalEl = -12.5°
         emit dataChanged(m_data)
         ↓
T=4ms    SystemDataLogger::onSystemStateChanged(state)
         ↓
T=5ms    GimbalMotionData gimbal = extractGimbalMotion(state)
         gimbal.timestamp = QDateTime::currentDateTime()
         gimbal.gimbalAz = 45.3°
         gimbal.gimbalEl = -12.5°
         gimbal.azimuthSpeed = 0.5°/s
         gimbal.opMode = OperationalMode::Attack
         ↓
T=6ms    m_gimbalMotionBuffer.append(gimbal)
         ↓
         [Stored in ring buffer, available for queries]
```

**Frequency:** This happens **60 times per second** for gimbal data!

**Performance:** <0.1ms per sample (measured on Jetson Orin)

#### 10.2.5 SQLite Persistence Strategy

**Optional long-term storage** for mission analysis:

```cpp
// Configuration
SystemDataLogger::LoggerConfig config;
config.enableDatabasePersistence = true;
config.databasePath = "./rcws_history.db";
config.databaseWriteIntervalSec = 60;  // Write every minute
```

**How it works:**

```
Main Thread                          Background Database Thread
-----------                          ---------------------------
Append to ring buffer
    ↓
[60 seconds pass]
    ↓
QTimer timeout
    ↓
onDatabaseWriteTimerTimeout()
    ├── Read new samples from buffers (since last write)
    │   (Thread-safe via QMutex)
    ├── Move to background thread ───────→  Write to SQLite
    │                                        INSERT INTO gimbal_motion ...
    │                                        INSERT INTO imu_data ...
    │                                        [~3600 records/minute]
    │                                        ↓
    ↓                                        Transaction COMMIT
Wait for next timer ←────────────────────── Emit databaseWriteComplete
```

**Database schema example:**

```sql
CREATE TABLE gimbal_motion (
    timestamp INTEGER PRIMARY KEY,  -- Milliseconds since epoch
    azimuth REAL,
    elevation REAL,
    azimuth_speed REAL,
    elevation_speed REAL,
    op_mode INTEGER,
    motion_mode INTEGER
);

CREATE INDEX idx_gimbal_timestamp ON gimbal_motion(timestamp);
```

**Why incremental writes?**
- ✅ Don't re-write existing data
- ✅ Minimal I/O overhead
- ✅ Track last written timestamp per category
- ✅ Only write new samples

**Memory vs Database tradeoff:**

| Storage | Capacity | Speed | Use Case |
|---------|----------|-------|----------|
| **Ring Buffers** | ~10 min to 1 hour | <1ms query | Real-time API, recent history |
| **SQLite Database** | Months/years | ~50ms query | Post-mission analysis, long-term trends |

#### 10.2.6 Query API Examples

**Example 1: Get last 60 seconds of gimbal motion**

```cpp
QDateTime endTime = QDateTime::currentDateTime();
QDateTime startTime = endTime.addSecs(-60);

QVector<GimbalMotionData> history = 
    m_dataLogger->getGimbalMotionHistory(startTime, endTime);

// Result: ~3600 samples (60 Hz × 60 seconds)
for (const auto& point : history) {
    qDebug() << point.timestamp << point.gimbalAz << point.gimbalEl;
}
```

**Example 2: Export IMU data to CSV**

```cpp
QDateTime start = QDateTime::fromString("2025-01-08T10:00:00", Qt::ISODate);
QDateTime end = QDateTime::fromString("2025-01-08T10:10:00", Qt::ISODate);

bool success = m_dataLogger->exportToCSV(
    DataCategory::ImuData,
    "/data/exports/imu_2025-01-08.csv",
    start,
    end
);

// CSV output:
// timestamp,roll,pitch,yaw,gyroX,gyroY,gyroZ,accelX,accelY,accelZ
// 1704711600000,0.1,-0.3,180.2,0.01,0.02,-0.01,0.0,0.0,9.81
// ...
```

**Example 3: Get memory usage statistics**

```cpp
auto stats = m_dataLogger->getMemoryUsage();

qInfo() << "Total memory:" << stats.totalBytes / 1024 / 1024 << "MB";
qInfo() << "Gimbal motion:" << stats.gimbalMotionBytes / 1024 << "KB";
qInfo() << "IMU data:" << stats.imuDataBytes / 1024 << "KB";
```

#### 10.2.7 Integration with Telemetry Services

The SystemDataLogger is the **primary data source** for telemetry APIs:

```cpp
// TelemetryApiService uses DataLogger for historical queries
GET /api/telemetry/history/gimbal?from=...&to=...
    ↓
TelemetryApiService::handleGimbalHistoryRequest()
    ↓
auto history = m_dataLogger->getGimbalMotionHistory(startTime, endTime);
    ↓
Convert to JSON
    ↓
Return HTTP response with gimbal history data
```

**Data flow:**

```
SystemStateModel → SystemDataLogger (ring buffers)
                         ↓
        ┌───────────────┴───────────────┐
        ▼                               ▼
  TelemetryApiService          TelemetryWebSocketServer
  (Historical queries)         (Real-time streaming)
```

### 10.3 Telemetry Services Ecosystem

**Three complementary services working together:**

```
┌──────────────────────────────────────────────────────┐
│         TelemetryAuthService                         │
│  • JWT token generation/validation                   │
│  • User management (Admin/Operator/Viewer)           │
│  • Role-based access control                         │
│  • IP whitelisting (optional)                        │
│  • Audit logging                                     │
└──────────────┬──────────────────┬────────────────────┘
               │                  │
               ▼                  ▼
┌──────────────────────┐   ┌──────────────────────────┐
│ TelemetryApiService  │   │ TelemetryWebSocketServer │
│ (REST HTTP)          │   │ (Real-Time Streaming)    │
│                      │   │                          │
│ Port: 8443          │   │ Port: 8444               │
│ Endpoints:          │   │ Protocol: WebSocket      │
│ • /api/auth/login   │   │ Update Rate: 10 Hz       │
│ • /api/telemetry/*  │   │ Subscriptions: Per-client│
│ • /api/export/csv   │   │ Message Type: JSON       │
│ • /api/users/*      │   │ Auth: JWT tokens         │
│ • /api/health       │   │ Heartbeat: 30s ping/pong │
└──────────┬───────────┘   └───────────┬──────────────┘
           │                           │
           └───────────┬───────────────┘
                       ▼
         ┌──────────────────────────┐
         │  SystemDataLogger        │
         │  (Data Source)           │
         └──────────────────────────┘
                       ▲
                       │
         ┌──────────────────────────┐
         │  SystemStateModel        │
         │  (Real-time Data)        │
         └──────────────────────────┘
```

#### 10.3.1 TelemetryAuthService

**File:** `src/services/telemetryauthservice.h/cpp`

**Purpose:** Centralized authentication and authorization for all telemetry APIs

**Features:**
- ✅ **JWT Token Generation:** Stateless authentication
- ✅ **Role-Based Access Control (RBAC):** Admin/Operator/Viewer roles
- ✅ **User Management:** Add/remove users, change passwords
- ✅ **IP Whitelisting:** Optional IP-based access control
- ✅ **Audit Logging:** Complete access trail for security compliance
- ✅ **Password Hashing:** Secure password storage (not implemented yet, uses plaintext in demo)

**Configuration:**

```cpp
TelemetryAuthService::Config authConfig;
authConfig.jwtSecret = "RCWS_SECURE_SECRET_KEY_CHANGE_IN_PRODUCTION_2025";
authConfig.tokenExpirationMinutes = 60;
authConfig.enableIpWhitelist = false;  // Disabled by default
authConfig.enableAuditLogging = true;
authConfig.auditLogPath = "./logs/telemetry_audit.log";

m_telemetryAuthService = new TelemetryAuthService(authConfig, this);
```

**User Roles:**

| Role | Numeric Value | Permissions |
|------|---------------|-------------|
| **Viewer** | 0 | Read telemetry, read history, read system health |
| **Operator** | 1 | All Viewer permissions + export data, modify settings |
| **Admin** | 2 | All Operator permissions + user management, system config |

**Authentication Flow:**

```
Client                          Server (TelemetryAuthService)
------                          ------------------------------
POST /api/auth/login
  {
    "username": "admin",
    "password": "admin123"
  }
                ───────────────→  1. Validate credentials
                                  2. Check user exists
                                  3. Verify password
                                  4. Generate JWT token
                                     Payload: {
                                       "username": "admin",
                                       "role": 2,
                                       "exp": 1704715200
                                     }
                                  5. Log audit: LOGIN_SUCCESS
                ←───────────────  Response:
                                  {
                                    "token": "eyJhbGc...",
                                    "expiresAt": "2025-01-08T15:30:00Z",
                                    "role": 2
                                  }

Store token for future requests
    ↓
GET /api/telemetry/current
  Authorization: Bearer eyJhbGc...
                ───────────────→  1. Extract token
                                  2. Verify signature
                                  3. Check expiration
                                  4. Extract user/role
                                  5. Check permissions
                                  6. Log audit: ACCESS
                ←───────────────  Response: telemetry data
```

**Audit Log Format:**

```
2025-01-08T14:30:15Z | admin | LOGIN_SUCCESS | 192.168.1.100 | /api/auth/login | SUCCESS | Role: 2
2025-01-08T14:30:20Z | admin | ACCESS | 192.168.1.100 | /api/telemetry/current | SUCCESS |
2025-01-08T14:31:05Z | operator | ACCESS | 192.168.1.101 | /api/telemetry/history/gimbal | SUCCESS |
2025-01-08T14:32:10Z | viewer | ACCESS_DENIED | 192.168.1.105 | /api/users | FORBIDDEN | Insufficient permissions
```

#### 10.3.2 TelemetryApiService

**File:** `src/services/telemetryapiservice.h/cpp`

**Purpose:** REST HTTP API server for telemetry access

**Port:** 8443 (HTTPS), 8080 (HTTP fallback)

**Implemented Endpoints:**

| Method | Endpoint | Auth | Purpose |
|--------|----------|------|---------|
| POST | `/api/auth/login` | No | Get JWT token |
| POST | `/api/auth/refresh` | Yes | Refresh token |
| POST | `/api/auth/logout` | Yes | Revoke token |
| GET | `/api/telemetry/current` | Yes | Get current system state |
| GET | `/api/telemetry/history/gimbal` | Yes | Gimbal motion history |
| GET | `/api/telemetry/history/imu` | Yes | IMU sensor history |
| GET | `/api/telemetry/history/tracking` | Yes | Tracking system history |
| GET | `/api/telemetry/history/weapon` | Yes | Weapon status history |
| GET | `/api/telemetry/history/camera` | Yes | Camera system history |
| GET | `/api/telemetry/history/sensor` | Yes | LRF/radar history |
| GET | `/api/telemetry/history/ballistic` | Yes | Ballistics data history |
| GET | `/api/telemetry/history/device` | Yes | Device health history |
| GET | `/api/telemetry/stats/memory` | Yes | Memory usage by category |
| GET | `/api/telemetry/stats/samples` | Yes | Sample counts |
| GET | `/api/telemetry/stats/timerange` | Yes | Available data time ranges |
| GET | `/api/telemetry/export/csv` | Yes | Export category to CSV |
| GET | `/api/health` | No | Health check |
| GET | `/api/version` | No | API version info |
| GET | `/api/users` | Admin | List all users |
| POST | `/api/users` | Admin | Create new user |
| DELETE | `/api/users/:username` | Admin | Delete user |
| PUT | `/api/users/:username/password` | Admin/Self | Change password |

**Example: Get Current Telemetry**

```bash
# Request
curl -X GET http://localhost:8443/api/telemetry/current \
  -H "Authorization: Bearer eyJhbGc..."

# Response (200 OK)
{
  "timestamp": "2025-01-08T14:30:15Z",
  "gimbalAz": 45.3,
  "gimbalEl": -12.5,
  "gimbalSpeed": 0.5,
  "imuRoll": 0.1,
  "imuPitch": -0.3,
  "imuYaw": 180.2,
  "trackingActive": true,
  "trackingPhase": 3,
  "gunArmed": false,
  "ammoLoaded": true,
  "lrfDistance": 1250.5,
  "activeCameraIsDay": true,
  "dayZoomPosition": 0.75,
  // ... (300+ fields from SystemStateData)
}
```

**Data Downsampling for Large Queries:**

```cpp
// Problem: Client requests 10 hours of IMU data @ 100 Hz
// Result: 3,600,000 samples × 100 bytes = 360 MB JSON!

// Solution: Automatic downsampling
if (sampleCount > MAX_SAMPLES_PER_RESPONSE) {
    int skipFactor = sampleCount / MAX_SAMPLES_PER_RESPONSE;
    // Return every Nth sample
    // Example: 3.6M samples → 10K samples (360:1 ratio)
}
```

**Rate Limiting:**

```cpp
// Per-IP rate limiting (default: 120 requests/minute)
if (requestCount[clientIp] > config.rateLimitPerMinute) {
    return QHttpServerResponse(
        QJsonObject{{"error", "Rate limit exceeded"}},
        QHttpServerResponse::StatusCode::TooManyRequests
    );
}
```

#### 10.3.3 TelemetryWebSocketServer

**File:** `src/services/telemetrywebsocketserver.h/cpp`

**Purpose:** Real-time telemetry streaming via WebSocket protocol

**Port:** 8444 (WSS), 8081 (WS fallback)

**Update Rate:** 10 Hz (configurable up to 100 Hz)

**Connection Flow:**

```
1. Client connects to ws://localhost:8444/telemetry
        ↓
2. Server sends welcome message:
   {
     "type": "welcome",
     "message": "RCWS Telemetry Server",
     "version": "1.0.0",
     "requiresAuth": true
   }
        ↓
3. Client sends authentication:
   {
     "type": "auth",
     "token": "eyJhbGc..."
   }
        ↓
4. Server validates JWT token → sends auth_success:
   {
     "type": "auth_success",
     "username": "admin",
     "role": 2
   }
        ↓
5. Client subscribes to categories:
   {
     "type": "subscribe",
     "categories": ["gimbal", "imu", "tracking"]
   }
        ↓
6. Server confirms subscription:
   {
     "type": "subscribe_success",
     "categories": ["gimbal", "imu", "tracking"]
   }
        ↓
7. Server pushes telemetry updates @ 10 Hz:
   {
     "type": "telemetry",
     "timestamp": "2025-01-08T14:30:15Z",
     "data": {
       "gimbal": { "azimuth": 45.3, "elevation": -12.5, ... },
       "imu": { "roll": 0.1, "pitch": -0.3, "yaw": 180.2, ... },
       "tracking": { "active": true, "phase": 3, ... }
     }
   }
   [Sent every 100ms]
```

**Available Subscription Categories:**

- `all` - All telemetry data (highest bandwidth)
- `gimbal` - Gimbal position, speed, direction, modes
- `imu` - Roll, pitch, yaw, gyro, accelerometer
- `tracking` - Tracking phase, target position, lock status
- `weapon` - Armed state, ready status, ammo, fire mode
- `camera` - Active camera, zoom, field of view
- `sensor` - LRF distance, radar plots
- `ballistic` - Zeroing, windage, lead angle
- `device` - Motor temperatures, driver temps, system health

**Per-Client Subscription Management:**

```cpp
// Server tracks subscriptions per client
QMap<QWebSocket*, QSet<QString>> m_clientSubscriptions;

// Client 1 subscribes to: gimbal, imu
// Client 2 subscribes to: all
// Client 3 subscribes to: weapon, tracking

// On 10 Hz timer:
for (auto* client : m_clients) {
    QJsonObject telemetryData;
    
    // Only include subscribed categories
    if (m_clientSubscriptions[client].contains("gimbal")) {
        telemetryData["gimbal"] = buildGimbalData();
    }
    if (m_clientSubscriptions[client].contains("imu")) {
        telemetryData["imu"] = buildImuData();
    }
    
    client->sendTextMessage(QJsonDocument(telemetryData).toJson());
}
```

**Performance Considerations:**

| Metric | Value |
|--------|-------|
| Update latency | <5ms from SystemStateModel update to WebSocket send |
| Bandwidth (per client, all categories) | ~2 KB/message × 10 Hz = 20 KB/s = 160 Kbps |
| Bandwidth (gimbal + imu only) | ~500 bytes × 10 Hz = 5 KB/s = 40 Kbps |
| Max concurrent clients | 50 (configurable) |
| Heartbeat interval | 30 seconds (ping/pong) |
| Disconnect timeout | 90 seconds (no pong received) |

### 10.4 TelemetryConfig - Centralized Configuration

**File:** `src/services/telemetryconfig.h/cpp`

**Purpose:** Single source of truth for all telemetry configuration

**Structure:**

```cpp
struct TelemetryConfig {
    HttpApiConfig httpApi;          // REST API settings
    WebSocketConfig webSocket;      // WebSocket server settings
    TlsConfig tls;                  // TLS/SSL security settings
    ExportConfig exportSettings;    // Data export settings
    
    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath) const;
    QString validate() const;
};
```

**Example Configuration (JSON):**

```json
{
  "httpApi": {
    "enabled": true,
    "bindAddress": "0.0.0.0",
    "port": 8443,
    "maxConnections": 100,
    "requestTimeoutSec": 30,
    "enableCors": true,
    "corsOrigins": ["*"],
    "rateLimitPerMinute": 120
  },
  "webSocket": {
    "enabled": true,
    "bindAddress": "0.0.0.0",
    "port": 8444,
    "maxConnections": 50,
    "heartbeatIntervalSec": 30,
    "updateRateHz": 10,
    "enableCompression": true
  },
  "tls": {
    "enabled": false,
    "certificatePath": "/etc/rcws/ssl/cert.pem",
    "privateKeyPath": "/etc/rcws/ssl/key.pem",
    "protocol": "TLSv1.2OrLater"
  },
  "exportSettings": {
    "enableCsvExport": true,
    "exportDirectory": "./data/exports",
    "maxExportRangeDays": 30,
    "maxExportSizeMB": 100
  }
}
```

**Loading Configuration:**

```cpp
// In SystemController::createTelemetryServices()
TelemetryConfig telemetryConfig;

// Option 1: Load from file
if (QFile::exists("./config/telemetry_config.json")) {
    if (!telemetryConfig.loadFromFile("./config/telemetry_config.json")) {
        qWarning() << "Failed to load telemetry config, using defaults";
    }
}

// Option 2: Use programmatic defaults (current implementation)
telemetryConfig.httpApi.enabled = true;
telemetryConfig.httpApi.port = 8443;
telemetryConfig.webSocket.enabled = true;
telemetryConfig.webSocket.port = 8444;

// Validate configuration
QString error = telemetryConfig.validate();
if (!error.isEmpty()) {
    qCritical() << "Invalid telemetry config:" << error;
    return;
}
```

### 10.5 Service Performance & Resource Usage

**Measured on NVIDIA Jetson Orin AGX:**

| Service | CPU Usage | Memory Usage | Thread Count | Notes |
|---------|-----------|--------------|--------------|-------|
| **SystemDataLogger** | <1% | 8-50 MB | 1 (+ 1 DB thread) | Depends on buffer sizes |
| **TelemetryApiService** | <2% | 5 MB | 1 | HTTP request handling |
| **TelemetryAuthService** | <0.1% | 2 MB | 0 | Called from API threads |
| **TelemetryWebSocketServer** | 1-3% | 3 MB × clients | 1 | Increases with client count |
| **TOTAL** | **<5%** | **~25 MB** | **3** | With 10 WebSocket clients |

**Network Bandwidth Usage:**

| Scenario | Bandwidth |
|----------|-----------|
| 1 WebSocket client (all categories @ 10 Hz) | 160 Kbps |
| 10 WebSocket clients (all categories) | 1.6 Mbps |
| REST API query (1 minute gimbal history) | 360 KB (one-time) |
| CSV export (10 minutes IMU @ 100 Hz) | 6 MB (one-time) |

---


## 7. Hardware Abstraction Layer (MIL-STD)

### 7.1 Three-Layer Device Architecture

The RCWS hardware layer follows **MIL-STD (Military Standard) architecture patterns** with strict separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│  Device Layer (Business Logic & State Management)      │
│  - ImuDevice, ServoActuatorDevice, Plc21Device, etc.   │
│  - Manages device lifecycle & state                    │
│  - Thread-safe data access via TemplatedDevice<T>      │
│  - Emits high-level signals (dataUpdated)              │
└───────────────────────┬─────────────────────────────────┘
                        │ Uses
                        ▼
┌─────────────────────────────────────────────────────────┐
│  Protocol Parser Layer (Message Parsing)                │
│  - ImuProtocolParser, ModbusProtocolParser, etc.       │
│  - Parses raw bytes into structured messages           │
│  - Handles protocol-specific logic (CRC, checksums)    │
│  - Converts messages to/from DataModel structures      │
└───────────────────────┬─────────────────────────────────┘
                        │ Uses
                        ▼
┌─────────────────────────────────────────────────────────┐
│  Transport Layer (Raw I/O & Connection Management)     │
│  - SerialPortTransport, ModbusTransport                │
│  - Opens/closes physical connections                   │
│  - Reads/writes raw bytes                             │
│  - Emits low-level signals (dataReceived, errorOccurred)│
└─────────────────────────────────────────────────────────┘
```

**Benefits:**
- ✅ **Separation of Concerns:** Each layer has a single responsibility
- ✅ **Testability:** Mock transports/parsers for unit tests
- ✅ **Maintainability:** Protocol changes isolated in parsers
- ✅ **Reusability:** Same transport can be used by multiple devices
- ✅ **Thread Safety:** Built-in via TemplatedDevice pattern

### 7.2 Example: IMU Device Architecture

**Complete implementation showing all three layers:**

```cpp
// ============================================================================
// LAYER 1: TRANSPORT (SerialPortTransport)
// ============================================================================
class SerialPortTransport : public QObject {
    Q_OBJECT
public:
    bool open(const QString& portName, int baudRate) {
        m_serialPort = new QSerialPort(portName, this);
        m_serialPort->setBaudRate(baudRate);
        
        if (!m_serialPort->open(QIODevice::ReadWrite)) {
            return false;
        }
        
        connect(m_serialPort, &QSerialPort::readyRead,
                this, &SerialPortTransport::onReadyRead);
        return true;
    }
    
    void write(const QByteArray& data) {
        m_serialPort->write(data);
    }
    
signals:
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& error);
    
private slots:
    void onReadyRead() {
        QByteArray data = m_serialPort->readAll();
        emit dataReceived(data);
    }
    
private:
    QSerialPort* m_serialPort = nullptr;
};

// ============================================================================
// LAYER 2: PROTOCOL PARSER (ImuProtocolParser)
// ============================================================================
class ImuProtocolParser : public QObject {
    Q_OBJECT
public:
    struct ImuMessage {
        float roll;
        float pitch;
        float yaw;
        double gyroX, gyroY, gyroZ;
        double accelX, accelY, accelZ;
        bool valid;
    };
    
    ImuMessage parse(const QByteArray& rawData) {
        ImuMessage msg;
        
        // Protocol: 28 bytes binary
        // [0-3]: roll (float)
        // [4-7]: pitch (float)
        // [8-11]: yaw (float)
        // [12-19]: gyroX (double)
        // [20-27]: gyroY (double)
        // ...
        
        if (rawData.size() < 28) {
            msg.valid = false;
            return msg;
        }
        
        // Parse using Qt data stream
        QDataStream stream(rawData);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> msg.roll >> msg.pitch >> msg.yaw;
        stream >> msg.gyroX >> msg.gyroY >> msg.gyroZ;
        stream >> msg.accelX >> msg.accelY >> msg.accelZ;
        
        msg.valid = true;
        return msg;
    }
    
    QByteArray buildCommand(ImuCommand cmd) {
        // Build command bytes for IMU
        // ...
        return commandBytes;
    }
};

// ============================================================================
// LAYER 3: DEVICE (ImuDevice)
// ============================================================================
class ImuDevice : public TemplatedDevice<ImuDataModel> {
    Q_OBJECT
public:
    ImuDevice(SerialPortTransport* transport,
              ImuProtocolParser* parser,
              QObject* parent = nullptr)
        : TemplatedDevice<ImuDataModel>(parent)
        , m_transport(transport)
        , m_parser(parser)
    {
        // Connect transport → parser → device
        connect(m_transport, &SerialPortTransport::dataReceived,
                this, &ImuDevice::onTransportDataReceived);
    }
    
    bool start() override {
        return m_transport->open("/dev/ttyUSB0", 115200);
    }
    
    void stop() override {
        m_transport->close();
    }
    
signals:
    void dataUpdated(const ImuDataModel& data);
    
private slots:
    void onTransportDataReceived(const QByteArray& rawData) {
        // Parse raw bytes
        auto message = m_parser->parse(rawData);
        
        if (!message.valid) {
            qWarning() << "IMU: Invalid message received";
            return;
        }
        
        // Update device data model
        ImuDataModel newData;
        newData.roll = message.roll;
        newData.pitch = message.pitch;
        newData.yaw = message.yaw;
        newData.gyroX = message.gyroX;
        newData.gyroY = message.gyroY;
        newData.gyroZ = message.gyroZ;
        newData.accelX = message.accelX;
        newData.accelY = message.accelY;
        newData.accelZ = message.accelZ;
        newData.timestamp = QDateTime::currentDateTime();
        
        setData(newData);  // Thread-safe write
        emit dataUpdated(newData);
    }
    
private:
    SerialPortTransport* m_transport;
    ImuProtocolParser* m_parser;
};
```

**Usage in HardwareManager:**

```cpp
// Create all three layers with dependency injection
auto* transport = new SerialPortTransport(this);
auto* parser = new ImuProtocolParser(this);
auto* device = new ImuDevice(transport, parser, this);

// Start device (opens transport)
device->start();

// Access data (thread-safe)
ImuDataModel currentData = device->data();
```

### 7.3 Implemented Devices

| # | Device Class | Protocol Parser | Transport | Update Freq |
|---|--------------|-----------------|-----------|-------------|
| 1 | **ImuDevice** | ImuProtocolParser | SerialPortTransport | 100 Hz |
| 2 | **ServoActuatorDevice** (Az) | ModbusProtocolParser | ModbusTransport | 60 Hz |
| 3 | **ServoActuatorDevice** (El) | ModbusProtocolParser | ModbusTransport | 60 Hz |
| 4 | **Plc21Device** | ModbusProtocolParser | ModbusTransport | 10 Hz |
| 5 | **Plc42Device** | ModbusProtocolParser | ModbusTransport | 10 Hz |
| 6 | **LrfDevice** | LrfProtocolParser | SerialPortTransport | On-demand |
| 7 | **RadarDevice** | RadarProtocolParser | SerialPortTransport | 10 Hz |
| 8 | **DayCameraControlDevice** | ViscaProtocolParser | SerialPortTransport | On-command |
| 9 | **NightCameraControlDevice** | FlirProtocolParser | SerialPortTransport | On-command |
| 10 | **CameraVideoStreamDevice** (Day) | GStreamer Pipeline | V4L2 (GStreamer) | 30 FPS |
| 11 | **CameraVideoStreamDevice** (Night) | GStreamer Pipeline | V4L2 (GStreamer) | 30 FPS |

---

## 8. MVVM Pattern & Data Flow

### 8.1 MVVM Architecture

The RCWS UI follows the **Model-View-ViewModel (MVVM)** pattern for clean separation between UI and business logic:

```
┌─────────────────────────────────────────────────────────┐
│  VIEW LAYER (QML)                                       │
│  - Declarative UI (main.qml, OsdOverlay.qml, etc.)     │
│  - No business logic                                   │
│  - Only data binding and event handlers                │
└───────────────────┬─────────────────────────────────────┘
                    │ Q_PROPERTY bindings
                    │ Signal connections
                    ▼
┌─────────────────────────────────────────────────────────┐
│  VIEWMODEL LAYER (C++)                                  │
│  - Exposes data to QML via Q_PROPERTY                  │
│  - Transforms SystemStateModel data for UI presentation│
│  - No direct hardware access                           │
│  - Example: OsdViewModel, ZeroingViewModel             │
└───────────────────┬─────────────────────────────────────┘
                    │ Qt signals/slots
                    │ Direct method calls
                    ▼
┌─────────────────────────────────────────────────────────┐
│  CONTROLLER LAYER (C++)                                 │
│  - Manages ViewModel state                             │
│  - Handles user commands                               │
│  - Orchestrates business logic                         │
│  - Example: OsdController, GimbalController            │
└───────────────────┬─────────────────────────────────────┘
                    │ Reads SystemStateModel
                    │ Commands hardware devices
                    ▼
┌─────────────────────────────────────────────────────────┐
│  MODEL LAYER (C++)                                      │
│  - SystemStateModel (central data hub)                 │
│  - Device data models (ImuDataModel, etc.)             │
│  - Pure data structures, no UI logic                   │
└─────────────────────────────────────────────────────────┘
```

### 8.2 Unidirectional Data Flow

**Critical architectural constraint:** Data flows in **one direction only**

```
Hardware → Device → DataModel → SystemStateModel → ViewModel → QML View

❌ NEVER: QML → SystemStateModel (no direct access)
✅ CORRECT: QML → ViewModel signal → Controller → Hardware
```

**Example: User Changes Zeroing Offset**

```
QML Slider (ZeroingOverlay.qml)
    onValueChanged: zeroingViewModel.setAzimuthOffset(value)
        ↓
ZeroingViewModel::setAzimuthOffset(float value)
    emit azimuthOffsetChangeRequested(value)
        ↓
ZeroingController::onAzimuthOffsetChangeRequested(float value)
    m_systemStateModel->setZeroingAzimuthOffset(value)
        ↓
SystemStateModel::setZeroingAzimuthOffset(float value)
    m_data.zeroingAzimuthOffset = value
    emit dataChanged(m_data)
        ↓
ZeroingViewModel::onSystemStateChanged(SystemStateData data)
    m_azimuthOffset = data.zeroingAzimuthOffset
    emit azimuthOffsetChanged()  // Q_PROPERTY notification
        ↓
QML Slider updates automatically (property binding)
```

**Benefits of Unidirectional Flow:**
- ✅ **Predictable state:** Always know where data comes from
- ✅ **Easier debugging:** Trace data flow in one direction
- ✅ **No circular dependencies:** Prevents infinite update loops
- ✅ **Testable:** Can mock any layer independently

### 8.3 Example: OsdViewModel

**Purpose:** Expose OSD-relevant data to QML

```cpp
class OsdViewModel : public QObject {
    Q_OBJECT
    
    // Gimbal position
    Q_PROPERTY(float gimbalAz READ gimbalAz NOTIFY gimbalAzChanged)
    Q_PROPERTY(float gimbalEl READ gimbalEl NOTIFY gimbalElChanged)
    
    // Camera
    Q_PROPERTY(bool activeCameraIsDay READ activeCameraIsDay NOTIFY activeCameraIsDayChanged)
    Q_PROPERTY(float currentZoom READ currentZoom NOTIFY currentZoomChanged)
    Q_PROPERTY(float currentHFOV READ currentHFOV NOTIFY currentHFOVChanged)
    
    // Tracking
    Q_PROPERTY(bool trackingActive READ trackingActive NOTIFY trackingActiveChanged)
    Q_PROPERTY(int trackingPhase READ trackingPhase NOTIFY trackingPhaseChanged)
    
    // Weapon
    Q_PROPERTY(bool gunArmed READ gunArmed NOTIFY gunArmedChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)
    
    // LRF
    Q_PROPERTY(float lrfDistance READ lrfDistance NOTIFY lrfDistanceChanged)
    
    // Reticle
    Q_PROPERTY(float reticleOffsetX READ reticleOffsetX NOTIFY reticleOffsetXChanged)
    Q_PROPERTY(float reticleOffsetY READ reticleOffsetY NOTIFY reticleOffsetYChanged)
    
    // ... (30+ properties total)
    
public:
    explicit OsdViewModel(QObject* parent = nullptr);
    
    // Getters
    float gimbalAz() const { return m_gimbalAz; }
    float gimbalEl() const { return m_gimbalEl; }
    bool trackingActive() const { return m_trackingActive; }
    // ...
    
public slots:
    // Connected to SystemStateModel::dataChanged
    void onSystemStateChanged(const SystemStateData& data) {
        if (m_gimbalAz != data.gimbalAz) {
            m_gimbalAz = data.gimbalAz;
            emit gimbalAzChanged();
        }
        if (m_gimbalEl != data.gimbalEl) {
            m_gimbalEl = data.gimbalEl;
            emit gimbalElChanged();
        }
        // ... update all properties
    }
    
signals:
    void gimbalAzChanged();
    void gimbalElChanged();
    void trackingActiveChanged();
    // ... (30+ signals)
    
private:
    float m_gimbalAz = 0.0f;
    float m_gimbalEl = 0.0f;
    bool m_trackingActive = false;
    // ... (30+ member variables)
};
```

**Usage in QML:**

```qml
// OsdOverlay.qml
Item {
    Text {
        text: "AZ: " + osdViewModel.gimbalAz.toFixed(2) + "°"
        
        // Automatically updates when osdViewModel.gimbalAzChanged() emitted
    }
    
    Rectangle {
        color: osdViewModel.gunArmed ? "red" : "green"
        
        // Automatically updates when osdViewModel.gunArmedChanged() emitted
    }
    
    TrackingBox {
        visible: osdViewModel.trackingActive
        phase: osdViewModel.trackingPhase
        
        // Automatically updates when properties change
    }
}
```

**Connection in SystemController:**

```cpp
// Phase 2: initializeQmlSystem()
m_osdViewModel = new OsdViewModel(this);

// Connect to SystemStateModel
connect(m_systemStateModel, &SystemStateModel::dataChanged,
        m_osdViewModel, &OsdViewModel::onSystemStateChanged);

// Register with QML
context->setContextProperty("osdViewModel", m_osdViewModel);
```

---

## 9. Motion Control System

### 9.1 Strategy Pattern for Motion Modes

The gimbal controller implements **5 different motion modes** using the **Strategy Pattern**:

```
GimbalController
    ├── Manual Motion Mode (joystick slewing)
    ├── Tracking Motion Mode (VPI DCF tracker)
    ├── Auto Sector Scan Mode (automated scanning)
    ├── Radar Slew Mode (slew to radar plot)
    └── TRP Scan Mode (visit preset target reference points)
```

**Base class:**

```cpp
class GimbalMotionModeBase : public QObject {
    Q_OBJECT
public:
    virtual ~GimbalMotionModeBase() = default;
    
    // Called every 16ms (60 Hz) to compute gimbal commands
    virtual void update(float deltaTime) = 0;
    
    // Called when mode is activated
    virtual void onActivate() {}
    
    // Called when mode is deactivated
    virtual void onDeactivate() {}
    
protected:
    GimbalController* m_gimbalController = nullptr;
    SystemStateModel* m_systemStateModel = nullptr;
};
```

**Example: Tracking Motion Mode**

```cpp
class TrackingMotionMode : public GimbalMotionModeBase {
public:
    void update(float deltaTime) override {
        const auto& state = m_systemStateModel->data();
        
        // Check if tracking is active
        if (!state.trackingActive || !state.trackerHasValidTarget) {
            return;  // No tracking, do nothing
        }
        
        // Get target angles from tracking system
        float targetAz = state.targetAz;
        float targetEl = state.targetEl;
        
        // Compute error (difference between target and current)
        float azError = targetAz - state.gimbalAz;
        float elError = targetEl - state.gimbalEl;
        
        // PID control to compute servo speeds
        float azSpeed = m_azPID.compute(azError, deltaTime);
        float elSpeed = m_elPID.compute(elError, deltaTime);
        
        // Send commands to servos
        m_gimbalController->commandAzimuthSpeed(azSpeed);
        m_gimbalController->commandElevationSpeed(elSpeed);
    }
    
private:
    PIDController m_azPID;
    PIDController m_elPID;
};
```

**Mode Switching:**

```cpp
// In GimbalController
void GimbalController::setMotionMode(MotionMode mode) {
    // Deactivate current mode
    if (m_currentMode) {
        m_currentMode->onDeactivate();
    }
    
    // Activate new mode
    switch (mode) {
        case MotionMode::Manual:
            m_currentMode = m_manualMode;
            break;
        case MotionMode::Tracking:
            m_currentMode = m_trackingMode;
            break;
        case MotionMode::AutoSectorScan:
            m_currentMode = m_sectorScanMode;
            break;
        // ...
    }
    
    m_currentMode->onActivate();
}

// Called every 16ms from QTimer
void GimbalController::onUpdateTimer() {
    float deltaTime = 0.016f;  // 16ms = 60 Hz
    
    if (m_currentMode) {
        m_currentMode->update(deltaTime);
    }
}
```

---

## 14. Error Handling & Logging Strategy

### 14.1 Logging Levels

The RCWS system uses Qt's logging framework with **four severity levels**:

```cpp
qDebug()    // Development/troubleshooting (disabled in release builds)
qInfo()     // Informational messages (system state changes)
qWarning()  // Warnings (recoverable errors, degraded functionality)
qCritical() // Critical errors (system failures, safety violations)
```

**Examples:**

```cpp
// Informational: System initialization
qInfo() << "=== PHASE 1: Hardware Initialization ===";
qInfo() << "  ✓ IMU device created";

// Warning: Device communication timeout (recoverable)
qWarning() << "IMU: No data received for 5 seconds";

// Critical: Safety violation
qCritical() << "EMERGENCY: Reticle in no-fire zone! Disabling weapon.";
```

### 14.2 Device Connection Handling

**Three connection states:**

| State | Color | Action |
|-------|-------|--------|
| **Connected** | Green | Normal operation |
| **Disconnected** | Red | Show warning on OSD, degrade gracefully |
| **Error** | Flashing Red | Show critical alarm, may enter safe mode |

**Example: IMU disconnection handling**

```cpp
// In ImuDevice
void ImuDevice::onTransportError(const QString& error) {
    qWarning() << "IMU: Transport error:" << error;
    
    m_connected = false;
    emit connectionStatusChanged(false);
    
    // Inform SystemStateModel
    ImuDataModel errorData;
    errorData.connected = false;
    errorData.error = error;
    setData(errorData);
}

// In SystemStateModel
void SystemStateModel::onImuDataChanged(const ImuDataModel& data) {
    m_data.imuConnected = data.connected;
    
    if (!data.connected) {
        // Disable stabilization when IMU disconnected
        m_data.enableStabilization = false;
        
        qWarning() << "IMU disconnected, stabilization disabled";
    }
    
    emit dataChanged(m_data);
}

// In OsdViewModel → displays warning on screen
// In SystemStatusViewModel → shows red status indicator
```

### 14.3 Graceful Degradation

**Philosophy:** Continue operating with reduced functionality rather than total failure

| Failure | System Response | Degraded Functionality |
|---------|-----------------|------------------------|
| **IMU disconnected** | Warning on OSD, continue | No stabilization, no roll/pitch compensation |
| **Day camera failed** | Switch to night camera | Thermal-only operation |
| **LRF timeout** | Show "LRF: ---" on OSD | Manual range estimation required |
| **Radar offline** | Hide radar overlay | No radar-based tracking |
| **Servo alarm** | Emergency stop, lock gimbal | Operator must clear alarm manually |

**Emergency stop behavior (critical failure):**

```cpp
void GimbalController::onEmergencyStop() {
    qCritical() << "EMERGENCY STOP ACTIVATED";
    
    // Stop all motion immediately
    commandAzimuthSpeed(0);
    commandElevationSpeed(0);
    
    // Disable weapon
    m_weaponController->disarm();
    
    // Enter safe mode
    m_systemStateModel->setOperationalMode(OperationalMode::Safe);
    
    // Display critical alarm on OSD
    emit emergencyStopActivated();
}
```

---

## 15. Performance & Memory Analysis

### 15.1 CPU Usage Breakdown (Jetson Orin AGX)

| Subsystem | CPU Usage | Notes |
|-----------|-----------|-------|
| **QML Rendering** | 8-12% | OSD updates @ 30 FPS |
| **Servo Control** | 3-5% | Two threads @ 60 Hz |
| **Video Processing (Day)** | 15-20% | GStreamer + VPI tracking |
| **Video Processing (Night)** | 10-15% | GStreamer only |
| **Device Communication** | 2-3% | Serial + Modbus |
| **Telemetry Services** | 1-3% | HTTP + WebSocket |
| **Data Logging** | <1% | Ring buffers |
| **Idle / Other** | ~40% | System overhead |
| **TOTAL** | **50-65%** | 4 cores utilized |

### 15.2 Memory Usage

| Component | RAM Usage |
|-----------|-----------|
| **Qt Framework** | ~150 MB |
| **QML Engine** | ~80 MB |
| **Video Buffers** | ~120 MB (2× cameras @ 1280×720) |
| **VPI Tracking** | ~250 MB (CUDA memory) |
| **YOLO Detection** | ~400 MB (if enabled) |
| **Data Logger Buffers** | ~8-50 MB |
| **Telemetry Services** | ~25 MB |
| **Application Code** | ~100 MB |
| **TOTAL** | **~800 MB** (without YOLO)<br>**~1200 MB** (with YOLO) |

### 15.3 Update Frequencies

| Data Source | Target Freq | Actual Measured | Jitter |
|-------------|-------------|-----------------|--------|
| Servo Position | 60 Hz | 58-60 Hz | ±2 Hz |
| IMU Data | 100 Hz | 95-100 Hz | ±3 Hz |
| Video Frame | 30 FPS | 28-30 FPS | ±1 FPS |
| VPI Tracking | 30 Hz | 25-30 Hz | ±2 Hz |
| OSD Rendering | 30 FPS | 28-30 FPS | ±1 FPS |
| Joystick Input | 100 Hz | 90-100 Hz | ±5 Hz |
| Telemetry WebSocket | 10 Hz | 10 Hz | <0.5 Hz |

---

# SUMMARY

## Key Architectural Achievements

1. **Threading Model:** 6-thread architecture with thread-safe data access
2. **Three-Phase Initialization:** Predictable, dependency-ordered startup
3. **Manager Pattern:** Modular subsystem organization
4. **MIL-STD Hardware Layer:** Three-layer device abstraction
5. **MVVM Pattern:** Clean separation of UI and business logic
6. **Services Layer:** Cross-cutting concerns (logging, telemetry, auth)
7. **Motion Control:** Strategy pattern with 5 motion modes
8. **Data Logging:** Time-series storage with 9 categories, ring buffers, SQLite
9. **Telemetry API:** REST + WebSocket with JWT authentication
10. **Graceful Degradation:** Resilient to device failures

## Development Guidelines

**When adding a new feature:**
1. Start with `SystemStateData` (add required fields)
2. Create/update `DataModel` (if new device)
3. Create `ViewModel` (expose to QML)
4. Create `Controller` (business logic)
5. Create QML UI components
6. Register in appropriate Manager
7. Test thoroughly (especially threading!)

**When adding a new device:**
1. Implement three layers: Transport → Parser → Device
2. Extend `TemplatedDevice<YourDataModel>`
3. Create in `HardwareManager::createHardware()`
4. Connect in `HardwareManager::connectDevicesToModels()`
5. Add to `SystemStateModel`

**For more details, see existing documentation:**
- `HARDWARE_ARCHITECTURE.md` - MIL-STD device layer details
- `DATALOGGER_DOCUMENTATION.md` - Data logger implementation
- README.md - User-facing features and deployment

---

**Document End**

**Revision History:**
- Version 1.0 (2025-01-08): Initial comprehensive SDD covering all architectural patterns

