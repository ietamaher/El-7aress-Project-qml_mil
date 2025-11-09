#include "SafetyZoneController.h"
#include "models/areazoneparameterviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "services/zonegeometryservice.h"
#include "services/servicemanager.h"
#include <QDebug>
#include <QtMath>

SafetyZoneController::SafetyZoneController(QObject* parent)
    : BaseZoneController(parent)
    , m_paramViewModel(nullptr)
    , m_editingZoneId(-1)
    , m_isModifying(false)
    , m_corner1Defined(false)
    , m_corner1Az(0.0f), m_corner1El(0.0f)
    , m_corner2Az(0.0f), m_corner2El(0.0f)
    , m_geometryService(nullptr)
{
    resetWipZone();
}

void SafetyZoneController::setParameterViewModel(AreaZoneParameterViewModel* paramViewModel)
{
    m_paramViewModel = paramViewModel;
}

void SafetyZoneController::initialize()
{
    BaseZoneController::initialize();

    Q_ASSERT(m_paramViewModel);

    // Get ZoneGeometryService
    m_geometryService = ServiceManager::instance()->get<ZoneGeometryService>();
    if (!m_geometryService) {
        m_geometryService = new ZoneGeometryService(this);
        ServiceManager::instance()->registerService(m_geometryService);
    }

    qDebug() << "SafetyZoneController initialized";
}

void SafetyZoneController::show()
{
    BaseZoneController::show();
    setupSelectActionUI();
}

// ============================================================================
// INPUT HANDLING - MenuVal Button
// ============================================================================

void SafetyZoneController::onMenuValButtonPressed()
{
    if (!isActive()) return;

    switch (currentState()) {
        case State::SelectAction:
            handleSelectActionInput();
            break;

        case State::SelectExistingZone:
            handleSelectExistingZoneInput();
            break;

        case State::AimingPoint:
            // Check which corner we're aiming
            if (!m_corner1Defined) {
                handleAimingCorner1Input();
            } else {
                handleAimingCorner2Input();
            }
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
            // Return to action selection
            setupSelectActionUI();
            break;

        default:
            qWarning() << "Unhandled MenuVal in state" << (int)currentState();
            break;
    }
}

void SafetyZoneController::onUpButtonPressed()
{
    if (!isActive()) return;

    if (currentState() == State::EditParameters) {
        routeUpToParameterPanel();
    } else {
        BaseZoneController::onUpButtonPressed();
    }
}

void SafetyZoneController::onDownButtonPressed()
{
    if (!isActive()) return;

    if (currentState() == State::EditParameters) {
        routeDownToParameterPanel();
    } else {
        BaseZoneController::onDownButtonPressed();
    }
}

// ============================================================================
// STATE-SPECIFIC INPUT HANDLERS
// ============================================================================

void SafetyZoneController::handleSelectActionInput()
{
    QString action = selectedMenuItem();

    if (action == "New Zone") {
        createNewZone();
    } else if (action == "Modify Zone") {
        setupSelectExistingZoneUI("Modify");
        m_isModifying = true;
    } else if (action == "Delete Zone") {
        setupSelectExistingZoneUI("Delete");
        m_isModifying = false;
    } else if (action == "Exit") {
        hide();
        emit finished();
    }
}

void SafetyZoneController::handleSelectExistingZoneInput()
{
    int zoneId = getZoneIdFromMenuIndex(currentMenuIndex());

    if (zoneId < 0) {
        showErrorMessage("Invalid zone selection");
        return;
    }

    if (m_isModifying) {
        loadZoneForModification(zoneId);
    } else {
        // Delete mode - show confirmation
        m_editingZoneId = zoneId;
        setupConfirmUI("Confirm Delete",
                      QString("Delete zone '%1'?").arg(m_wipZone.name));
        transitionToState(State::ConfirmDelete);
    }
}

