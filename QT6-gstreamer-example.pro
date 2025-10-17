QT += quick serialbus serialport dbus

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


SOURCES += \
        src/controllers/cameracontroller.cpp \
        src/controllers/gimbalcontroller.cpp \
        src/controllers/joystickcontroller.cpp \
        src/controllers/motion_modes/autosectorscanmotionmode.cpp \
        src/controllers/motion_modes/gimbalmotionmodebase.cpp \
        src/controllers/motion_modes/manualmotionmode.cpp \
        src/controllers/motion_modes/radarslewmotionmode.cpp \
        src/controllers/motion_modes/trackingmotionmode.cpp \
        src/controllers/motion_modes/trpscanmotionmode.cpp \
        src/controllers/weaponcontroller.cpp \
        src/controllers/applicationcontroller.cpp \
        src/controllers/colormenucontroller.cpp \
        src/controllers/osdcontroller.cpp \
        src/controllers/systemcontroller.cpp \
        src/controllers/windagecontroller.cpp \
        src/controllers/zeroingcontroller.cpp \
        src/controllers/zonedefinitioncontroller.cpp \
        src/models/areazoneparameterviewmodel.cpp \
        src/models/sectorscanparameterviewmodel.cpp \
        src/models/trpparameterviewmodel.cpp \
        src/models/windageviewmodel.cpp \
        src/models/zeroingviewmodel.cpp \
        src/models/domain/systemstatemodel.cpp \
        src/models/zonedefinitionviewmodel.cpp \
        src/models/zonemapviewmodel.cpp \
        src/services/zonegeometryservice.cpp \
        src/video/gstvideosource.cpp \
        src/main.cpp \
        src/controllers/mainmenucontroller.cpp \
        src/models/menuviewmodel.cpp \
        src/models/osdviewmodel.cpp \
        src/controllers/reticlemenucontroller.cpp \
        src/services/servicemanager.cpp \
        src/video/videoimageprovider.cpp \
        src/utils/reticleaimpointcalculator.cpp \
        src/utils/colorutils.cpp \
        src/hardware/devices/baseserialdevice.cpp \
        src/hardware/devices/imudevice.cpp \
        src/hardware/devices/modbusdevicebase.cpp \
        src/hardware/devices/radardevice.cpp \
        src/hardware/devices/cameravideostreamdevice.cpp \
        src/hardware/devices/servoactuatordevice.cpp \
        src/hardware/devices/plc42device.cpp \
        src/hardware/devices/plc21device.cpp \
        src/hardware/devices/nightcameracontroldevice.cpp \
        src/hardware/devices/daycameracontroldevice.cpp \
        src/hardware/devices/lrfdevice.cpp \
        src/hardware/devices/joystickdevice.cpp \
        src/hardware/devices/lensdevice.cpp \
        src/hardware/devices/servodriverdevice.cpp \

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
    src/controllers/cameracontroller.h \
    src/controllers/gimbalcontroller.h \
    src/controllers/joystickcontroller.h \
    src/controllers/motion_modes/autosectorscanmotionmode.h \
    src/controllers/motion_modes/gimbalmotionmodebase.h \
    src/controllers/motion_modes/manualmotionmode.h \
    src/controllers/motion_modes/radarslewmotionmode.h \
    src/controllers/motion_modes/trackingmotionmode.h \
    src/controllers/motion_modes/trpscanmotionmode.h \
    src/controllers/weaponcontroller.h \
    src/controllers/applicationcontroller.h \
    src/controllers/colormenucontroller.h \
    src/controllers/osdcontroller.h \
    src/controllers/systemcontroller.h \
    src/controllers/windagecontroller.h \
    src/controllers/zeroingcontroller.h \
    src/controllers/zonedefinitioncontroller.h \
    src/models/areazoneparameterviewmodel.h \
    src/models/sectorscanparameterviewmodel.h \
    src/models/systemstatedata.h \
    src/models/trpparameterviewmodel.h \
    src/models/windageviewmodel.h \
    src/models/zeroingviewmodel.h \
    src/models/domain/systemstatemodel.h \
    src/models/domain/systemstatedata.h \
    src/models/zonedefinitionviewmodel.h \
    src/models/zonemapviewmodel.h \
    src/services/zonegeometryservice.h \
    src/video/gstvideosource.h \
    src/controllers/mainmenucontroller.h \
    src/models/menuviewmodel.h \
    src/models/osdviewmodel.h \
    src/controllers/reticlemenucontroller.h \
    src/services/servicemanager.h \
    src/video/videoimageprovider.h \
    src/utils/reticleaimpointcalculator.h \
    src/utils/colorutils.h \
    src/hardware/devices/baseserialdevice.h \
        src/hardware/devices/imudevice.h \
        src/hardware/devices/modbusdevicebase.h \
        src/hardware/devices/radardevice.h \
        src/hardware/devices/cameravideostreamdevice.h \
        src/hardware/devices/servoactuatordevice.h \
        src/hardware/devices/plc42device.h \
        src/hardware/devices/plc21device.h \
        src/hardware/devices/nightcameracontroldevice.h \
        src/hardware/devices/daycameracontroldevice.h \
        src/hardware/devices/lrfdevice.h \
        src/hardware/devices/joystickdevice.h \
        src/hardware/devices/lensdevice.h \
        src/hardware/devices/servodriverdevice.h \


