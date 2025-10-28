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
 DIRECTORY STRUCTURE:
----------------------------------------
└── QT6-gstreamer-example
    ├── CCIP.md
    ├── QT6-gstreamer-example.pro
    ├── agent.md
    ├── config
    │   └── devices.json
    ├── data
    ├── documentation
    │   └── DATALOGGER_DOCUMENTATION.md
    ├── joystick_manual_md.md
    ├── post_git.md
    ├── preview.webp
    ├── qml
    │   ├── common
    │   │   ├── NavigableList.qml
    │   │   └── ParameterField.qml
    │   ├── components
    │   │   ├── AboutDialog.qml
    │   │   ├── AreaZoneParameterPanel.qml
    │   │   ├── AzimuthIndicator.qml
    │   │   ├── ElevationScale.qml
    │   │   ├── OsdOverlay.qml
    │   │   ├── ReticleRenderer.qml
    │   │   ├── SectorScanParameterPanel.qml
    │   │   ├── SystemStatusOverlay.qml
    │   │   ├── TRPParameterPanel.qml
    │   │   ├── TrackingBox.qml
    │   │   ├── WindageOverlay.qml
    │   │   ├── ZeroingOverlay.qml
    │   │   ├── ZoneDefinitionOverlay.qml
    │   │   └── ZoneMapCanvas.qml
    │   └── views
    │       ├── MainMenu.qml
    │       └── main.qml
    ├── readme.md
    ├── resources
    │   └── resources.qrc
    ├── scripts
    │   ├── cleanup_virtual_ports.sh
    │   ├── run_full_test.sh
    │   ├── setup_virtual_ports.sh
    │   ├── sim_plc21.sh
    │   ├── sim_plc42.sh
    │   ├── sim_servo_az.sh
    │   ├── sim_servo_el.sh
    │   ├── start_modbus_simulators.sh
    │   └── verify_setup.sh
    └── src
        ├── controllers
        │   ├── aboutcontroller.cpp
        │   ├── aboutcontroller.h
        │   ├── applicationcontroller.cpp
        │   ├── applicationcontroller.h
        │   ├── cameracontroller.cpp
        │   ├── cameracontroller.h
        │   ├── colormenucontroller.cpp
        │   ├── colormenucontroller.h
        │   ├── deviceconfiguration.cpp
        │   ├── deviceconfiguration.h
        │   ├── gimbalcontroller.cpp
        │   ├── gimbalcontroller.h
        │   ├── joystickcontroller.cpp
        │   ├── joystickcontroller.h
        │   ├── mainmenucontroller.cpp
        │   ├── mainmenucontroller.h
        │   ├── motion_modes
        │   │   ├── autosectorscanmotionmode.cpp
        │   │   ├── autosectorscanmotionmode.h
        │   │   ├── gimbalmotionmodebase.cpp
        │   │   ├── gimbalmotionmodebase.h
        │   │   ├── manualmotionmode.cpp
        │   │   ├── manualmotionmode.h
        │   │   ├── pidcontroller.h
        │   │   ├── radarslewmotionmode.cpp
        │   │   ├── radarslewmotionmode.h
        │   │   ├── trackingmotionmode.cpp
        │   │   ├── trackingmotionmode.h
        │   │   ├── trpscanmotionmode.cpp
        │   │   └── trpscanmotionmode.h
        │   ├── osdcontroller.cpp
        │   ├── osdcontroller.h
        │   ├── reticlemenucontroller.cpp
        │   ├── reticlemenucontroller.h
        │   ├── systemcontroller.cpp
        │   ├── systemcontroller.h
        │   ├── systemstatuscontroller.cpp
        │   ├── systemstatuscontroller.h
        │   ├── weaponcontroller.cpp
        │   ├── weaponcontroller.h
        │   ├── windagecontroller.cpp
        │   ├── windagecontroller.h
        │   ├── zeroingcontroller.cpp
        │   ├── zeroingcontroller.h
        │   ├── zonedefinitioncontroller.cpp
        │   └── zonedefinitioncontroller.h
        ├── hardware
        │   └── devices
        │       ├── baseserialdevice.cpp
        │       ├── baseserialdevice.h
        │       ├── cameravideostreamdevice.cpp
        │       ├── cameravideostreamdevice.h
        │       ├── daycameracontroldevice.cpp
        │       ├── daycameracontroldevice.h
        │       ├── imudevice.cpp
        │       ├── imudevice.h
        │       ├── joystickdevice.cpp
        │       ├── joystickdevice.h
        │       ├── lensdevice.cpp
        │       ├── lensdevice.h
        │       ├── lrfdevice.cpp
        │       ├── lrfdevice.h
        │       ├── modbusdevicebase.cpp
        │       ├── modbusdevicebase.h
        │       ├── nightcameracontroldevice.cpp
        │       ├── nightcameracontroldevice.h
        │       ├── plc21device.cpp
        │       ├── plc21device.h
        │       ├── plc42device.cpp
        │       ├── plc42device.h
        │       ├── radardevice.cpp
        │       ├── radardevice.h
        │       ├── servoactuatordevice.cpp
        │       ├── servoactuatordevice.h
        │       ├── servodriverdevice.cpp
        │       ├── servodriverdevice.h
        │       └── vpi_helpers.h
        ├── logger
        │   ├── systemdatalogger.cpp
        │   └── systemdatalogger.h
        ├── main.cpp
        ├── models
        │   ├── aboutviewmodel.cpp
        │   ├── aboutviewmodel.h
        │   ├── areazoneparameterviewmodel.cpp
        │   ├── areazoneparameterviewmodel.h
        │   ├── domain
        │   │   ├── daycameradatamodel.h
        │   │   ├── gyrodatamodel.h
        │   │   ├── joystickdatamodel.cpp
        │   │   ├── joystickdatamodel.h
        │   │   ├── lensdatamodel.h
        │   │   ├── lrfdatamodel.h
        │   │   ├── nightcameradatamodel.h
        │   │   ├── plc21datamodel.h
        │   │   ├── plc42datamodel.h
        │   │   ├── radardatamodel.h
        │   │   ├── servoactuatordatamodel.h
        │   │   ├── servodriverdatamodel.h
        │   │   ├── systemstatedata.h
        │   │   ├── systemstatemodel.cpp
        │   │   └── systemstatemodel.h
        │   ├── historyviewmodel.cpp
        │   ├── historyviewmodel.h
        │   ├── menuviewmodel.cpp
        │   ├── menuviewmodel.h
        │   ├── osdviewmodel.cpp
        │   ├── osdviewmodel.h
        │   ├── sectorscanparameterviewmodel.cpp
        │   ├── sectorscanparameterviewmodel.h
        │   ├── systemstatusviewmodel.cpp
        │   ├── systemstatusviewmodel.h
        │   ├── trpparameterviewmodel.cpp
        │   ├── trpparameterviewmodel.h
        │   ├── viewmodels
        │   ├── windageviewmodel.cpp
        │   ├── windageviewmodel.h
        │   ├── zeroingviewmodel.cpp
        │   ├── zeroingviewmodel.h
        │   ├── zonedefinitionviewmodel.cpp
        │   ├── zonedefinitionviewmodel.h
        │   ├── zonemapviewmodel.cpp
        │   └── zonemapviewmodel.h
        ├── services
        │   ├── servicemanager.cpp
        │   ├── servicemanager.h
        │   ├── zonegeometryservice.cpp
        │   └── zonegeometryservice.h
        ├── utils
        │   ├── TimestampLogger.h
        │   ├── ballisticsprocessor.cpp
        │   ├── ballisticsprocessor.h
        │   ├── colorutils.cpp
        │   ├── colorutils.h
        │   ├── inference.cpp
        │   ├── inference.h
        │   ├── millenious.h
        │   ├── reticleaimpointcalculator.cpp
        │   ├── reticleaimpointcalculator.h
        │   └── targetstate.h
        └── video
            ├── gstvideosource.cpp
            ├── gstvideosource.h
            ├── videoimageprovider.cpp
            └── videoimageprovider.h


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
