#include "controllers/reticlemenucontroller.h"
#include "services/servicemanager.h"
#include "models/osdviewmodel.h"
#include <QDebug>

ReticleMenuController::ReticleMenuController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_osdViewModel(nullptr)
    , m_originalReticleType(ReticleType::Basic)
{
}

void ReticleMenuController::initialize()
{
    // Get the RETICLE menu's ViewModel specifically by name
    m_viewModel = ServiceManager::instance()->get<MenuViewModel>(QString("ReticleMenuViewModel"));
    m_osdViewModel = ServiceManager::instance()->get<OsdViewModel>();

    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_osdViewModel);

    connect(m_viewModel, &MenuViewModel::optionSelected,
            this, &ReticleMenuController::handleMenuOptionSelected);
}

QStringList ReticleMenuController::buildReticleOptions() const
{
    QStringList options;

    for (int i = 0; i < static_cast<int>(ReticleType::COUNT); ++i) {
        options << reticleTypeToString(static_cast<ReticleType>(i));
    }

    options << "Return ...";
    return options;
}

QString ReticleMenuController::reticleTypeToString(ReticleType type) const
{
    switch(type) {
    case ReticleType::Basic: return "Basic";
    case ReticleType::BoxCrosshair: return "Box Crosshair";
    case ReticleType::StandardCrosshair: return "Standard Crosshair";
    case ReticleType::PrecisionCrosshair: return "Precision Crosshair";
    case ReticleType::MilDot: return "Mil-Dot";
    default: return "Unknown";
    }
}

ReticleType ReticleMenuController::stringToReticleType(const QString& str) const
{
    if (str == "Basic") return ReticleType::Basic;
    if (str == "Box Crosshair") return ReticleType::BoxCrosshair;
    if (str == "Standard Crosshair") return ReticleType::StandardCrosshair;
    if (str == "Precision Crosshair") return ReticleType::PrecisionCrosshair;
    if (str == "Mil-Dot") return ReticleType::MilDot;
    return ReticleType::Basic;
}

void ReticleMenuController::show()
{
    // Save current reticle type
    // m_originalReticleType = m_osdViewModel->getCurrentReticleType();

    QStringList options = buildReticleOptions();
    m_viewModel->showMenu("Personalize Reticle", "Select Reticle Style", options);

    // Set current selection to match current reticle
    // int currentIndex = static_cast<int>(m_originalReticleType);
    // if (currentIndex >= 0 && currentIndex < options.size()) {
    //     m_viewModel->setCurrentIndex(currentIndex);
    // }
}

void ReticleMenuController::hide()
{
    m_viewModel->hideMenu();
}

void ReticleMenuController::onUpButtonPressed()
{
    m_viewModel->moveSelectionUp();

    // Preview the reticle as user navigates
    int currentIndex = m_viewModel->currentIndex();
    handleCurrentItemChanged(currentIndex);
}

void ReticleMenuController::onDownButtonPressed()
{
    m_viewModel->moveSelectionDown();

    // Preview the reticle as user navigates
    int currentIndex = m_viewModel->currentIndex();
    handleCurrentItemChanged(currentIndex);
}

void ReticleMenuController::onSelectButtonPressed()
{
    m_viewModel->selectCurrentItem();
}

void ReticleMenuController::onBackButtonPressed()
{
    // Restore original reticle on cancel
    // m_osdViewModel->setReticleType(m_originalReticleType);

    hide();
    emit returnToMainMenu();
    emit menuFinished();
}

void ReticleMenuController::handleCurrentItemChanged(int index)
{
    // Get the option text at this index for preview
    QStringList options = buildReticleOptions();
    if (index >= 0 && index < options.size() - 1) { // Exclude "Return ..."
        QString optionText = options.at(index);
        ReticleType previewType = stringToReticleType(optionText);

        // Update OSD with preview
        // m_osdViewModel->setReticleType(previewType);

        qDebug() << "ReticleMenuController: Previewing" << optionText;
    }
}

void ReticleMenuController::handleMenuOptionSelected(const QString& option)
{
    qDebug() << "ReticleMenuController: Selected" << option;

    hide();

    if (option == "Return ...") {
        // Restore original
        // m_osdViewModel->setReticleType(m_originalReticleType);
        emit returnToMainMenu();
    } else {
        // Apply the selected reticle permanently
        ReticleType selectedType = stringToReticleType(option);
        // m_osdViewModel->setReticleType(selectedType);
        // m_osdViewModel->saveReticleType(); // Persist to settings

        qDebug() << "ReticleMenuController: Applied" << option;
        emit returnToMainMenu();
    }

    emit menuFinished();
}