void SafetyZoneController::handleAimingCorner1Input()
{
    // Capture current gimbal position as corner 1
    m_corner1Az = currentGimbalAz();
    m_corner1El = currentGimbalEl();
    m_corner1Defined = true;

    qDebug() << "Corner 1 captured:" << m_corner1Az << "," << m_corner1El;

    // Move to corner 2 aiming
    setupAimingCorner2UI();
}

void SafetyZoneController::handleAimingCorner2Input()
{
    // Capture current gimbal position as corner 2
    m_corner2Az = currentGimbalAz();
    m_corner2El = currentGimbalEl();

    qDebug() << "Corner 2 captured:" << m_corner2Az << "," << m_corner2El;

    // Calculate zone geometry
    calculateZoneGeometry();

    // Validate geometry
    validateZoneGeometry();

    // Move to parameter editing
    setupEditParametersUI();
}

void SafetyZoneController::handleEditParametersInput()
{
    // Sync parameters from UI to WIP zone
    syncParameterPanelToWipZone();

    // Show save confirmation
    setupConfirmUI("Confirm Save",
                  QString("Save zone '%1'?").arg(m_wipZone.name));
    transitionToState(State::ConfirmSave);
}

void SafetyZoneController::handleConfirmSaveInput()
{
    if (saveCurrentZone()) {
        QString msg = (m_editingZoneId < 0) ?
            "Zone created successfully" : "Zone modified successfully";
        showSuccessMessage(msg);

        if (m_editingZoneId < 0) {
            emit zoneCreated(ZoneType::AreaZone);
        } else {
            emit zoneModified(ZoneType::AreaZone, m_editingZoneId);
        }

        resetWipZone();
    } else {
        showErrorMessage("Failed to save zone");
    }
}

void SafetyZoneController::handleConfirmDeleteInput()
{
    performZoneDeletion(m_editingZoneId);
    showSuccessMessage("Zone deleted successfully");
    emit zoneDeleted(ZoneType::AreaZone, m_editingZoneId);
    resetWipZone();
}

// ============================================================================
// ABSTRACT METHOD IMPLEMENTATIONS
// ============================================================================

void SafetyZoneController::createNewZone()
{
    qDebug() << "SafetyZoneController: Creating new zone";

    resetWipZone();
    m_editingZoneId = -1;
    m_corner1Defined = false;

    // Assign next available ID
    const auto& data = stateModel()->data();
    int maxId = 0;
    for (const auto& zone : data.areaZones) {
        if (zone.id > maxId) maxId = zone.id;
    }
    m_wipZone.id = maxId + 1;
    m_wipZone.name = QString("Zone %1").arg(m_wipZone.id);

    setupAimingCorner1UI();
}

void SafetyZoneController::loadZoneForModification(int zoneId)
{
    qDebug() << "SafetyZoneController: Loading zone" << zoneId << "for modification";

    loadWipZoneFromSystem(zoneId);
    m_editingZoneId = zoneId;

    // Skip aiming - go directly to parameter editing
    syncWipZoneToParameterPanel();
    setupEditParametersUI();
}

void SafetyZoneController::performZoneDeletion(int zoneId)
{
    qDebug() << "SafetyZoneController: Deleting zone" << zoneId;

    // Remove zone from system state
    stateModel()->removeAreaZone(zoneId);
}

bool SafetyZoneController::saveCurrentZone()
{
    qDebug() << "SafetyZoneController: Saving zone" << m_wipZone.id;

    // Sync final parameters from UI
    syncParameterPanelToWipZone();

    // Validate zone
    if (m_wipZone.name.isEmpty()) {
        showErrorMessage("Zone name cannot be empty");
        return false;
    }

    if (m_wipZone.widthMeters <= 0.0f || m_wipZone.heightMeters <= 0.0f) {
        showErrorMessage("Invalid zone dimensions");
        return false;
    }

    // Save to system state
    if (m_editingZoneId < 0) {
        // New zone
        stateModel()->addAreaZone(m_wipZone);
    } else {
        // Update existing zone
        stateModel()->updateAreaZone(m_editingZoneId, m_wipZone);
    }

    return true;
}

