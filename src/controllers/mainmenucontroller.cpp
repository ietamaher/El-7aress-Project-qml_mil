#include "controllers/mainmenucontroller.h"
#include "services/servicemanager.h"
#include "models/menuviewmodel.h"
#include "models/osdviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include <QDebug>

MainMenuController::MainMenuController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_stateModel(nullptr)
{
}

void MainMenuController::initialize()
{
    //m_viewModel = ServiceManager::instance()->get<MenuViewModel>(QString("MainMenuViewModel"));
    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_stateModel);

    connect(m_viewModel, &MenuViewModel::optionSelected,
            this, &MainMenuController::handleMenuOptionSelected);
    //SystemStateModel* stateModel = ServiceManager::instance()->get<SystemStateModel>();
    if (m_stateModel) {
        connect(m_stateModel, &SystemStateModel::colorStyleChanged,
                this, &MainMenuController::onColorStyleChanged);
        
        // Set initial color
        const auto& data = m_stateModel->data();
        m_viewModel->setAccentColor(data.colorStyle);
    }
}

QStringList MainMenuController::buildMainMenuOptions() const
{
    QStringList options;
    options << "--- RETICLE & DISPLAY ---"
            << "Personalize Reticle"
            << "Personalize Colors"
            << "Adjust Brightness"
            << "--- BALLISTICS ---"
            << "Zeroing"
            << "Clear Active Zero"
            << "Windage"
            << "Clear Active Windage"
            << "--- SYSTEM ---"
            << "Zone Definitions"
            << "System Status"
            << "--- INFO ---"
            << "Help/About"
            << "Return ...";

    // NO "Return ..." option since pressing MENU/VAL again will close the menu

    return options;
}

void MainMenuController::show()
{
    QStringList menuOptions = buildMainMenuOptions();
    m_viewModel->showMenu("Main Menu", "Navigate with UP/DOWN, Select with MENU/VAL", menuOptions);
}

void MainMenuController::hide()
{
    m_viewModel->hideMenu();
}

void MainMenuController::onUpButtonPressed()
{
    m_viewModel->moveSelectionUp();
}

void MainMenuController::onDownButtonPressed()
{
    m_viewModel->moveSelectionDown();
}

void MainMenuController::onSelectButtonPressed()
{
    // This is called when MENU/VAL is pressed while in the main menu
    m_viewModel->selectCurrentItem();
}

void MainMenuController::handleMenuOptionSelected(const QString& option)
{
    qDebug() << "MainMenuController: Option selected:" << option;

    hide(); // Always hide the menu after selection

    // Route to appropriate handler based on selection
    if (option == "Personalize Reticle") {
        emit personalizeReticleRequested();
        emit menuFinished(); // ✅ Emit after each action
    }
    else if (option == "Personalize Colors") {
        emit personalizeColorsRequested();
        emit menuFinished();
    }
    else if (option == "Adjust Brightness") {
        emit adjustBrightnessRequested();
        emit menuFinished();
    }
    else if (option == "Zeroing") {
        emit zeroingRequested();
        // ❌ DON'T emit menuFinished() - zeroing is a procedure
    }
    else if (option == "Clear Active Zero") {
        emit clearZeroRequested();
        emit menuFinished();
    }
    else if (option == "Windage") {
        emit windageRequested();
        // ❌ DON'T emit menuFinished() - windage is a procedure
    }
    else if (option == "Clear Active Windage") {
        emit clearWindageRequested();
        emit menuFinished();
    }
    else if (option == "Zone Definitions") {
        emit zoneDefinitionsRequested();
        // ❌ DON'T emit menuFinished() - zone definition is a procedure
    }
    else if (option == "System Status") {
        emit systemStatusRequested();
       // emit menuFinished();
    }
    else if (option == "Radar Target List") {
        emit radarTargetListRequested();
        //emit menuFinished();
    }
    else if (option == "Help/About") {
        emit helpAboutRequested();
        // menuFinished();
    }
    else if (option == "Return ...") {
        qDebug() << "MainMenuController: Return option selected - closing menu";
        emit menuFinished();
    }
    else {
        qWarning() << "MainMenuController: Unknown option:" << option;
    }
}

void MainMenuController::onColorStyleChanged(const QColor& color)
{
    qDebug() << "MainMenuController: Color changed to" << color;
    m_viewModel->setAccentColor(color);
}

void MainMenuController::setViewModel(MenuViewModel* viewModel)
{
    m_viewModel = viewModel;
}

void MainMenuController::setStateModel(SystemStateModel* stateModel)
{
    m_stateModel = stateModel;
}

