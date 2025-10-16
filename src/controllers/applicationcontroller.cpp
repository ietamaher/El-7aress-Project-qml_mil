#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "controllers/zonedefinitioncontroller.h"
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
    , m_zoneDefinitionController(nullptr)
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
    m_zoneDefinitionController = ServiceManager::instance()->get<ZoneDefinitionController>();

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
    if (!m_zoneDefinitionController) {
        m_zoneDefinitionController = new ZoneDefinitionController(this);
        m_zoneDefinitionController->initialize();
    }
    // =========================================================================
    // MAIN MENU CONNECTIONS
    // =========================================================================
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

    // THIS IS CRITICAL - Handle "Return ..." from main menu
    connect(m_mainMenuController, &MainMenuController::menuFinished,
            this, &ApplicationController::handleMainMenuFinished);

    // =========================================================================
    // RETICLE MENU CONNECTIONS
    // =========================================================================
    connect(m_reticleMenuController, &ReticleMenuController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_reticleMenuController, &ReticleMenuController::menuFinished,
            this, &ApplicationController::handleReticleMenuFinished);

    // =========================================================================
    // COLOR MENU CONNECTIONS
    // =========================================================================
    connect(m_colorMenuController, &ColorMenuController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_colorMenuController, &ColorMenuController::menuFinished,
            this, &ApplicationController::handleColorMenuFinished);

    // =========================================================================
    // ZEROING CONNECTIONS - THIS WAS MISSING!!!
    // =========================================================================
    connect(m_zeroingController, &ZeroingController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_zeroingController, &ZeroingController::zeroingFinished,
            this, &ApplicationController::handleZeroingFinished);

    // =========================================================================
    // WINDAGE CONNECTIONS
    // =========================================================================
    connect(m_windageController, &WindageController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_windageController, &WindageController::windageFinished,
            this, &ApplicationController::handleWindageFinished);

    qDebug() << "ApplicationController: All signal connections established";

    // =========================================================================
    // ZONE DEFINITION CONNECTIONS
    // =========================================================================
    connect(m_zoneDefinitionController, &ZoneDefinitionController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_zoneDefinitionController, &ZoneDefinitionController::closed,
            this, &ApplicationController::handleZoneDefinitionFinished);

    qDebug() << "ApplicationController: All signal connections established (including Zone Definition)";

}

void ApplicationController::setMenuState(MenuState state)
{
    m_currentMenuState = state;
    qDebug() << "ApplicationController: Menu state changed to" << static_cast<int>(state);
}

void ApplicationController::showMainMenu()
{
    qDebug() << "ApplicationController: showMainMenu() called";
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
    m_zoneDefinitionController->hide();
}

// =============================================================================
// MENU/VAL BUTTON LOGIC
// =============================================================================

void ApplicationController::onMenuValButtonPressed()
{
    qDebug() << "ApplicationController: MENU/VAL button pressed in state"
             << static_cast<int>(m_currentMenuState);

    // Handle procedures first (they have priority)
    if (m_currentMenuState == MenuState::ZeroingProcedure ||
        m_currentMenuState == MenuState::WindageProcedure ||
        m_currentMenuState == MenuState::ZoneDefinition) {  // ✅ ADD THIS
        handleMenuValInProcedure();
        return;
    }

    // Handle menu states
    if (m_currentMenuState == MenuState::None) {
        showMainMenu();
        return;
    }

    if (m_currentMenuState == MenuState::MainMenu) {
        m_mainMenuController->onSelectButtonPressed();
        return;
    }

    if (m_currentMenuState == MenuState::ReticleMenu) {
        m_reticleMenuController->onSelectButtonPressed();
        return;
    }

    if (m_currentMenuState == MenuState::ColorMenu) {
        m_colorMenuController->onSelectButtonPressed();
        return;
    }

    qWarning() << "ApplicationController: MENU/VAL pressed in unhandled state:"
               << static_cast<int>(m_currentMenuState);
}

void ApplicationController::handleMenuValInNoMenuState()
{
    qDebug() << "ApplicationController: Opening main menu";
    showMainMenu();
}