void SafetyZoneController::updateWipZoneVisualization()
{
    if (!m_corner1Defined) {
        // No corner defined yet
        mapViewModel()->clearWipZone();
        return;
    }

    if (!m_corner1Defined || (m_corner2Az == 0.0f && m_corner2El == 0.0f)) {
        // Only corner 1 defined - show a point
        // (You could show a line from corner 1 to current gimbal position)
        mapViewModel()->clearWipZone();
        return;
    }

    // Both corners defined - show WIP zone
    mapViewModel()->setWipAreaZone(m_wipZone);
}

QStringList SafetyZoneController::getExistingZoneNames() const
{
    QStringList names;
    const auto& data = stateModel()->data();

    for (const auto& zone : data.areaZones) {
        QString typeStr = (zone.type == AreaZone::Type::NoFire) ? "NoFire" : "NoTraverse";
        names << QString("%1 (%2)").arg(zone.name, typeStr);
    }

    if (names.isEmpty()) {
        names << "(No zones defined)";
    }

    return names;
}

int SafetyZoneController::getZoneIdFromMenuIndex(int menuIndex) const
{
    const auto& data = stateModel()->data();

    if (menuIndex < 0 || menuIndex >= data.areaZones.size()) {
        return -1;
    }

    return data.areaZones[menuIndex].id;
}

// ============================================================================
// UI SETUP
// ============================================================================

void SafetyZoneController::setupSelectActionUI()
{
    QStringList actions = {"New Zone", "Modify Zone", "Delete Zone", "Exit"};
    setupMenuUI("Safety Zone Management", actions);
    transitionToState(State::SelectAction);
}

void SafetyZoneController::setupSelectExistingZoneUI(const QString& action)
{
    QStringList zones = getExistingZoneNames();
    setupMenuUI(QString("%1 Safety Zone").arg(action), zones);
    transitionToState(State::SelectExistingZone);
}

void SafetyZoneController::setupAimingCorner1UI()
{
    viewModel()->setTitle("Aim Corner 1");
    viewModel()->setInstruction("Point gimbal at first corner, then press VAL");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(false);
    viewModel()->setShowConfirmDialog(false);
    transitionToState(State::AimingPoint);
}

void SafetyZoneController::setupAimingCorner2UI()
{
    viewModel()->setTitle("Aim Corner 2");
    viewModel()->setInstruction("Point gimbal at second corner, then press VAL");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(false);
    viewModel()->setShowConfirmDialog(false);
    // State remains AimingPoint
}

void SafetyZoneController::setupEditParametersUI()
{
    viewModel()->setTitle("Edit Zone Parameters");
    viewModel()->setInstruction("Use UP/DOWN to navigate, VAL to confirm");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(true);
    viewModel()->setShowConfirmDialog(false);

    // Sync WIP zone to parameter panel
    syncWipZoneToParameterPanel();

    transitionToState(State::EditParameters);
}

// ============================================================================
// PARAMETER PANEL ROUTING
// ============================================================================

void SafetyZoneController::routeUpToParameterPanel()
{
    m_paramViewModel->navigateUp();
}

void SafetyZoneController::routeDownToParameterPanel()
{
    m_paramViewModel->navigateDown();
}

void SafetyZoneController::routeSelectToParameterPanel()
{
    m_paramViewModel->confirmSelection();
}

// ============================================================================
// GEOMETRY CALCULATION
// ============================================================================

