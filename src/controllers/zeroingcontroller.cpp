#include "zeroingcontroller.h"
#include "models/zeroingviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "services/servicemanager.h"
#include <QDebug>

ZeroingController::ZeroingController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_stateModel(nullptr)
    , m_currentState(ZeroingState::Idle)
{
}

void ZeroingController::initialize()
{
    m_viewModel = ServiceManager::instance()->get<ZeroingViewModel>(QString("ZeroingViewModel"));
    m_stateModel = ServiceManager::instance()->get<SystemStateModel>();

    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_stateModel);

    // Connect to model changes
    connect(m_stateModel, &SystemStateModel::dataChanged,
            this, [this](const SystemStateData& data) {
                // If zeroing is externally cancelled
                if (!data.zeroingModeActive && m_currentState != ZeroingState::Idle
                    && m_currentState != ZeroingState::Completed) {
                    qDebug() << "Zeroing mode became inactive externally, finishing.";
                    hide();
                    emit zeroingFinished();
                }
            });
}

void ZeroingController::show()
{
    m_stateModel->startZeroingProcedure();
    transitionToState(ZeroingState::Instruct_MoveReticleToImpact);
    m_viewModel->setVisible(true);
}

void ZeroingController::hide()
{
    m_viewModel->setVisible(false);
    transitionToState(ZeroingState::Idle);
}

void ZeroingController::transitionToState(ZeroingState newState)
{
    m_currentState = newState;
    updateUI();
}

void ZeroingController::updateUI()
{
    switch (m_currentState) {
    case ZeroingState::Instruct_MoveReticleToImpact:
        m_viewModel->setTitle("Weapon Zeroing: Adjust");
        m_viewModel->setInstruction(
            "ZEROING\n\n"
            "1. (Fire weapon at a fixed target)\n"
            "2. Observe impact point.\n"
            "3. Use JOYSTICK to move main RETICLE to the ACTUAL IMPACT POINT.\n\n"
            "Press SELECT to set this as the new zero."
            );
        m_viewModel->setStatus("ADJUSTING RETICLE TO IMPACT");
        m_viewModel->setShowOffsets(false);
        break;

    case ZeroingState::Completed:
        m_viewModel->setTitle("Zeroing Applied");
        m_viewModel->setInstruction(
            "Zeroing Adjustment Applied!\n"
            "'Z' will display on OSD when active.\n"
            "You may need to repeat the process if further adjustment is required.\n\n"
            "Press SELECT or BACK to return to Settings Menu."
            );
        m_viewModel->setStatus(QString("FINAL OFFSETS: Az %1, El %2")
                                   .arg(m_stateModel->data().zeroingAzimuthOffset, 0, 'f', 2)
                                   .arg(m_stateModel->data().zeroingElevationOffset, 0, 'f', 2));
        m_viewModel->setShowOffsets(true);
        m_viewModel->setAzimuthOffset(m_stateModel->data().zeroingAzimuthOffset);
        m_viewModel->setElevationOffset(m_stateModel->data().zeroingElevationOffset);
        break;

    case ZeroingState::Idle:
    default:
        m_viewModel->setTitle("Weapon Zeroing");
        m_viewModel->setInstruction("Zeroing Standby.");
        m_viewModel->setStatus("");
        m_viewModel->setShowOffsets(false);
        break;
    }
}

void ZeroingController::onSelectButtonPressed()
{
    switch (m_currentState) {
    case ZeroingState::Instruct_MoveReticleToImpact:
        m_stateModel->finalizeZeroing();
        qDebug() << "Zeroing finalized. Offsets Az:"
                 << m_stateModel->data().zeroingAzimuthOffset
                 << "El:" << m_stateModel->data().zeroingElevationOffset;
        transitionToState(ZeroingState::Completed);
        break;

    case ZeroingState::Completed:
        // FIXED: Return to main menu instead of exiting
        hide();
        emit returnToMainMenu();  // Goes back to main menu
        emit zeroingFinished();
        break;

    default:
        qWarning() << "ZeroingController: handleSelectAction unhandled for state:"
                   << static_cast<int>(m_currentState);
        break;
    }
}

void ZeroingController::onBackButtonPressed()
{
    // NOTE: This method exists for compatibility but won't be called
    // since there's no physical BACK button anymore
    // Only MENU/VAL acts as SELECT in procedures

    SystemStateData currentData = m_stateModel->data();

    if (currentData.zeroingModeActive) {
        if (!currentData.zeroingAppliedToBallistics && m_currentState != ZeroingState::Completed) {
            m_stateModel->clearZeroing();
        } else {
            SystemStateData updatedData = currentData;
            updatedData.zeroingModeActive = false;
            m_stateModel->updateData(updatedData);
            qDebug() << "ZeroingController: Exiting UI, applied zeroing remains.";
        }
    }

    hide();
    emit returnToMainMenu();
    emit zeroingFinished();
}

void ZeroingController::onUpButtonPressed()
{
    // TODO: Implement fine-tuning if needed
}

void ZeroingController::onDownButtonPressed()
{
    // TODO: Implement fine-tuning if needed
}
