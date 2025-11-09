#include "ZoneManagementCoordinator.h"
#include "SafetyZoneController.h"
#include "SectorScanZoneController.h"
#include "TRPZoneController.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "models/areazoneparameterviewmodel.h"
#include "models/sectorscanparameterviewmodel.h"
#include "models/trpparameterviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include <QDebug>

ZoneManagementCoordinator::ZoneManagementCoordinator(QObject* parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_mapViewModel(nullptr)
    , m_areaZoneParamViewModel(nullptr)
    , m_sectorScanParamViewModel(nullptr)
    , m_trpParamViewModel(nullptr)
    , m_stateModel(nullptr)
    , m_safetyZoneController(nullptr)
    , m_sectorScanController(nullptr)
    , m_trpController(nullptr)
    , m_currentState(CoordinatorState::Idle)
    , m_selectedZoneType(ZoneType::None)
    , m_zoneTypeMenuIndex(0)
{
    qDebug() << "ZoneManagementCoordinator created";

    // Create specialized controllers
    m_safetyZoneController = new SafetyZoneController(this);
    m_sectorScanController = new SectorScanZoneController(this);
    m_trpController = new TRPZoneController(this);

    // Connect controller lifecycle signals
    connect(m_safetyZoneController, &SafetyZoneController::finished,
            this, &ZoneManagementCoordinator::onSafetyZoneFinished);
    connect(m_sectorScanController, &SectorScanZoneController::finished,
            this, &ZoneManagementCoordinator::onSectorScanFinished);
    connect(m_trpController, &TRPZoneController::finished,
            this, &ZoneManagementCoordinator::onTRPFinished);

    // Setup zone type menu
    m_zoneTypeMenuItems = {
        "Safety Zones (NoFire/NoTraverse)",
        "Auto Sector Scan Zones",
        "Target Reference Points (TRP)",
        "Exit"
    };
}

ZoneManagementCoordinator::~ZoneManagementCoordinator()
{
    qDebug() << "ZoneManagementCoordinator destroyed";
}

void ZoneManagementCoordinator::setViewModel(ZoneDefinitionViewModel* viewModel)
{
    m_viewModel = viewModel;

    // Share ViewModel with all controllers
    if (m_safetyZoneController) {
        m_safetyZoneController->setViewModel(viewModel);
    }
    if (m_sectorScanController) {
        m_sectorScanController->setViewModel(viewModel);
    }
    if (m_trpController) {
        m_trpController->setViewModel(viewModel);
    }
}

void ZoneManagementCoordinator::setMapViewModel(ZoneMapViewModel* mapViewModel)
{
    m_mapViewModel = mapViewModel;

    // Share MapViewModel with all controllers
    if (m_safetyZoneController) {
        m_safetyZoneController->setMapViewModel(mapViewModel);
    }
    if (m_sectorScanController) {
        m_sectorScanController->setMapViewModel(mapViewModel);
    }
    if (m_trpController) {
        m_trpController->setMapViewModel(mapViewModel);
    }
}

void ZoneManagementCoordinator::setParameterViewModels(AreaZoneParameterViewModel* areaVM,
                                                       SectorScanParameterViewModel* sectorVM,
                                                       TRPParameterViewModel* trpVM)
{
    m_areaZoneParamViewModel = areaVM;
    m_sectorScanParamViewModel = sectorVM;
    m_trpParamViewModel = trpVM;

    // Assign parameter ViewModels to respective controllers
    if (m_safetyZoneController) {
        m_safetyZoneController->setParameterViewModel(areaVM);
    }
    if (m_sectorScanController) {
        m_sectorScanController->setParameterViewModel(sectorVM);
    }
    if (m_trpController) {
        m_trpController->setParameterViewModel(trpVM);
    }
}

void ZoneManagementCoordinator::setStateModel(SystemStateModel* stateModel)
{
    m_stateModel = stateModel;

    // Share StateModel with all controllers
    if (m_safetyZoneController) {
        m_safetyZoneController->setStateModel(stateModel);
    }
    if (m_sectorScanController) {
        m_sectorScanController->setStateModel(stateModel);
    }
    if (m_trpController) {
        m_trpController->setStateModel(stateModel);
    }
}

void ZoneManagementCoordinator::initialize()
{
    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_mapViewModel);
    Q_ASSERT(m_areaZoneParamViewModel);
    Q_ASSERT(m_sectorScanParamViewModel);
    Q_ASSERT(m_trpParamViewModel);
    Q_ASSERT(m_stateModel);

    // Initialize all specialized controllers
    m_safetyZoneController->initialize();
    m_sectorScanController->initialize();
    m_trpController->initialize();

    qDebug() << "ZoneManagementCoordinator initialized with all controllers";
}

// ============================================================================
// MAIN INTERFACE (QML entry points)
// ============================================================================

void ZoneManagementCoordinator::show()
{
    qDebug() << "ZoneManagementCoordinator: show()";

    m_currentState = CoordinatorState::SelectingZoneType;
    m_selectedZoneType = ZoneType::None;

    // Show zone type selection menu
    setupZoneTypeSelectionUI();

    // Make UI visible
    m_viewModel->setVisible(true);

    // Update map with current zones
    m_mapViewModel->updateZones(m_stateModel);
}

void ZoneManagementCoordinator::hide()
{
    qDebug() << "ZoneManagementCoordinator: hide()";

    // Deactivate all controllers
    deactivateAllControllers();

    // Clear UI
    m_viewModel->setVisible(false);
    m_mapViewModel->clearWipZone();

    m_currentState = CoordinatorState::Idle;
}

// ============================================================================
// INPUT ROUTING
// ============================================================================

