#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "video/videoimageprovider.h"
#include "video/gstvideosource.h"
#include "models/osdviewmodel.h"
#include <QTimer>
#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "models/menuviewmodel.h"
#include "models/zeroingviewmodel.h"
#include "models/windageviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "services/servicemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    gst_init(&argc, &argv);

    QQmlApplicationEngine engine;
    ServiceManager* services = ServiceManager::instance();

    // === PHASE 1: CREATE ALL SERVICES ===
    OsdViewModel* osdViewModel = new OsdViewModel(1024, 768);
    SystemStateModel* stateModel = new SystemStateModel(); // Your existing state model

    // Create separate MenuViewModel instances for each menu
    MenuViewModel* mainMenuViewModel = new MenuViewModel();
    MenuViewModel* reticleMenuViewModel = new MenuViewModel();
    MenuViewModel* colorMenuViewModel = new MenuViewModel();

    // Create ViewModels for procedures
    ZeroingViewModel* zeroingViewModel = new ZeroingViewModel();
    WindageViewModel* windageViewModel = new WindageViewModel();

    // Create controllers
    MainMenuController* mainMenuController = new MainMenuController();
    ReticleMenuController* reticleMenuController = new ReticleMenuController();
    ColorMenuController* colorMenuController = new ColorMenuController();
    ZeroingController* zeroingController = new ZeroingController();
    WindageController* windageController = new WindageController();
    ApplicationController* appController = new ApplicationController();

    // === PHASE 2: REGISTER SERVICES ===
    services->registerService(OsdViewModel::staticMetaObject.className(), osdViewModel);
    services->registerService(SystemStateModel::staticMetaObject.className(), stateModel);

    // Register with unique names to distinguish them
    services->registerService("MainMenuViewModel", mainMenuViewModel);
    services->registerService("ReticleMenuViewModel", reticleMenuViewModel);
    services->registerService("ColorMenuViewModel", colorMenuViewModel);
    services->registerService("ZeroingViewModel", zeroingViewModel);
    services->registerService("WindageViewModel", windageViewModel);

    services->registerService(MainMenuController::staticMetaObject.className(), mainMenuController);
    services->registerService(ReticleMenuController::staticMetaObject.className(), reticleMenuController);
    services->registerService(ColorMenuController::staticMetaObject.className(), colorMenuController);
    services->registerService(ZeroingController::staticMetaObject.className(), zeroingController);
    services->registerService(WindageController::staticMetaObject.className(), windageController);
    services->registerService(ApplicationController::staticMetaObject.className(), appController);

    // === PHASE 3: INITIALIZE SERVICES ===
    mainMenuController->initialize();
    reticleMenuController->initialize();
    colorMenuController->initialize();
    zeroingController->initialize();
    windageController->initialize();
    appController->initialize();

    // === PHASE 4: EXPOSE VIEWMODELS TO QML ===
    engine.rootContext()->setContextProperty("osdViewModelInstance", osdViewModel);
    engine.rootContext()->setContextProperty("mainMenuViewModel", mainMenuViewModel);
    engine.rootContext()->setContextProperty("reticleMenuViewModel", reticleMenuViewModel);
    engine.rootContext()->setContextProperty("colorMenuViewModel", colorMenuViewModel);
    engine.rootContext()->setContextProperty("zeroingViewModel", zeroingViewModel);
    engine.rootContext()->setContextProperty("windageViewModel", windageViewModel);
    engine.rootContext()->setContextProperty("appController", appController);

    // --- Video Feed Backend ---
    VideoImageProvider videoProvider;
    GstVideoSource videoSource(&videoProvider);
    engine.addImageProvider("video", &videoProvider);

    // --- Load QML UI ---
    engine.load(QUrl(QStringLiteral("qrc:/qml/views/main.qml")));
    videoSource.startPipeline();

    return app.exec();
}
