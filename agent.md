# Qt6/QML Project Refactoring Report

This document outlines the reorganization of the repository into a structured Qt6/QML project.

## Project Structure

The project has been reorganized into the following directory structure:

```
.
├── QT6-gstreamer-example.pro
├── agent.md
├── qml
│   ├── components
│   │   └── OsdOverlay.qml
│   └── views
│       ├── MainMenu.qml
│       ├── Menu.qml
│       └── main.qml
├── resources
│   └── resources.qrc
└── src
    ├── controllers
    │   ├── applicationcontroller.cpp
    │   ├── applicationcontroller.h
    │   ├── colormenucontroller.cpp
    │   ├── colormenucontroller.h
    │   ├── mainmenucontroller.cpp
    │   ├── mainmenucontroller.h
    │   ├── reticlemenucontroller.cpp
    │   └── reticlemenucontroller.h
    ├── main.cpp
    ├── models
    │   ├── menuviewmodel.cpp
    │   ├── menuviewmodel.h
    │   ├── osdviewmodel.cpp
    │   └── osdviewmodel.h
    ├── services
    │   ├── servicemanager.cpp
    │   └── servicemanager.h
    └── video
        ├── gstvideosource.cpp
        ├── gstvideosource.h
        ├── videoimageprovider.cpp
        └── videoimageprovider.h
```

## New Project Structure


```
    QT6-gstreamer-example/
    ├── qml/
    │   ├── components/          # Reusable UI components
    │   │   ├── MenuOverlay.qml
    │   │   ├── ZeroingOverlay.qml
    │   │   └── OSDOverlay.qml
    │   └── views/
    │       └── main.qml
    │
    ├── src/
    │   ├── controllers/         # Application Flow Controllers
    │   │   ├── ApplicationController       # Root coordinator
    │   │   ├── MainMenuController
    │   │   ├── GimbalController           # Controls gimbal movement
    │   │   ├── TrackingController         # Manages target tracking
    │   │   ├── JoystickController         # Joystick input handler
    │   │   ├── ZeroingController
    │   │   ├── WindageController
    │   │   └── FireControlController      # Fire control logic
    │   │
    │   ├── models/              # Data Models & ViewModels
    │   │   ├── viewmodels/      # UI ViewModels
    │   │   │   ├── MenuViewModel
    │   │   │   ├── OsdViewModel
    │   │   │   ├── ZeroingViewModel
    │   │   │   └── WindageViewModel
    │   │   │
    │   │   └── domain/          # Domain/Business Models
    │   │       ├── SystemState.h           # Core system state (read-only for most)
    │   │       ├── GimbalState.h           # Gimbal position, status
    │   │       ├── BallisticsState.h       # Zeroing, windage, lead angle
    │   │       ├── TrackingState.h         # Tracking lock, target info
    │   │       ├── ZoneDefinitions.h       # Area zones, TRPs, etc.
    │   │       └── WeaponState.h           # Fire control, safety
    │   │
    │   ├── services/            # Singleton Business Logic Services
    │   │   ├── ServiceManager              # DI container
    │   │   ├── StateCoordinator            # Central state management
    │   │   ├── ZoneManager                 # Manage zones (no-fire, TRP, etc.)
    │   │   ├── BallisticsService           # Zeroing, windage, lead angle
    │   │   ├── TrackingService             # Target tracking logic
    │   │   ├── SafetyService               # Fire control safety checks
    │   │   └── ConfigurationService        # Save/load settings
    │   │
    │   ├── hardware/            # Hardware Interface Layer
    │   │   ├── interfaces/      # Abstract interfaces
    │   │   │   ├── IServoDriver.h
    │   │   │   ├── ISensor.h
    │   │   │   └── IInputDevice.h
    │   │   │
    │   │   ├── devices/         # Concrete device implementations
    │   │   │   ├── Plc21Device
    │   │   │   ├── Plc42Device
    │   │   │   ├── ServoAzimuthDriver
    │   │   │   ├── ServoElevationDriver
    │   │   │   ├── LrfDevice              # Laser Range Finder
    │   │   │   ├── ImuDevice              # Gyroscope/IMU
    │   │   │   ├── JoystickDevice
    │   │   │   ├── DayCameraDevice
    │   │   │   ├── NightCameraDevice
    │   │   │   └── RadarDevice
    │   │   │
    │   │   └── protocols/       # Communication protocols
    │   │       ├── ModbusProtocol
    │   │       ├── SerialProtocol
    │   │       └── CanBusProtocol
    │   │
    │   ├── utils/               # Utility classes
    │   │   ├── ReticleAimpointCalculator
    │   │   ├── GeometryUtils
    │   │   └── TimestampLogger
    │   │
    │   └── video/               # Video pipeline
    │       ├── GstVideoSource
    │       └── VideoImageProvider


```
## Rationale for Reorganization

The project was reorganized to follow Qt/QML best practices, improving maintainability, scalability, and developer onboarding. The key principles were:

*   **Separation of Concerns:** C++ backend code (`src`), QML frontend code (`qml`), and resource files (`resources`) are now in distinct top-level directories.
*   **Domain-Driven C++ Structure:** The `src` directory is further divided by functionality (`controllers`, `models`, `services`, `video`), making it easier to locate and understand specific parts of the C++ codebase.
*   **Component-Based QML Structure:** The `qml` directory separates reusable UI controls (`components`) from application screens (`views`). This encourages modularity and code reuse.
 
## Build and Runtime Changes

### Build System (`.pro` file)

*   **`QT6-gstreamer-example.pro`:**
    *   The `SOURCES` and `HEADERS` variables were updated with the new paths for all C++ files (e.g., `src/controllers/applicationcontroller.cpp`).
    *   The `RESOURCES` variable was updated to `resources/resources.qrc`.
    *   `QML_IMPORT_PATH` was set to `qml` to help Qt Creator's code model resolve QML imports.

### Resources (`.qrc` file)

*   **`resources/resources.qrc`:**
    *   The resource `prefix` was changed from `/` to `/qml` for better namespacing.
    *   File paths were updated to reflect the new `qml/views` and `qml/components` structure (e.g., `../qml/views/main.qml`).
    *   A missing entry for `Menu.qml` was added.

### Runtime Code

*   **`src/main.cpp`:**
    *   The QML engine loading path was changed from `qrc:/qml/main.qml` to `qrc:/qml/views/main.qml` to match the new resource path.
    *   All `#include` statements were updated to use relative paths to the new header locations (e.g., `#include "controllers/applicationcontroller.h"`).
*   **Other C++ Files:** All `#include` statements were updated to use relative paths (e.g., `../services/servicemanager.h` from within a controller).
*   **`qml/views/main.qml`:**
    *   Import statements were added for the `components` and `views` directories to make dependencies explicit: `import "qrc:/qml/components"` and `import "qrc:/qml/views"`.
