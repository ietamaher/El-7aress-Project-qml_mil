#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "services/servicemanager.h"
#include "models/domain/systemstatemodel.h"
#include <QDebug>

ApplicationController::ApplicationController(QObject *parent)
    : QObject(parent)
    , m_currentMenuState(MenuState::None)
    , m_mainMenuController(nullptr)
    , m_reticleMenuController(nullptr)
    , m_colorMenuController(nullptr)
    , m_zeroingController(nullptr)
    , m_windageController(nullptr)
{
}

void ApplicationController::initialize()
{
    // Get controllers from ServiceManager
    m_mainMenuController = ServiceManager::instance()->get<MainMenuController>();
    Q_ASSERT(m_mainMenuController);

    m_reticleMenuController = ServiceManager::instance()->get<ReticleMenuController>();
    m_colorMenuController = ServiceManager::instance()->get<ColorMenuController>();
    m_zeroingController = ServiceManager::instance()->get<ZeroingController>();
    m_windageController = ServiceManager::instance()->get<WindageController>();

    // If not registered, create them here:
    if (!m_reticleMenuController) {
        m_reticleMenuController = new ReticleMenuController(this);
        m_reticleMenuController->initialize();
    }

    if (!m_colorMenuController) {
        m_colorMenuController = new ColorMenuController(this);
        m_colorMenuController->initialize();
    }

    if (!m_zeroingController) {
        m_zeroingController = new ZeroingController(this);
        m_zeroingController->initialize();
    }

    if (!m_windageController) {
        m_windageController = new WindageController(this);
        m_windageController->initialize();
    }

    // Connect main menu signals
    connect(m_mainMenuController, &MainMenuController::personalizeReticleRequested,
            this, &ApplicationController::handlePersonalizeReticle);
    connect(m_mainMenuController, &MainMenuController::personalizeColorsRequested,
            this, &ApplicationController::handlePersonalizeColors);
    connect(m_mainMenuController, &MainMenuController::adjustBrightnessRequested,
            this, &ApplicationController::handleAdjustBrightness);
    connect(m_mainMenuController, &MainMenuController::zeroingRequested,
            this, &ApplicationController::handleZeroing);
    connect(m_mainMenuController, &MainMenuController::clearZeroRequested,
            this, &ApplicationController::handleClearZero);
    connect(m_mainMenuController, &MainMenuController::windageRequested,
            this, &ApplicationController::handleWindage);
    connect(m_mainMenuController, &MainMenuController::clearWindageRequested,
            this, &ApplicationController::handleClearWindage);
    connect(m_mainMenuController, &MainMenuController::zoneDefinitionsRequested,
            this, &ApplicationController::handleZoneDefinitions);
    connect(m_mainMenuController, &MainMenuController::systemStatusRequested,
            this, &ApplicationController::handleSystemStatus);
    connect(m_mainMenuController, &MainMenuController::radarTargetListRequested,
            this, &ApplicationController::handleRadarTargetList);
    connect(m_mainMenuController, &MainMenuController::helpAboutRequested,
            this, &ApplicationController::handleHelpAbout);

    // Connect submenu signals
    connect(m_reticleMenuController, &ReticleMenuController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_reticleMenuController, &ReticleMenuController::menuFinished,
            this, &ApplicationController::handleReticleMenuFinished);

    connect(m_colorMenuController, &ColorMenuController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_colorMenuController, &ColorMenuController::menuFinished,
            this, &ApplicationController::handleColorMenuFinished);

    // Connect zeroing signals
    connect(m_zeroingController, &ZeroingController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_zeroingController, &ZeroingController::zeroingFinished,
            this, &ApplicationController::handleZeroingFinished);

    // Connect windage signals
    connect(m_windageController, &WindageController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_windageController, &WindageController::windageFinished,
            this, &ApplicationController::handleWindageFinished);
}

void ApplicationController::setMenuState(MenuState state)
{
    m_currentMenuState = state;
    qDebug() << "ApplicationController: Menu state changed to" << static_cast<int>(state);
}

void ApplicationController::showMainMenu()
{
    hideAllMenus();
    m_mainMenuController->show();
    setMenuState(MenuState::MainMenu);
}

