QT += quick serialbus serialport dbus sql httpserver

#LIBS += -L/usr/lib/x86_64-linux-gnu/gstreamer-1.0 -lgstxvimagesink
INCLUDEPATH += "/usr/include/gstreamer-1.0"
INCLUDEPATH += src

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0
PKGCONFIG += gstreamer-video-1.0

INCLUDEPATH += "/usr/include/vpi3"
INCLUDEPATH += "/opt/nvidia/vpi3/include"
INCLUDEPATH += /usr/include/SDL2
LIBS += -L/opt/nvidia/vpi3/lib/x86_64-linux-gnu -lnvvpi
LIBS += -lSDL2

LIBS += -lgstreamer-1.0 -lgstapp-1.0 -lgstbase-1.0 -lgobject-2.0 -lglib-2.0
PKGCONFIG += gstreamer-gl-1.0

# Common configurations
#INCLUDEPATH += "/usr/include/opencv4"
INCLUDEPATH += "/usr/local/include/opencv4"
INCLUDEPATH += "/usr/include/eigen3"
INCLUDEPATH += "/usr/include/glib-2.0"

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc
LIBS += -L/usr/local/lib -lopencv_core   -lopencv_dnn -lopencv_videoio
PKGCONFIG += gstreamer-gl-1.0

INCLUDEPATH += /usr/local/cuda/include
LIBS += -L/usr/local/cuda/lib64 -lcudart

SOURCES += \
    src/controllers/aboutcontroller.cpp \
    src/controllers/applicationcontroller.cpp \
    src/controllers/cameracontroller.cpp \
    src/controllers/colormenucontroller.cpp \
    src/controllers/deviceconfiguration.cpp \
    src/controllers/gimbalcontroller.cpp \
    src/controllers/joystickcontroller.cpp \
    src/controllers/mainmenucontroller.cpp \
    src/controllers/motion_modes/autosectorscanmotionmode.cpp \
    src/controllers/motion_modes/gimbalmotionmodebase.cpp \
    src/controllers/motion_modes/manualmotionmode.cpp \
    src/controllers/motion_modes/radarslewmotionmode.cpp \
    src/controllers/motion_modes/trackingmotionmode.cpp \
    src/controllers/motion_modes/trpscanmotionmode.cpp \
    src/controllers/osdcontroller.cpp \
    src/controllers/reticlemenucontroller.cpp \
    src/controllers/systemcontroller.cpp \
    src/controllers/systemstatuscontroller.cpp \
    src/controllers/weaponcontroller.cpp \
    src/controllers/windagecontroller.cpp \
    src/controllers/zeroingcontroller.cpp \
    src/controllers/zonedefinitioncontroller.cpp \
    src/managers/HardwareManager.cpp \
    src/managers/ViewModelRegistry.cpp \
    src/managers/ControllerRegistry.cpp \
    src/config/ConfigurationValidator.cpp \
    src/hardware/devices/cameravideostreamdevice.cpp \
    src/hardware/devices/daycameracontroldevice.cpp \
    src/hardware/devices/imudevice.cpp \
    src/hardware/devices/joystickdevice.cpp \
    src/hardware/devices/lrfdevice.cpp \
    src/hardware/devices/nightcameracontroldevice.cpp \
    src/hardware/devices/plc21device.cpp \
    src/hardware/devices/plc42device.cpp \
    src/hardware/devices/radardevice.cpp \
    src/hardware/devices/servoactuatordevice.cpp \
    src/hardware/devices/servodriverdevice.cpp \
    src/logger/systemdatalogger.cpp \
    src/main.cpp \
    src/models/aboutviewmodel.cpp \
    src/models/areazoneparameterviewmodel.cpp \
    src/models/domain/joystickdatamodel.cpp \
    src/models/domain/systemstatemodel.cpp \
    src/models/historyviewmodel.cpp \
    src/models/menuviewmodel.cpp \
    src/models/osdviewmodel.cpp \
    src/models/sectorscanparameterviewmodel.cpp \
    src/models/systemstatusviewmodel.cpp \
    src/models/trpparameterviewmodel.cpp \
    src/models/windageviewmodel.cpp \
    src/models/zeroingviewmodel.cpp \
    src/models/zonedefinitionviewmodel.cpp \
    src/models/zonemapviewmodel.cpp \
    src/services/servicemanager.cpp \
    src/services/zonegeometryservice.cpp \
    src/utils/ballisticsprocessor.cpp \
    src/utils/colorutils.cpp \
    src/utils/inference.cpp \
    src/utils/reticleaimpointcalculator.cpp \
    src/video/gstvideosource.cpp \
    src/video/videoimageprovider.cpp \
    src/hardware/communication/modbustransport.cpp \
    src/hardware/communication/serialporttransport.cpp \
    src/hardware/protocols/DayCameraProtocolParser.cpp \
    src/hardware/protocols/ImuProtocolParser.cpp \
    src/hardware/protocols/JoystickProtocolParser.cpp \
    src/hardware/protocols/LrfProtocolParser.cpp \
    src/hardware/protocols/NightCameraProtocolParser.cpp \
    src/hardware/protocols/Plc21ProtocolParser.cpp \
    src/hardware/protocols/Plc42ProtocolParser.cpp \
    src/hardware/protocols/RadarProtocolParser.cpp \
    src/hardware/protocols/ServoActuatorProtocolParser.cpp \
    src/hardware/protocols/ServoDriverProtocolParser.cpp

