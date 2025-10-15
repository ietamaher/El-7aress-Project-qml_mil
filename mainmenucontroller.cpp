#include "mainmenucontroller.h"
#include "servicemanager.h"
#include "menuviewmodel.h"
#include <QDebug>

MainMenuController::MainMenuController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
{
}

void MainMenuController::initialize()
{
    m_viewModel = ServiceManager::instance()->get<MenuViewModel>();
    Q_ASSERT(m_viewModel);

    connect(m_viewModel, &MenuViewModel::optionSelected,
            this, &MainMenuController::handleMenuOptionSelected);
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
            << "Radar Target List" // Optional
            << "--- INFO ---"
            << "Help/About"
            << "Return ...";

    return options;
}

void MainMenuController::show()
{
    QStringList menuOptions = buildMainMenuOptions();
    m_viewModel->showMenu("Main Menu", "Navigate Through Options", menuOptions);
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
    m_viewModel->selectCurrentItem();
}

void MainMenuController::onBackButtonPressed()
{
    // Back button closes the menu
    hide();
    emit menuFinished();
}

void MainMenuController::handleMenuOptionSelected(const QString& option)
{
    qDebug() << "MainMenuController: Option selected:" << option;

    hide(); // Always hide the menu after selection

    // Route to appropriate handler based on selection
    if (option == "Personalize Reticle") {
        emit personalizeReticleRequested();
    }
    else if (option == "Personalize Colors") {
        emit personalizeColorsRequested();
    }
    else if (option == "Adjust Brightness") {
        emit adjustBrightnessRequested();
    }
    else if (option == "Zeroing") {
        emit zeroingRequested();
    }
    else if (option == "Clear Active Zero") {
        emit clearZeroRequested();
    }
    else if (option == "Windage") {
        emit windageRequested();
    }
    else if (option == "Clear Active Windage") {
        emit clearWindageRequested();
    }
    else if (option == "Zone Definitions") {
        emit zoneDefinitionsRequested();
    }
    else if (option == "System Status") {
        emit systemStatusRequested();
    }
    else if (option == "Radar Target List") {
        emit radarTargetListRequested();
    }
    else if (option == "Help/About") {
        emit helpAboutRequested();
    }
    else if (option == "Return ...") {
        // Just close menu
    }
    else {
        qWarning() << "MainMenuController: Unknown option:" << option;
    }

    emit menuFinished();
}