void ApplicationController::hideAllMenus()
{
    m_mainMenuController->hide();
    m_reticleMenuController->hide();
    m_colorMenuController->hide();
    m_zeroingController->hide();
    m_windageController->hide();
}

// =============================================================================
// MENU/VAL BUTTON LOGIC - The main button that does everything
// =============================================================================

void ApplicationController::onMenuValButtonPressed()
{
    qDebug() << "ApplicationController: MENU/VAL button pressed in state"
             << static_cast<int>(m_currentMenuState);

    switch (m_currentMenuState) {
    case MenuState::None:
        handleMenuValInNoMenuState();
        break;

    case MenuState::MainMenu:
        handleMenuValInMainMenu();
        break;

    case MenuState::ReticleMenu:
    case MenuState::ColorMenu:
        handleMenuValInSubmenu();
        break;

    case MenuState::ZeroingProcedure:
    case MenuState::WindageProcedure:
        handleMenuValInProcedure();
        break;

    default:
        // For future states (brightness, zones, etc.)
        // Default to closing and returning to main menu
        hideAllMenus();
        showMainMenu();
        break;
    }
}

void ApplicationController::handleMenuValInNoMenuState()
{
    // No menu open - MENU/VAL opens the main menu
    qDebug() << "ApplicationController: Opening main menu";
    showMainMenu();
}

void ApplicationController::handleMenuValInMainMenu()
{
    // In main menu - MENU/VAL acts as SELECT to choose highlighted option
    qDebug() << "ApplicationController: Selecting main menu item";
    m_mainMenuController->onSelectButtonPressed();
}

void ApplicationController::handleMenuValInSubmenu()
{
    // In submenu (Reticle/Color) - MENU/VAL acts as SELECT to confirm choice
    qDebug() << "ApplicationController: Selecting submenu item";

    switch (m_currentMenuState) {
    case MenuState::ReticleMenu:
        m_reticleMenuController->onSelectButtonPressed();
        break;
    case MenuState::ColorMenu:
        m_colorMenuController->onSelectButtonPressed();
        break;
    default:
        break;
    }
}

void ApplicationController::handleMenuValInProcedure()
{
    // In procedure (Zeroing/Windage) - MENU/VAL acts as SELECT to confirm step
    qDebug() << "ApplicationController: Confirming procedure step";

    switch (m_currentMenuState) {
    case MenuState::ZeroingProcedure:
        m_zeroingController->onSelectButtonPressed();
        break;
    case MenuState::WindageProcedure:
        m_windageController->onSelectButtonPressed();
        break;
    default:
        break;
    }
}

// =============================================================================
// UP/DOWN BUTTON LOGIC - Navigate through options
// =============================================================================

void ApplicationController::onUpButtonPressed()
{
    qDebug() << "ApplicationController: UP button pressed";

    switch (m_currentMenuState) {
    case MenuState::MainMenu:
        m_mainMenuController->onUpButtonPressed();
        break;
    case MenuState::ReticleMenu:
        m_reticleMenuController->onUpButtonPressed();
        break;
    case MenuState::ColorMenu:
        m_colorMenuController->onUpButtonPressed();
        break;
    case MenuState::ZeroingProcedure:
        m_zeroingController->onUpButtonPressed();
        break;
    case MenuState::WindageProcedure:
        m_windageController->onUpButtonPressed();
        break;
    default:
        // No menu open - UP might control other things (brightness, zoom, etc.)
        qDebug() << "ApplicationController: UP pressed with no active menu";
        break;
    }
}

void ApplicationController::onDownButtonPressed()
{
    qDebug() << "ApplicationController: DOWN button pressed";

    switch (m_currentMenuState) {
    case MenuState::MainMenu:
        m_mainMenuController->onDownButtonPressed();
        break;
    case MenuState::ReticleMenu:
        m_reticleMenuController->onDownButtonPressed();
        break;
    case MenuState::ColorMenu:
        m_colorMenuController->onDownButtonPressed();
        break;
    case MenuState::ZeroingProcedure:
        m_zeroingController->onDownButtonPressed();
        break;
    case MenuState::WindageProcedure:
        m_windageController->onDownButtonPressed();
        break;
    default:
        // No menu open - DOWN might control other things
        qDebug() << "ApplicationController: DOWN pressed with no active menu";
        break;
    }
}