RESOURCES += resources/resources.qrc

#resources.files = main.qml
#resources.prefix = /$${TARGET}
#RESOURCES += resources \
#    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/controllers/aboutcontroller.h \
    src/controllers/applicationcontroller.h \
    src/controllers/cameracontroller.h \
    src/controllers/colormenucontroller.h \
    src/controllers/deviceconfiguration.h \
    src/controllers/gimbalcontroller.h \
    src/controllers/joystickcontroller.h \
    src/controllers/mainmenucontroller.h \
    src/controllers/motion_modes/autosectorscanmotionmode.h \
    src/controllers/motion_modes/gimbalmotionmodebase.h \
    src/controllers/motion_modes/manualmotionmode.h \
    src/controllers/motion_modes/pidcontroller.h \
    src/controllers/motion_modes/radarslewmotionmode.h \
    src/controllers/motion_modes/trackingmotionmode.h \
    src/controllers/motion_modes/trpscanmotionmode.h \
    src/controllers/osdcontroller.h \
    src/controllers/reticlemenucontroller.h \
    src/controllers/systemcontroller.h \
    src/controllers/systemstatuscontroller.h \
    src/controllers/weaponcontroller.h \
    src/controllers/windagecontroller.h \
    src/controllers/zeroingcontroller.h \
    src/controllers/zonedefinitioncontroller.h \
    src/managers/HardwareManager.h \
    src/managers/ViewModelRegistry.h \
    src/managers/ControllerRegistry.h \
    src/config/AppConstants.h \
    src/config/ConfigurationValidator.h \
    src/hardware/devices/cameravideostreamdevice.h \
    src/hardware/devices/daycameracontroldevice.h \
    src/hardware/devices/imudevice.h \
    src/hardware/devices/joystickdevice.h \
    src/hardware/devices/lrfdevice.h \
    src/hardware/devices/nightcameracontroldevice.h \
    src/hardware/devices/plc21device.h \
    src/hardware/devices/plc42device.h \
    src/hardware/devices/radardevice.h \
    src/hardware/devices/servoactuatordevice.h \
    src/hardware/devices/servodriverdevice.h \
    src/hardware/devices/vpi_helpers.h \
    src/logger/systemdatalogger.h \
    src/models/aboutviewmodel.h \
    src/models/areazoneparameterviewmodel.h \
    src/models/domain/daycameradatamodel.h \
    src/models/domain/gyrodatamodel.h \
    src/models/domain/joystickdatamodel.h \
    src/models/domain/lrfdatamodel.h \
    src/models/domain/nightcameradatamodel.h \
    src/models/domain/plc21datamodel.h \
    src/models/domain/plc42datamodel.h \
    src/models/domain/radardatamodel.h \
    src/models/domain/servoactuatordatamodel.h \
    src/models/domain/servodriverdatamodel.h \
    src/models/domain/systemstatedata.h \
    src/models/domain/systemstatemodel.h \
    src/models/historyviewmodel.h \
    src/models/menuviewmodel.h \
    src/models/osdviewmodel.h \
    src/models/sectorscanparameterviewmodel.h \
    src/models/systemstatusviewmodel.h \
    src/models/trpparameterviewmodel.h \
    src/models/windageviewmodel.h \
    src/models/zeroingviewmodel.h \
    src/models/zonedefinitionviewmodel.h \
    src/models/zonemapviewmodel.h \
    src/services/servicemanager.h \
    src/services/zonegeometryservice.h \
    src/utils/TimestampLogger.h \
    src/utils/ballisticsprocessor.h \
    src/utils/colorutils.h \
    src/utils/inference.h \
    src/utils/millenious.h \
    src/utils/reticleaimpointcalculator.h \
    src/utils/targetstate.h \
    src/video/gstvideosource.h \
    src/video/videoimageprovider.h \
    src/hardware/interfaces/IDevice.h \
    src/hardware/interfaces/Transport.h \
    src/hardware/interfaces/ProtocolParser.h \
    src/hardware/interfaces/Message.h \
    src/hardware/data/DataTypes.h \
    src/hardware/devices/TemplatedDevice.h \
    src/hardware/communication/modbustransport.h \
    src/hardware/communication/serialporttransport.h \
    src/hardware/protocols/DayCameraProtocolParser.h \
    src/hardware/protocols/ImuProtocolParser.h \
    src/hardware/protocols/JoystickProtocolParser.h \
    src/hardware/protocols/LrfProtocolParser.h \
    src/hardware/protocols/NightCameraProtocolParser.h \
    src/hardware/protocols/Plc21ProtocolParser.h \
    src/hardware/protocols/Plc42ProtocolParser.h \
    src/hardware/protocols/RadarProtocolParser.h \
    src/hardware/protocols/ServoActuatorProtocolParser.h \
    src/hardware/protocols/ServoDriverProtocolParser.h \
    src/hardware/messages/DayCameraMessage.h \
    src/hardware/messages/ImuMessage.h \
    src/hardware/messages/JoystickMessage.h \
    src/hardware/messages/LrfMessage.h \
    src/hardware/messages/NightCameraMessage.h \
    src/hardware/messages/Plc21Message.h \
    src/hardware/messages/Plc42Message.h \
    src/hardware/messages/RadarMessage.h \
    src/hardware/messages/ServoActuatorMessage.h \
    src/hardware/messages/ServoDriverMessage.h