void SafetyZoneController::calculateZoneGeometry()
{
    qDebug() << "Calculating zone geometry...";
    qDebug() << "  Corner 1:" << m_corner1Az << "," << m_corner1El;
    qDebug() << "  Corner 2:" << m_corner2Az << "," << m_corner2El;

    // Use ZoneGeometryService for calculation
    if (m_geometryService) {
        m_wipZone = m_geometryService->calculateAreaZoneFromCorners(
            m_corner1Az, m_corner1El,
            m_corner2Az, m_corner2El
        );
    } else {
        // Fallback: simple calculation
        m_wipZone.centerAzimuth = (m_corner1Az + m_corner2Az) / 2.0f;
        m_wipZone.centerElevation = (m_corner1El + m_corner2El) / 2.0f;

        float azDiff = std::abs(m_corner2Az - m_corner1Az);
        float elDiff = std::abs(m_corner2El - m_corner1El);

        // Estimate dimensions (rough approximation)
        // In real system, use proper geodetic calculations
        m_wipZone.widthMeters = azDiff * 10.0f;  // Placeholder
        m_wipZone.heightMeters = elDiff * 10.0f; // Placeholder
        m_wipZone.rotationAngle = 0.0f;
    }

    qDebug() << "  Calculated center:" << m_wipZone.centerAzimuth << "," << m_wipZone.centerElevation;
    qDebug() << "  Dimensions:" << m_wipZone.widthMeters << "x" << m_wipZone.heightMeters << "m";

    updateWipZoneVisualization();
}

void SafetyZoneController::validateZoneGeometry()
{
    // Ensure dimensions are reasonable
    if (m_wipZone.widthMeters < 1.0f) {
        qWarning() << "Zone width too small, adjusting to minimum";
        m_wipZone.widthMeters = 1.0f;
    }

    if (m_wipZone.heightMeters < 1.0f) {
        qWarning() << "Zone height too small, adjusting to minimum";
        m_wipZone.heightMeters = 1.0f;
    }

    // Normalize angles
    m_wipZone.centerAzimuth = normalizeAzimuthTo360(m_wipZone.centerAzimuth);
    m_wipZone.centerElevation = normalizeElevation(m_wipZone.centerElevation);
}

// ============================================================================
// WIP ZONE MANAGEMENT
// ============================================================================

void SafetyZoneController::resetWipZone()
{
    m_wipZone = AreaZone{};
    m_wipZone.enabled = true;
    m_wipZone.type = AreaZone::Type::NoFire;  // Default to most restrictive
    m_editingZoneId = -1;
    m_corner1Defined = false;
    m_corner1Az = m_corner1El = 0.0f;
    m_corner2Az = m_corner2El = 0.0f;
}

void SafetyZoneController::loadWipZoneFromSystem(int zoneId)
{
    const auto& data = stateModel()->data();

    for (const auto& zone : data.areaZones) {
        if (zone.id == zoneId) {
            m_wipZone = zone;
            qDebug() << "Loaded zone" << zoneId << ":" << zone.name;
            return;
        }
    }

    qWarning() << "Zone" << zoneId << "not found!";
    resetWipZone();
}

void SafetyZoneController::syncWipZoneToParameterPanel()
{
    m_paramViewModel->setZoneName(m_wipZone.name);
    m_paramViewModel->setZoneType(m_wipZone.type);
    m_paramViewModel->setEnabled(m_wipZone.enabled);
    m_paramViewModel->setWidth(m_wipZone.widthMeters);
    m_paramViewModel->setHeight(m_wipZone.heightMeters);
    m_paramViewModel->setCenterAz(m_wipZone.centerAzimuth);
    m_paramViewModel->setCenterEl(m_wipZone.centerElevation);
    m_paramViewModel->setRotation(m_wipZone.rotationAngle);
}

void SafetyZoneController::syncParameterPanelToWipZone()
{
    m_wipZone.name = m_paramViewModel->zoneName();
    m_wipZone.type = m_paramViewModel->zoneType();
    m_wipZone.enabled = m_paramViewModel->enabled();
    m_wipZone.widthMeters = m_paramViewModel->width();
    m_wipZone.heightMeters = m_paramViewModel->height();
    m_wipZone.centerAzimuth = m_paramViewModel->centerAz();
    m_wipZone.centerElevation = m_paramViewModel->centerEl();
    m_wipZone.rotationAngle = m_paramViewModel->rotation();
}
