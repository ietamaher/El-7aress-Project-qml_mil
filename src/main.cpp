#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "controllers/systemcontroller.h"
#include "controllers/deviceconfiguration.h"
#include <gst/gst.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    gst_init(&argc, &argv);

    // ========================================================================
    // LOAD DEVICE CONFIGURATION
    // ========================================================================
    if (!DeviceConfiguration::load("./config/devices.json")) {
        qCritical() << "Failed to load device configuration!";
        return -1;
    }
    // ========================================================================
    // PHASE 1: Initialize Hardware
    // ========================================================================
    SystemController sysCtrl;
    sysCtrl.initializeHardware();

    // ========================================================================
    // PHASE 2: Initialize QML System
    // ========================================================================
    QQmlApplicationEngine engine;
    sysCtrl.initializeQmlSystem(&engine);

    // ========================================================================
    // PHASE 3: Load QML UI
    // ========================================================================
    engine.load(QUrl(QStringLiteral("qrc:/qml/views/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML!";
        return -1;
    }

    // ========================================================================
    // PHASE 4: Start System (devices, threads, video)
    // ========================================================================
    sysCtrl.startSystem();

    return app.exec();
}
