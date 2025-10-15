#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "video/videoimageprovider.h"
#include "video/gstvideosource.h"
#include "models/osdviewmodel.h"
#include <QTimer>
#include "controllers/applicationcontroller.h"
#include "models/menuviewmodel.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "services/servicemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    gst_init(&argc, &argv);

    QQmlApplicationEngine engine;
    ServiceManager* services = ServiceManager::instance();

    // === PHASE 1: CREATE ALL SERVICES ===
    OsdViewModel* osdViewModel = new OsdViewModel(1024, 768);
    MenuViewModel* mainMenuViewModel = new MenuViewModel();
    MainMenuController* mainMenuController = new MainMenuController();
    ReticleMenuController* reticleMenuController = new ReticleMenuController();
    ColorMenuController* colorMenuController = new ColorMenuController();
    ApplicationController* appController = new ApplicationController();

    // === PHASE 2: REGISTER SERVICES ===
    services->registerService(OsdViewModel::staticMetaObject.className(), osdViewModel);
    services->registerService(MenuViewModel::staticMetaObject.className(), mainMenuViewModel);
    services->registerService(MainMenuController::staticMetaObject.className(), mainMenuController);
    services->registerService(ReticleMenuController::staticMetaObject.className(), reticleMenuController);
    services->registerService(ColorMenuController::staticMetaObject.className(), colorMenuController);
    services->registerService(ApplicationController::staticMetaObject.className(), appController);

    // === PHASE 3: INITIALIZE SERVICES ===
    mainMenuController->initialize();
    reticleMenuController->initialize();
    colorMenuController->initialize();
    appController->initialize();

    // === PHASE 4: EXPOSE VIEWMODELS TO QML ===
    engine.rootContext()->setContextProperty("osdViewModelInstance", osdViewModel);
    engine.rootContext()->setContextProperty("mainMenuViewModel", mainMenuViewModel);
    engine.rootContext()->setContextProperty("appController", appController);

    // --- Video Feed Backend ---
    VideoImageProvider videoProvider;
    GstVideoSource videoSource(&videoProvider);
    engine.addImageProvider("video", &videoProvider);

    // --- Load QML UI ---
    engine.load(QUrl(QStringLiteral("qrc:/qml/views/main.qml")));
    videoSource.startPipeline();

    // --- Test button simulation (optional) ---
    QTimer buttonSimulator;
    QObject::connect(&buttonSimulator, &QTimer::timeout, [&](){
        static bool menuOpened = false;
        if (!menuOpened) {
            appController->onMenuButtonPressed();
            menuOpened = true;
        }
    });
    // buttonSimulator.start(2000); // Uncomment to auto-open menu after 2 seconds

    return app.exec();
}
