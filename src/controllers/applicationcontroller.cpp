#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "controllers/zeroingcontroller.h"
#include "controllers/windagecontroller.h"
#include "controllers/zonedefinitioncontroller.h"
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
    , m_systemStateModel(nullptr)
{
}

// ============================================================================
// DEPENDENCY INJECTION
// ============================================================================

void ApplicationController::setMainMenuController(MainMenuController* controller)
{
    m_mainMenuController = controller;
}

void ApplicationController::setReticleMenuController(ReticleMenuController* controller)
{
    m_reticleMenuController = controller;
}

void ApplicationController::setColorMenuController(ColorMenuController* controller)
{
    m_colorMenuController = controller;
}

void ApplicationController::setZeroingController(ZeroingController* controller)
{
    m_zeroingController = controller;
}

void ApplicationController::setWindageController(WindageController* controller)
{
    m_windageController = controller;
}

void ApplicationController::setZoneDefinitionController(ZoneDefinitionController* controller)
{
    m_zoneDefinitionController = controller;
}

void ApplicationController::setSystemStateModel(SystemStateModel* model)
{
    m_systemStateModel = model;
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void ApplicationController::initialize()
{
    qDebug() << "ApplicationController: Initializing...";

    // Verify all dependencies are set
    Q_ASSERT(m_mainMenuController);
    Q_ASSERT(m_reticleMenuController);
    Q_ASSERT(m_colorMenuController);
    Q_ASSERT(m_zeroingController);
    Q_ASSERT(m_windageController);
    Q_ASSERT(m_zoneDefinitionController);
    Q_ASSERT(m_systemStateModel);

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
    // ZEROING CONNECTIONS
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

    // =========================================================================
    // ZONE DEFINITION CONNECTIONS
    // =========================================================================
    connect(m_zoneDefinitionController, &ZoneDefinitionController::returnToMainMenu,
            this, &ApplicationController::handleReturnToMainMenu);
    connect(m_zoneDefinitionController, &ZoneDefinitionController::closed,
            this, &ApplicationController::handleZoneDefinitionFinished);

    qDebug() << "ApplicationController: All signal connections established";
}

// ============================================================================
// STATE MANAGEMENT
// ============================================================================

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

// ============================================================================
// BUTTON HANDLERS
// ============================================================================

void ApplicationController::onMenuValButtonPressed()
{
    qDebug() << "ApplicationController: MENU/VAL button pressed in state"
             << static_cast<int>(m_currentMenuState);

    // Handle procedures first (they have priority)
    if (m_currentMenuState == MenuState::ZeroingProcedure ||
        m_currentMenuState == MenuState::WindageProcedure ||
        m_currentMenuState == MenuState::ZoneDefinition) {
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

// ============================================================================
// MAIN MENU ACTION HANDLERS
// ============================================================================

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
    // TODO: Show brightness adjustment UI
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
    if (m_systemStateModel) {
        m_systemStateModel->clearZeroing();
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
    if (m_systemStateModel) {
        m_systemStateModel->clearWindage();
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

void ApplicationController::handleSystemStatus()
{
    qDebug() << "ApplicationController: System Status requested";
    hideAllMenus();
    setMenuState(MenuState::SystemStatus);
    // TODO: Show system status UI
    showMainMenu();
}

void ApplicationController::handleRadarTargetList()
{
    qDebug() << "ApplicationController: Radar Target List requested";
    hideAllMenus();
    setMenuState(MenuState::RadarTargets);
    // TODO: Show radar target list UI
    showMainMenu();
}

void ApplicationController::handleHelpAbout()
{
    qDebug() << "ApplicationController: Help/About requested";
    hideAllMenus();
    setMenuState(MenuState::HelpAbout);
    // TODO: Show help/about UI
    showMainMenu();
}

// ============================================================================
// COMPLETION HANDLERS
// ============================================================================

void ApplicationController::handleMainMenuFinished()
{
    qDebug() << "ApplicationController: handleMainMenuFinished()";
    qDebug() << "  Current state:" << static_cast<int>(m_currentMenuState);

    // Only close menu if still in MainMenu state
    // If state changed, an action was already taken
    if (m_currentMenuState == MenuState::MainMenu) {
        qDebug() << "  'Return ...' was selected - closing menu";
        hideAllMenus();
        setMenuState(MenuState::None);
    } else {
        qDebug() << "  State already changed, action was taken";
    }
}

void ApplicationController::handleReticleMenuFinished()
{
    qDebug() << "ApplicationController: Reticle menu finished";
}

void ApplicationController::handleColorMenuFinished()
{
    qDebug() << "ApplicationController: Color menu finished";
}

void ApplicationController::handleZeroingFinished()
{
    qDebug() << "ApplicationController: Zeroing procedure finished";
}

void ApplicationController::handleWindageFinished()
{
    qDebug() << "ApplicationController: Windage procedure finished";
}

void ApplicationController::handleZoneDefinitionFinished()
{
    qDebug() << "ApplicationController: Zone Definition finished";
}

void ApplicationController::handleReturnToMainMenu()
{
    qDebug() << "ApplicationController: handleReturnToMainMenu()";
    qDebug() << "  Current state:" << static_cast<int>(m_currentMenuState);

    hideAllMenus();
    showMainMenu();

    qDebug() << "  New state:" << static_cast<int>(m_currentMenuState);
}
