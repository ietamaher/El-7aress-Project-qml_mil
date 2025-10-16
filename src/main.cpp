#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "video/videoimageprovider.h"
#include "video/gstvideosource.h"
#include "models/osdviewmodel.h"
#include <QTimer>
#include "controllers/applicationcontroller.h"
#include "controllers/osdcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "controllers/zonedefinitioncontroller.h"
#include "models/menuviewmodel.h"
#include "models/zeroingviewmodel.h"
#include "models/windageviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "services/servicemanager.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "models/areazoneparameterviewmodel.h"
#include "models/sectorscanparameterviewmodel.h"
#include "models/trpparameterviewmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    gst_init(&argc, &argv);

    QQmlApplicationEngine engine;
    ServiceManager* services = ServiceManager::instance();

    // === PHASE 1: CREATE ALL SERVICES ===
    OsdViewModel* osdViewModel = new OsdViewModel();
    SystemStateModel* stateModel = new SystemStateModel(); // Your existing state model

    // Create separate MenuViewModel instances for each menu
    MenuViewModel* mainMenuViewModel = new MenuViewModel();
    MenuViewModel* reticleMenuViewModel = new MenuViewModel();
    MenuViewModel* colorMenuViewModel = new MenuViewModel();

    // Create ViewModels for procedures
    ZeroingViewModel* zeroingViewModel = new ZeroingViewModel();
    WindageViewModel* windageViewModel = new WindageViewModel();

    // Create Zone Definition ViewModels ***
    ZoneDefinitionViewModel* zoneDefinitionViewModel = new ZoneDefinitionViewModel();
    ZoneMapViewModel* zoneMapViewModel = new ZoneMapViewModel();
    AreaZoneParameterViewModel* areaZoneParameterViewModel = new AreaZoneParameterViewModel();
    SectorScanParameterViewModel* sectorScanParameterViewModel = new SectorScanParameterViewModel();
    TRPParameterViewModel* trpParameterViewModel = new TRPParameterViewModel();

    // Create controllers
    OsdController* osdController = new OsdController();
    MainMenuController* mainMenuController = new MainMenuController();
    ReticleMenuController* reticleMenuController = new ReticleMenuController();
    ColorMenuController* colorMenuController = new ColorMenuController();
    ZeroingController* zeroingController = new ZeroingController();
    WindageController* windageController = new WindageController();
    ZoneDefinitionController* zoneDefinitionController = new ZoneDefinitionController();

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

    // Register Zone Definition ViewModels ***
    services->registerService("ZoneDefinitionViewModel", zoneDefinitionViewModel);
    services->registerService("ZoneMapViewModel", zoneMapViewModel);
    services->registerService("AreaZoneParameterViewModel", areaZoneParameterViewModel);
    services->registerService("SectorScanParameterViewModel", sectorScanParameterViewModel);
    services->registerService("TRPParameterViewModel", trpParameterViewModel);

    services->registerService(OsdController::staticMetaObject.className(), osdController);
    services->registerService(MainMenuController::staticMetaObject.className(), mainMenuController);
    services->registerService(ReticleMenuController::staticMetaObject.className(), reticleMenuController);
    services->registerService(ColorMenuController::staticMetaObject.className(), colorMenuController);
    services->registerService(ZeroingController::staticMetaObject.className(), zeroingController);
    services->registerService(WindageController::staticMetaObject.className(), windageController);
    services->registerService(ZoneDefinitionController::staticMetaObject.className(), zoneDefinitionController);

    services->registerService(ApplicationController::staticMetaObject.className(), appController);

    // === PHASE 3: INITIALIZE SERVICES ===
    osdController->initialize();
    mainMenuController->initialize();
    reticleMenuController->initialize();
    colorMenuController->initialize();
    zeroingController->initialize();
    windageController->initialize();
    zoneDefinitionController->initialize();
    appController->initialize();

    // === PHASE 4: EXPOSE VIEWMODELS TO QML ===
    engine.rootContext()->setContextProperty("osdViewModel", osdViewModel);
    engine.rootContext()->setContextProperty("mainMenuViewModel", mainMenuViewModel);
    engine.rootContext()->setContextProperty("reticleMenuViewModel", reticleMenuViewModel);
    engine.rootContext()->setContextProperty("colorMenuViewModel", colorMenuViewModel);
    engine.rootContext()->setContextProperty("zeroingViewModel", zeroingViewModel);
    engine.rootContext()->setContextProperty("windageViewModel", windageViewModel);

    // Expose Zone Definition ViewModels ***
    engine.rootContext()->setContextProperty("zoneDefinitionViewModel", zoneDefinitionViewModel);
    engine.rootContext()->setContextProperty("zoneMapViewModel", zoneMapViewModel);
    engine.rootContext()->setContextProperty("areaZoneParameterViewModel", areaZoneParameterViewModel);
    engine.rootContext()->setContextProperty("sectorScanParameterViewModel", sectorScanParameterViewModel);
    engine.rootContext()->setContextProperty("trpParameterViewModel", trpParameterViewModel);


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