void ZoneManagementCoordinator::onUpButtonPressed()
{
    switch (m_currentState) {
        case CoordinatorState::SelectingZoneType:
            // Navigate zone type menu
            m_zoneTypeMenuIndex--;
            if (m_zoneTypeMenuIndex < 0) {
                m_zoneTypeMenuIndex = m_zoneTypeMenuItems.size() - 1;
            }
            m_viewModel->setCurrentMenuIndex(m_zoneTypeMenuIndex);
            break;

        case CoordinatorState::ManagingSafetyZones:
            m_safetyZoneController->onUpButtonPressed();
            break;

        case CoordinatorState::ManagingSectorScans:
            m_sectorScanController->onUpButtonPressed();
            break;

        case CoordinatorState::ManagingTRPs:
            m_trpController->onUpButtonPressed();
            break;

        default:
            break;
    }
}

void ZoneManagementCoordinator::onDownButtonPressed()
{
    switch (m_currentState) {
        case CoordinatorState::SelectingZoneType:
            // Navigate zone type menu
            m_zoneTypeMenuIndex++;
            if (m_zoneTypeMenuIndex >= m_zoneTypeMenuItems.size()) {
                m_zoneTypeMenuIndex = 0;
            }
            m_viewModel->setCurrentMenuIndex(m_zoneTypeMenuIndex);
            break;

        case CoordinatorState::ManagingSafetyZones:
            m_safetyZoneController->onDownButtonPressed();
            break;

        case CoordinatorState::ManagingSectorScans:
            m_sectorScanController->onDownButtonPressed();
            break;

        case CoordinatorState::ManagingTRPs:
            m_trpController->onDownButtonPressed();
            break;

        default:
            break;
    }
}

void ZoneManagementCoordinator::onMenuValButtonPressed()
{
    switch (m_currentState) {
        case CoordinatorState::SelectingZoneType:
            selectZoneType();
            break;

        case CoordinatorState::ManagingSafetyZones:
            m_safetyZoneController->onMenuValButtonPressed();
            break;

        case CoordinatorState::ManagingSectorScans:
            m_sectorScanController->onMenuValButtonPressed();
            break;

        case CoordinatorState::ManagingTRPs:
            m_trpController->onMenuValButtonPressed();
            break;

        default:
            break;
    }
}

// ============================================================================
// ZONE TYPE SELECTION
// ============================================================================

void ZoneManagementCoordinator::selectZoneType()
{
    QString selection = m_zoneTypeMenuItems[m_zoneTypeMenuIndex];

    if (selection == "Safety Zones (NoFire/NoTraverse)") {
        transitionToController(ZoneType::AreaZone);
    } else if (selection == "Auto Sector Scan Zones") {
        transitionToController(ZoneType::AutoSectorScanZone);
    } else if (selection == "Target Reference Points (TRP)") {
        transitionToController(ZoneType::TargetReferencePoint);
    } else if (selection == "Exit") {
        hide();
        emit closed();
    }
}

void ZoneManagementCoordinator::transitionToController(ZoneType type)
{
    qDebug() << "Transitioning to controller for zone type:" << (int)type;

    deactivateAllControllers();
    m_selectedZoneType = type;

    switch (type) {
        case ZoneType::AreaZone:
            m_currentState = CoordinatorState::ManagingSafetyZones;
            m_safetyZoneController->show();
            break;

        case ZoneType::AutoSectorScanZone:
            m_currentState = CoordinatorState::ManagingSectorScans;
            m_sectorScanController->show();
            break;

        case ZoneType::TargetReferencePoint:
            m_currentState = CoordinatorState::ManagingTRPs;
            m_trpController->show();
            break;

        default:
            qWarning() << "Unknown zone type:" << (int)type;
            setupZoneTypeSelectionUI();
            break;
    }
}

// ============================================================================
// CONTROLLER LIFECYCLE
// ============================================================================

void ZoneManagementCoordinator::onSafetyZoneFinished()
{
    qDebug() << "Safety zone controller finished";
    m_safetyZoneController->hide();

    // Return to zone type selection
    m_currentState = CoordinatorState::SelectingZoneType;
    setupZoneTypeSelectionUI();
}

void ZoneManagementCoordinator::onSectorScanFinished()
{
    qDebug() << "Sector scan controller finished";
    m_sectorScanController->hide();

    // Return to zone type selection
    m_currentState = CoordinatorState::SelectingZoneType;
    setupZoneTypeSelectionUI();
}

void ZoneManagementCoordinator::onTRPFinished()
{
    qDebug() << "TRP controller finished";
    m_trpController->hide();

    // Return to zone type selection
    m_currentState = CoordinatorState::SelectingZoneType;
    setupZoneTypeSelectionUI();
}

void ZoneManagementCoordinator::deactivateAllControllers()
{
    if (m_safetyZoneController && m_safetyZoneController->isActive()) {
        m_safetyZoneController->hide();
    }
    if (m_sectorScanController && m_sectorScanController->isActive()) {
        m_sectorScanController->hide();
    }
    if (m_trpController && m_trpController->isActive()) {
        m_trpController->hide();
    }
}

// ============================================================================
// UI SETUP
// ============================================================================

void ZoneManagementCoordinator::setupZoneTypeSelectionUI()
{
    m_zoneTypeMenuIndex = 0;

    m_viewModel->setTitle("Zone Management");
    m_viewModel->setInstructionText("Select zone type to manage:");
    m_viewModel->setShowMenu(true);
    m_viewModel->setShowParameterPanel(false);
    m_viewModel->setShowConfirmButtons(false);
    m_viewModel->setMenuItems(m_zoneTypeMenuItems);
    m_viewModel->setCurrentMenuIndex(m_zoneTypeMenuIndex);

    m_currentState = CoordinatorState::SelectingZoneType;
}
