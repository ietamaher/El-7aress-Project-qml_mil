QT += quick

#LIBS += -L/usr/lib/x86_64-linux-gnu/gstreamer-1.0 -lgstxvimagesink
INCLUDEPATH += "/usr/include/gstreamer-1.0"

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0
PKGCONFIG += gstreamer-video-1.0


LIBS += -lgstreamer-1.0 -lgstapp-1.0 -lgstbase-1.0 -lgobject-2.0 -lglib-2.0
PKGCONFIG += gstreamer-gl-1.0



SOURCES += \
        applicationcontroller.cpp \
        colormenucontroller.cpp \
        gstvideosource.cpp \
        main.cpp \
        mainmenucontroller.cpp \
        menuviewmodel.cpp \
        osdviewmodel.cpp \
        reticlemenucontroller.cpp \
        servicemanager.cpp \
        videoimageprovider.cpp

RESOURCES += resources.qrc

#resources.files = main.qml
#resources.prefix = /$${TARGET}
#RESOURCES += resources \
#    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    applicationcontroller.h \
    colormenucontroller.h \
    gstvideosource.h \
    mainmenucontroller.h \
    menuviewmodel.h \
    osdviewmodel.h \
    reticlemenucontroller.h \
    servicemanager.h \
    videoimageprovider.h