// =============================================================================
// MAIN MENU ACTION HANDLERS
// =============================================================================

void ApplicationController::handlePersonalizeReticle()
{
    qDebug() << "ApplicationController: Showing Reticle Menu";
    hideAllMenus();
    m_reticleMenuController->show();
    setMenuState(MenuState::ReticleMenu);
}

void ApplicationController::handlePersonalizeColors()
{
    qDebug() << "ApplicationController: Showing Color Menu";
    hideAllMenus();
    m_colorMenuController->show();
    setMenuState(MenuState::ColorMenu);
}

void ApplicationController::handleAdjustBrightness()
{
    qDebug() << "ApplicationController: Adjust Brightness requested";
    hideAllMenus();
    setMenuState(MenuState::BrightnessAdjust);

    // TODO: Show brightness adjustment widget/dialog
    // For now, just return to main menu
    showMainMenu();
}

void ApplicationController::handleZeroing()
{
    qDebug() << "ApplicationController: Zeroing requested";
    hideAllMenus();
    m_zeroingController->show();
    setMenuState(MenuState::ZeroingProcedure);
}

void ApplicationController::handleClearZero()
{
    qDebug() << "ApplicationController: Clear Zero requested";
    SystemStateModel* stateModel = ServiceManager::instance()->get<SystemStateModel>();
    if (stateModel) {
        stateModel->clearZeroing();
    }
    showMainMenu();
}

void ApplicationController::handleWindage()
{
    qDebug() << "ApplicationController: Windage requested";
    hideAllMenus();
    m_windageController->show();
    setMenuState(MenuState::WindageProcedure);
}

void ApplicationController::handleClearWindage()
{
    qDebug() << "ApplicationController: Clear Windage requested";
    SystemStateModel* stateModel = ServiceManager::instance()->get<SystemStateModel>();
    if (stateModel) {
        stateModel->clearWindage();
    }
    showMainMenu();
}

void ApplicationController::handleZoneDefinitions()
{
    qDebug() << "ApplicationController: Zone Definitions requested";
    hideAllMenus();
    setMenuState(MenuState::ZoneDefinition);

    // TODO: Show zone definition widget
    showMainMenu();
}

void ApplicationController::handleSystemStatus()
{
    qDebug() << "ApplicationController: System Status requested";
    hideAllMenus();
    setMenuState(MenuState::SystemStatus);

    // TODO: Show system status widget
    showMainMenu();
}

void ApplicationController::handleRadarTargetList()
{
    qDebug() << "ApplicationController: Radar Target List requested";
    hideAllMenus();
    setMenuState(MenuState::RadarTargets);

    // TODO: Show radar targets widget
    showMainMenu();
}

void ApplicationController::handleHelpAbout()
{
    qDebug() << "ApplicationController: Help/About requested";
    hideAllMenus();
    setMenuState(MenuState::HelpAbout);

    // TODO: Show help/about dialog
    showMainMenu();
}

// =============================================================================
// SUBMENU AND PROCEDURE COMPLETION HANDLERS
// =============================================================================

void ApplicationController::handleReticleMenuFinished()
{
    qDebug() << "ApplicationController: Reticle menu finished";
    // Menu controller already hid itself and applied selection
}

void ApplicationController::handleColorMenuFinished()
{
    qDebug() << "ApplicationController: Color menu finished";
    // Menu controller already hid itself and applied selection
}

void ApplicationController::handleZeroingFinished()
{
    qDebug() << "ApplicationController: Zeroing procedure finished";
    // Zeroing controller already hid itself
}

void ApplicationController::handleWindageFinished()
{
    qDebug() << "ApplicationController: Windage procedure finished";
    // Windage controller already hid itself
}

void ApplicationController::handleReturnToMainMenu()
{
    qDebug() << "ApplicationController: Returning to main menu";
    hideAllMenus();
    showMainMenu();
}

