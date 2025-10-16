QT += quick serialbus serialport dbus

#LIBS += -L/usr/lib/x86_64-linux-gnu/gstreamer-1.0 -lgstxvimagesink
INCLUDEPATH += "/usr/include/gstreamer-1.0"
INCLUDEPATH += src

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0
PKGCONFIG += gstreamer-video-1.0


LIBS += -lgstreamer-1.0 -lgstapp-1.0 -lgstbase-1.0 -lgobject-2.0 -lglib-2.0
PKGCONFIG += gstreamer-gl-1.0



SOURCES += \
        src/controllers/applicationcontroller.cpp \
        src/controllers/colormenucontroller.cpp \
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
        src/utils/colorutils.cpp


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
    src/controllers/applicationcontroller.h \
    src/controllers/colormenucontroller.h \
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
    src/utils/colorutils.h




