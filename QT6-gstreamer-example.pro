QT += quick

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
        src/video/gstvideosource.cpp \
        src/main.cpp \
        src/controllers/mainmenucontroller.cpp \
        src/models/menuviewmodel.cpp \
        src/models/osdviewmodel.cpp \
        src/controllers/reticlemenucontroller.cpp \
        src/services/servicemanager.cpp \
        src/video/videoimageprovider.cpp

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
    src/video/gstvideosource.h \
    src/controllers/mainmenucontroller.h \
    src/models/menuviewmodel.h \
    src/models/osdviewmodel.h \
    src/controllers/reticlemenucontroller.h \
    src/services/servicemanager.h \
    src/video/videoimageprovider.h
