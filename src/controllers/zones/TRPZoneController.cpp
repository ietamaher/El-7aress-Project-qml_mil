#include "TRPZoneController.h"
#include "models/trpparameterviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include <QDebug>

TRPZoneController::TRPZoneController(QObject* parent)
    : BaseZoneController(parent)
    , m_paramViewModel(nullptr)
    , m_editingTRPId(-1)
    , m_isModifying(false)
{
    resetWipTRP();
}

void TRPZoneController::setParameterViewModel(TRPParameterViewModel* paramViewModel)
{
    m_paramViewModel = paramViewModel;
}

void TRPZoneController::initialize()
{
    BaseZoneController::initialize();
    Q_ASSERT(m_paramViewModel);
    qDebug() << "TRPZoneController initialized";
}

void TRPZoneController::show()
{
    BaseZoneController::show();
    setupSelectActionUI();
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

void TRPZoneController::onMenuValButtonPressed()
{
    if (!isActive()) return;

    switch (currentState()) {
        case State::SelectAction:
            handleSelectActionInput();
            break;
        case State::SelectExistingZone:
            handleSelectExistingTRPInput();
            break;
        case State::AimingPoint:
            handleAimingPointInput();
            break;
        case State::EditParameters:
            handleEditParametersInput();
            break;
        case State::ConfirmSave:
            handleConfirmSaveInput();
            break;
        case State::ConfirmDelete:
            handleConfirmDeleteInput();
            break;
        case State::ShowMessage:
            setupSelectActionUI();
            break;
        default:
            qWarning() << "Unhandled MenuVal in state" << (int)currentState();
            break;
    }
}

void TRPZoneController::onUpButtonPressed()
{
    if (!isActive()) return;

    if (currentState() == State::EditParameters) {
        routeUpToParameterPanel();
    } else {
        BaseZoneController::onUpButtonPressed();
    }
}

void TRPZoneController::onDownButtonPressed()
{
    if (!isActive()) return;

    if (currentState() == State::EditParameters) {
        routeDownToParameterPanel();
    } else {
        BaseZoneController::onDownButtonPressed();
    }
}

// ============================================================================
// STATE HANDLERS
// ============================================================================

void TRPZoneController::handleSelectActionInput()
{
    QString action = selectedMenuItem();

    if (action == "New TRP") {
        createNewZone();
    } else if (action == "Modify TRP") {
        setupSelectExistingTRPUI("Modify");
        m_isModifying = true;
    } else if (action == "Delete TRP") {
        setupSelectExistingTRPUI("Delete");
        m_isModifying = false;
    } else if (action == "Exit") {
        hide();
        emit finished();
    }
}

void TRPZoneController::handleSelectExistingTRPInput()
{
    int trpId = getZoneIdFromMenuIndex(currentMenuIndex());

    if (trpId < 0) {
        showErrorMessage("Invalid TRP selection");
        return;
    }

    if (m_isModifying) {
        loadZoneForModification(trpId);
    } else {
        m_editingTRPId = trpId;
        loadWipTRPFromSystem(trpId);
        setupConfirmUI("Confirm Delete",
                      QString("Delete TRP %1-%2 (Az:%.1f° El:%.1f°)?")
                          .arg(m_wipTRP.locationPage).arg(m_wipTRP.trpInPage)
                          .arg(m_wipTRP.azimuth).arg(m_wipTRP.elevation));
        transitionToState(State::ConfirmDelete);
    }
}

void TRPZoneController::handleAimingPointInput()
{
    // Capture current gimbal position
    m_wipTRP.azimuth = currentGimbalAz();
    m_wipTRP.elevation = currentGimbalEl();

    qDebug() << "TRP point captured:" << m_wipTRP.azimuth << "," << m_wipTRP.elevation;

    // Move to parameter editing
    setupEditParametersUI();
}

void TRPZoneController::handleEditParametersInput()
{
    syncParameterPanelToWipTRP();
    setupConfirmUI("Confirm Save",
                  QString("Save TRP %1-%2?")
                      .arg(m_wipTRP.locationPage).arg(m_wipTRP.trpInPage));
    transitionToState(State::ConfirmSave);
}

void TRPZoneController::handleConfirmSaveInput()
{
    if (saveCurrentZone()) {
        QString msg = (m_editingTRPId < 0) ?
            "TRP created successfully" : "TRP modified successfully";
        showSuccessMessage(msg);

        if (m_editingTRPId < 0) {
            emit zoneCreated(ZoneType::TargetReferencePoint);
        } else {
            emit zoneModified(ZoneType::TargetReferencePoint, m_editingTRPId);
        }

        resetWipTRP();
    } else {
        showErrorMessage("Failed to save TRP");
    }
}

void TRPZoneController::handleConfirmDeleteInput()
{
    performZoneDeletion(m_editingTRPId);
    showSuccessMessage("TRP deleted successfully");
    emit zoneDeleted(ZoneType::TargetReferencePoint, m_editingTRPId);
    resetWipTRP();
}

// ============================================================================
// ABSTRACT METHOD IMPLEMENTATIONS
// ============================================================================

void TRPZoneController::createNewZone()
{
    qDebug() << "TRPZoneController: Creating new TRP";

    resetWipTRP();
    m_editingTRPId = -1;

    // Assign next available ID
    const auto& data = stateModel()->data();
    int maxId = 0;
    for (const auto& trp : data.targetReferencePoints) {
        if (trp.id > maxId) maxId = trp.id;
    }
    m_wipTRP.id = maxId + 1;
    m_wipTRP.locationPage = 1;
    m_wipTRP.trpInPage = m_wipTRP.id;

    setupAimingPointUI();
}

void TRPZoneController::loadZoneForModification(int trpId)
{
    qDebug() << "TRPZoneController: Loading TRP" << trpId;

    loadWipTRPFromSystem(trpId);
    m_editingTRPId = trpId;

    syncWipTRPToParameterPanel();
    setupEditParametersUI();
}

void TRPZoneController::performZoneDeletion(int trpId)
{
    qDebug() << "TRPZoneController: Deleting TRP" << trpId;
    stateModel()->deleteTRP(trpId);
}

bool TRPZoneController::saveCurrentZone()
{
    qDebug() << "TRPZoneController: Saving TRP" << m_wipTRP.id;

    syncParameterPanelToWipTRP();

    // Validate - ensure we have valid positioning
    if (m_wipTRP.locationPage <= 0 || m_wipTRP.trpInPage <= 0) {
        showErrorMessage("TRP location/number invalid");
        return false;
    }

    // Save to system state
    if (m_editingTRPId < 0) {
        stateModel()->addTRP(m_wipTRP);
    } else {
        stateModel()->modifyTRP(m_editingTRPId, m_wipTRP);
    }

    return true;
}

void TRPZoneController::updateWipZoneVisualization()
{
    // TRP visualization on map (placeholder - implement if needed)
    // mapViewModel()->setWipTRP(m_wipTRP);
}

QStringList TRPZoneController::getExistingZoneNames() const
{
    QStringList names;
    const auto& data = stateModel()->data();

    for (const auto& trp : data.targetReferencePoints) {
        names << QString("TRP %1-%2 (Az:%.1f° El:%.1f°)")
                     .arg(trp.locationPage).arg(trp.trpInPage)
                     .arg(trp.azimuth)
                     .arg(trp.elevation);
    }

    if (names.isEmpty()) {
        names << "(No TRPs defined)";
    }

    return names;
}

int TRPZoneController::getZoneIdFromMenuIndex(int menuIndex) const
{
    const auto& data = stateModel()->data();

    if (menuIndex < 0 || menuIndex >= data.targetReferencePoints.size()) {
        return -1;
    }

    return data.targetReferencePoints[menuIndex].id;
}

// ============================================================================
// UI SETUP
// ============================================================================

void TRPZoneController::setupSelectActionUI()
{
    QStringList actions = {"New TRP", "Modify TRP", "Delete TRP", "Exit"};
    setupMenuUI("Target Reference Point Management", actions);
    transitionToState(State::SelectAction);
}

void TRPZoneController::setupSelectExistingTRPUI(const QString& action)
{
    QStringList trps = getExistingZoneNames();
    setupMenuUI(QString("%1 TRP").arg(action), trps);
    transitionToState(State::SelectExistingZone);
}

void TRPZoneController::setupAimingPointUI()
{
    viewModel()->setTitle("Aim at Target");
    viewModel()->setInstruction("Point gimbal at target reference point, then press VAL");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(false);
    viewModel()->setShowConfirmDialog(false);
    transitionToState(State::AimingPoint);
}

void TRPZoneController::setupEditParametersUI()
{
    viewModel()->setTitle("Edit TRP Parameters");
    viewModel()->setInstruction("Use UP/DOWN to navigate, VAL to confirm");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(true);
    viewModel()->setShowConfirmDialog(false);

    syncWipTRPToParameterPanel();
    transitionToState(State::EditParameters);
}

// ============================================================================
// PARAMETER PANEL ROUTING
// ============================================================================

void TRPZoneController::routeUpToParameterPanel()
{
    // TRP parameter navigation - simplified
    // m_paramViewModel->navigateUp();
}

void TRPZoneController::routeDownToParameterPanel()
{
    // TRP parameter navigation - simplified
    // m_paramViewModel->navigateDown();
}

void TRPZoneController::routeSelectToParameterPanel()
{
    // TRP parameter selection - simplified
    // m_paramViewModel->confirmSelection();
}

// ============================================================================
// WIP TRP MANAGEMENT
// ============================================================================

void TRPZoneController::resetWipTRP()
{
    m_wipTRP = TargetReferencePoint{};
    m_editingTRPId = -1;
}

void TRPZoneController::loadWipTRPFromSystem(int trpId)
{
    const auto& data = stateModel()->data();

    for (const auto& trp : data.targetReferencePoints) {
        if (trp.id == trpId) {
            m_wipTRP = trp;
            qDebug() << "Loaded TRP" << trpId << ": Page" << trp.locationPage << "-" << trp.trpInPage;
            return;
        }
    }

    qWarning() << "TRP" << trpId << "not found!";
    resetWipTRP();
}

void TRPZoneController::syncWipTRPToParameterPanel()
{
    // Sync WIP TRP data to parameter panel
    m_paramViewModel->setTRPName(QString("TRP %1-%2").arg(m_wipTRP.locationPage).arg(m_wipTRP.trpInPage));
    m_paramViewModel->setDescription(QString("Az:%.1f° El:%.1f°").arg(m_wipTRP.azimuth).arg(m_wipTRP.elevation));
    m_paramViewModel->setAzimuth(m_wipTRP.azimuth);
    m_paramViewModel->setElevation(m_wipTRP.elevation);
}

void TRPZoneController::syncParameterPanelToWipTRP()
{
    // Sync parameter panel changes back to WIP TRP
    // Note: locationPage/trpInPage are set during creation, not editable via panel
    m_wipTRP.azimuth = m_paramViewModel->azimuth();
    m_wipTRP.elevation = m_paramViewModel->elevation();
}
