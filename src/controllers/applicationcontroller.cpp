#include "controllers/applicationcontroller.h"
#include "controllers/mainmenucontroller.h"
#include "controllers/reticlemenucontroller.h"
#include "controllers/colormenucontroller.h"
#include "services/servicemanager.h"
#include <QDebug>

ApplicationController::ApplicationController(QObject *parent)
    : QObject(parent)
    , m_currentMenuState(MenuState::None)
    , m_mainMenuController(nullptr)
    , m_reticleMenuController(nullptr)
    , m_colorMenuController(nullptr)
{
}

void ApplicationController::initialize()
{
    // Get controllers from ServiceManager
    m_mainMenuController = ServiceManager::instance()->get<MainMenuController>();
    Q_ASSERT(m_mainMenuController);

    // You'll need to register these in ServiceManager too
    m_reticleMenuController = ServiceManager::instance()->get<ReticleMenuController>();
    m_colorMenuController = ServiceManager::instance()->get<ColorMenuController>();

    // If not registered, create them here:
    if (!m_reticleMenuController) {
        m_reticleMenuController = new ReticleMenuController(this);
        m_reticleMenuController->initialize();
    }

    if (!m_colorMenuController) {
        m_colorMenuController = new ColorMenuController(this);
        m_colorMenuController->initialize();
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
    // Hide other menus as needed
}

void ApplicationController::onMenuButtonPressed()
{
    qDebug() << "ApplicationController: Menu button pressed";

    switch (m_currentMenuState) {
    case MenuState::None:
        showMainMenu();
        break;

    case MenuState::MainMenu:
        // Toggle off
        hideAllMenus();
        setMenuState(MenuState::None);
        break;

    case MenuState::ReticleMenu:
    case MenuState::ColorMenu:
        // Return to main menu
        handleReturnToMainMenu();
        break;

    default:
        // For other states, close everything
        hideAllMenus();
        setMenuState(MenuState::None);
        break;
    }
}

void ApplicationController::onUpButtonPressed()
{
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
    // Add other menu states
    default:
        break;
    }
}

void ApplicationController::onDownButtonPressed()
{
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
    // Add other menu states
    default:
        break;
    }
}

void ApplicationController::onSelectButtonPressed()
{
    switch (m_currentMenuState) {
    case MenuState::MainMenu:
        m_mainMenuController->onSelectButtonPressed();
        break;
    case MenuState::ReticleMenu:
        m_reticleMenuController->onSelectButtonPressed();
        break;
    case MenuState::ColorMenu:
        m_colorMenuController->onSelectButtonPressed();
        break;
    // Add other menu states
    default:
        break;
    }
}

void ApplicationController::onBackButtonPressed()
{
    switch (m_currentMenuState) {
    case MenuState::MainMenu:
        m_mainMenuController->onBackButtonPressed();
        setMenuState(MenuState::None);
        break;
    case MenuState::ReticleMenu:
        m_reticleMenuController->onBackButtonPressed();
        break;
    case MenuState::ColorMenu:
        m_colorMenuController->onBackButtonPressed();
        break;
    // Add other menu states
    default:
        hideAllMenus();
        setMenuState(MenuState::None);
        break;
    }
}

// Main Menu Action Handlers
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
    setMenuState(MenuState::ZeroingProcedure);

    // TODO: Show zeroing widget
    // Create and show ZeroingWidget from your existing code
    // For now, return to main menu
    showMainMenu();
}

void ApplicationController::handleClearZero()
{
    qDebug() << "ApplicationController: Clear Zero requested";
    // TODO: Call model to clear zeroing
    // m_stateModel->clearZeroing();
    showMainMenu();
}

void ApplicationController::handleWindage()
{
    qDebug() << "ApplicationController: Windage requested";
    hideAllMenus();
    setMenuState(MenuState::WindageProcedure);

    // TODO: Show windage widget
    // Create and show WindageWidget from your existing code
    showMainMenu();
}

void ApplicationController::handleClearWindage()
{
    qDebug() << "ApplicationController: Clear Windage requested";
    // TODO: Call model to clear windage
    // m_stateModel->clearWindage();
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

// Submenu handlers
void ApplicationController::handleReticleMenuFinished()
{
    qDebug() << "ApplicationController: Reticle menu finished";
    // Menu is already hidden by controller
}

void ApplicationController::handleColorMenuFinished()
{
    qDebug() << "ApplicationController: Color menu finished";
    // Menu is already hidden by controller
}

void ApplicationController::handleReturnToMainMenu()
{
    qDebug() << "ApplicationController: Returning to main menu";
    hideAllMenus();
    showMainMenu();
}
