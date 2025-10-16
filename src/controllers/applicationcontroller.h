#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <QObject>

class MainMenuController;
class ReticleMenuController;
class ColorMenuController;
class ZeroingController;
class WindageController;
class ZoneDefinitionController;

class ApplicationController : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationController(QObject *parent = nullptr);
    void initialize();

public slots:
    // PHYSICAL BUTTONS - Only 3 buttons now
    void onMenuValButtonPressed();  // Combined MENU and VALIDATION button
    void onUpButtonPressed();
    void onDownButtonPressed();

private slots:
    // Main Menu handlers
    void handlePersonalizeReticle();
    void handlePersonalizeColors();
    void handleAdjustBrightness();
    void handleZeroing();
    void handleClearZero();
    void handleWindage();
    void handleClearWindage();
    void handleZoneDefinitions();
    void handleSystemStatus();
    void handleRadarTargetList();
    void handleHelpAbout();

    // Submenu handlers
    void handleReticleMenuFinished();
    void handleColorMenuFinished();
    void handleZeroingFinished();
    void handleWindageFinished();
    void handleReturnToMainMenu();
    void handleMainMenuFinished();
    void handleZoneDefinitionFinished();

private:
    enum class MenuState {
        None,                    // No menu visible
        MainMenu,               // Main menu open
        ReticleMenu,            // Reticle selection submenu
        ColorMenu,              // Color selection submenu
        BrightnessAdjust,       // Brightness adjustment (future)
        ZeroingProcedure,       // Zeroing procedure active
        WindageProcedure,       // Windage procedure active
        ZoneDefinition,         // Zone definition (future)
        SystemStatus,           // System status (future)
        RadarTargets,           // Radar targets (future)
        HelpAbout               // Help/About (future)
    };

    void showMainMenu();
    void hideAllMenus();
    void setMenuState(MenuState state);

    // MENU/VAL button behavior depends on current state
    void handleMenuValInNoMenuState();
    void handleMenuValInMainMenu();
    void handleMenuValInSubmenu();
    void handleMenuValInProcedure();

    MenuState m_currentMenuState;

    // Controllers
    MainMenuController* m_mainMenuController;
    ReticleMenuController* m_reticleMenuController;
    ColorMenuController* m_colorMenuController;
    ZeroingController* m_zeroingController;
    WindageController* m_windageController;
    ZoneDefinitionController* m_zoneDefinitionController;
};

#endif // APPLICATIONCONTROLLER_H