void ApplicationController::handleMenuValInMainMenu()
{
    qDebug() << "ApplicationController: Selecting main menu item";
    m_mainMenuController->onSelectButtonPressed();
}

void ApplicationController::handleMenuValInSubmenu()
{
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
    qDebug() << "ApplicationController: Confirming procedure step";

    switch (m_currentMenuState) {
    case MenuState::ZeroingProcedure:
        m_zeroingController->onSelectButtonPressed();
        break;
    case MenuState::WindageProcedure:
        m_windageController->onSelectButtonPressed();
        break;
    case MenuState::ZoneDefinition:
        m_zoneDefinitionController->onMenuValButtonPressed();
        break;
    default:
        break;
    }
}

// =============================================================================
// UP/DOWN BUTTON LOGIC
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
    case MenuState::ZoneDefinition:
        m_zoneDefinitionController->onUpButtonPressed();
        break;
    default:
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
    case MenuState::ZoneDefinition:
        m_zoneDefinitionController->onDownButtonPressed();
        break;
    default:
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
    m_zoneDefinitionController->show();
    setMenuState(MenuState::ZoneDefinition);
}

void ApplicationController::handleZoneDefinitionFinished()
{
    qDebug() << "ApplicationController: Zone Definition finished";
    // State should be None now (set when returning to main menu)
}
void ApplicationController::handleSystemStatus()
{
    qDebug() << "ApplicationController: System Status requested";
    hideAllMenus();
    setMenuState(MenuState::SystemStatus);
    showMainMenu();
}

void ApplicationController::handleRadarTargetList()
{
    qDebug() << "ApplicationController: Radar Target List requested";
    hideAllMenus();
    setMenuState(MenuState::RadarTargets);
    showMainMenu();
}

void ApplicationController::handleHelpAbout()
{
    qDebug() << "ApplicationController: Help/About requested";
    hideAllMenus();
    setMenuState(MenuState::HelpAbout);
    showMainMenu();
}

// =============================================================================
// COMPLETION HANDLERS - THE KEY TO NAVIGATION
// =============================================================================

void ApplicationController::handleMainMenuFinished()
{
    qDebug() << "ApplicationController: handleMainMenuFinished() called";
    qDebug() << "ApplicationController: Current state =" << static_cast<int>(m_currentMenuState);

    // CRITICAL FIX: Only close menu if we're STILL in MainMenu state
    // If state already changed (e.g. to ZeroingProcedure), an action was taken
    // and we should NOT close the menu - it was already handled
    if (m_currentMenuState == MenuState::MainMenu) {
        // We're still in MainMenu state, which means "Return ..." was selected
        qDebug() << "ApplicationController: 'Return ...' was selected - closing menu";
        hideAllMenus();
        setMenuState(MenuState::None);
        qDebug() << "ApplicationController: Menu closed, state is now None";
    } else {
        qDebug() << "ApplicationController: State already changed, action was taken";
    }
}

void ApplicationController::handleReticleMenuFinished()
{
    qDebug() << "ApplicationController: Reticle menu finished";
    // Don't change state - reticle controller already did via returnToMainMenu
}

void ApplicationController::handleColorMenuFinished()
{
    qDebug() << "ApplicationController: Color menu finished";
    // Don't change state - color controller already did via returnToMainMenu
}

void ApplicationController::handleZeroingFinished()
{
    qDebug() << "ApplicationController: Zeroing procedure finished";
    // State should be None now (set when returning to main menu)
}

void ApplicationController::handleWindageFinished()
{
    qDebug() << "ApplicationController: Windage procedure finished";
    // State should be None now (set when returning to main menu)
}

void ApplicationController::handleReturnToMainMenu()
{
    qDebug() << "ApplicationController: handleReturnToMainMenu() CALLED";
    qDebug() << "ApplicationController: Current state before return:" << static_cast<int>(m_currentMenuState);

    hideAllMenus();
    showMainMenu();

    qDebug() << "ApplicationController: State after return:" << static_cast<int>(m_currentMenuState);
    qDebug() << "ApplicationController: Main menu should now be visible";
}


