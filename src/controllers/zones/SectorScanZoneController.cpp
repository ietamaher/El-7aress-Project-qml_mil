#include "SectorScanZoneController.h"
#include "models/sectorscanparameterviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include <QDebug>
#include <QtMath>

SectorScanZoneController::SectorScanZoneController(QObject* parent)
    : BaseZoneController(parent)
    , m_paramViewModel(nullptr)
    , m_editingZoneId(-1)
    , m_isModifying(false)
    , m_point1Defined(false)
    , m_point1Az(0.0f), m_point1El(0.0f)
    , m_point2Az(0.0f), m_point2El(0.0f)
{
    resetWipZone();
}

void SectorScanZoneController::setParameterViewModel(SectorScanParameterViewModel* paramViewModel)
{
    m_paramViewModel = paramViewModel;
}

void SectorScanZoneController::initialize()
{
    BaseZoneController::initialize();
    Q_ASSERT(m_paramViewModel);
    qDebug() << "SectorScanZoneController initialized";
}

void SectorScanZoneController::show()
{
    BaseZoneController::show();
    setupSelectActionUI();
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

void SectorScanZoneController::onMenuValButtonPressed()
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
            if (!m_point1Defined) {
                handleAimingPoint1Input();
            } else {
                handleAimingPoint2Input();
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
            setupSelectActionUI();
            break;
        default:
            qWarning() << "Unhandled MenuVal in state" << (int)currentState();
            break;
    }
}

void SectorScanZoneController::onUpButtonPressed()
{
    if (!isActive()) return;

    if (currentState() == State::EditParameters) {
        routeUpToParameterPanel();
    } else {
        BaseZoneController::onUpButtonPressed();
    }
}

void SectorScanZoneController::onDownButtonPressed()
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

void SectorScanZoneController::handleSelectActionInput()
{
    QString action = selectedMenuItem();

    if (action == "New Scan Zone") {
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

void SectorScanZoneController::handleSelectExistingZoneInput()
{
    int zoneId = getZoneIdFromMenuIndex(currentMenuIndex());

    if (zoneId < 0) {
        showErrorMessage("Invalid zone selection");
        return;
    }

    if (m_isModifying) {
        loadZoneForModification(zoneId);
    } else {
        m_editingZoneId = zoneId;
        loadWipZoneFromSystem(zoneId);
        setupConfirmUI("Confirm Delete",
                      QString("Delete scan zone #%1?").arg(m_wipZone.id));
        transitionToState(State::ConfirmDelete);
    }
}

void SectorScanZoneController::handleAimingPoint1Input()
{
    m_point1Az = currentGimbalAz();
    m_point1El = currentGimbalEl();
    m_point1Defined = true;

    qDebug() << "Scan start point captured:" << m_point1Az << "," << m_point1El;
    setupAimingPoint2UI();
}

void SectorScanZoneController::handleAimingPoint2Input()
{
    m_point2Az = currentGimbalAz();
    m_point2El = currentGimbalEl();

    qDebug() << "Scan end point captured:" << m_point2Az << "," << m_point2El;

    calculateSectorGeometry();
    validateSectorGeometry();
    setupEditParametersUI();
}

void SectorScanZoneController::handleEditParametersInput()
{
    syncParameterPanelToWipZone();
    setupConfirmUI("Confirm Save",
                  QString("Save scan zone #%1?").arg(m_wipZone.id));
    transitionToState(State::ConfirmSave);
}

void SectorScanZoneController::handleConfirmSaveInput()
{
    if (saveCurrentZone()) {
        QString msg = (m_editingZoneId < 0) ?
            "Scan zone created successfully" : "Scan zone modified successfully";
        showSuccessMessage(msg);

        if (m_editingZoneId < 0) {
            emit zoneCreated(ZoneType::AutoSectorScan);
        } else {
            emit zoneModified(ZoneType::AutoSectorScan, m_editingZoneId);
        }

        resetWipZone();
    } else {
        showErrorMessage("Failed to save scan zone");
    }
}

void SectorScanZoneController::handleConfirmDeleteInput()
{
    performZoneDeletion(m_editingZoneId);
    showSuccessMessage("Scan zone deleted successfully");
    emit zoneDeleted(ZoneType::AutoSectorScan, m_editingZoneId);
    resetWipZone();
}

// ============================================================================
// ABSTRACT METHOD IMPLEMENTATIONS
// ============================================================================

void SectorScanZoneController::createNewZone()
{
    qDebug() << "SectorScanZoneController: Creating new scan zone";

    resetWipZone();
    m_editingZoneId = -1;
    m_point1Defined = false;

    // Assign next available ID
    const auto& data = stateModel()->data();
    int maxId = 0;
    for (const auto& zone : data.sectorScanZones) {
        if (zone.id > maxId) maxId = zone.id;
    }
    m_wipZone.id = maxId + 1;

    setupAimingPoint1UI();
}

void SectorScanZoneController::loadZoneForModification(int zoneId)
{
    qDebug() << "SectorScanZoneController: Loading zone" << zoneId;

    loadWipZoneFromSystem(zoneId);
    m_editingZoneId = zoneId;

    syncWipZoneToParameterPanel();
    setupEditParametersUI();
}

void SectorScanZoneController::performZoneDeletion(int zoneId)
{
    qDebug() << "SectorScanZoneController: Deleting zone" << zoneId;
    stateModel()->deleteSectorScanZone(zoneId);
}

bool SectorScanZoneController::saveCurrentZone()
{
    qDebug() << "SectorScanZoneController: Saving zone" << m_wipZone.id;

    syncParameterPanelToWipZone();

    // Validate
    if (m_wipZone.scanSpeed <= 0.0f) {
        showErrorMessage("Scan rate must be positive");
        return false;
    }

    // Save to system state
    if (m_editingZoneId < 0) {
        stateModel()->addSectorScanZone(m_wipZone);
    } else {
        stateModel()->modifySectorScanZone(m_editingZoneId, m_wipZone);
    }

    return true;
}

void SectorScanZoneController::updateWipZoneVisualization()
{
    if (!m_point1Defined) {
        mapViewModel()->clearWipZone();
        return;
    }

    // Convert AutoSectorScanZone to QVariantMap for visualization
    QVariantMap zoneMap;
    zoneMap["id"] = m_wipZone.id;
    zoneMap["isEnabled"] = m_wipZone.isEnabled;
    zoneMap["az1"] = m_wipZone.az1;
    zoneMap["el1"] = m_wipZone.el1;
    zoneMap["az2"] = m_wipZone.az2;
    zoneMap["el2"] = m_wipZone.el2;
    zoneMap["scanSpeed"] = m_wipZone.scanSpeed;

    // definingStart = point1 being defined, definingEnd = point2 being defined
    bool definingStart = !m_point1Defined;
    bool definingEnd = m_point1Defined && !m_point2Az && !m_point2El;

    mapViewModel()->setWipZone(zoneMap, static_cast<int>(ZoneType::AutoSectorScan), definingStart, definingEnd);
}

QStringList SectorScanZoneController::getExistingZoneNames() const
{
    QStringList names;
    const auto& data = stateModel()->data();

    for (const auto& zone : data.sectorScanZones) {
        names << QString("%1 (%.1f°-%.1f°)")
                     .arg(zone.name)
                     .arg(zone.startAzimuth)
                     .arg(zone.endAzimuth);
    }

    if (names.isEmpty()) {
        names << "(No scan zones defined)";
    }

    return names;
}

int SectorScanZoneController::getZoneIdFromMenuIndex(int menuIndex) const
{
    const auto& data = stateModel()->data();

    if (menuIndex < 0 || menuIndex >= data.sectorScanZones.size()) {
        return -1;
    }

    return data.sectorScanZones[menuIndex].id;
}

// ============================================================================
// UI SETUP
// ============================================================================

void SectorScanZoneController::setupSelectActionUI()
{
    QStringList actions = {"New Scan Zone", "Modify Zone", "Delete Zone", "Exit"};
    setupMenuUI("Auto Sector Scan Management", actions);
    transitionToState(State::SelectAction);
}

void SectorScanZoneController::setupSelectExistingZoneUI(const QString& action)
{
    QStringList zones = getExistingZoneNames();
    setupMenuUI(QString("%1 Scan Zone").arg(action), zones);
    transitionToState(State::SelectExistingZone);
}

void SectorScanZoneController::setupAimingPoint1UI()
{
    viewModel()->setTitle("Aim Scan Start");
    viewModel()->setInstruction("Point gimbal at scan start position, then press VAL");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(false);
    viewModel()->setShowConfirmDialog(false);
    transitionToState(State::AimingPoint);
}

void SectorScanZoneController::setupAimingPoint2UI()
{
    viewModel()->setTitle("Aim Scan End");
    viewModel()->setInstruction("Point gimbal at scan end position, then press VAL");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(false);
    viewModel()->setShowConfirmDialog(false);
}

void SectorScanZoneController::setupEditParametersUI()
{
    viewModel()->setTitle("Edit Scan Parameters");
    viewModel()->setInstruction("Use UP/DOWN to navigate, VAL to confirm");
    viewModel()->setShowMainMenu(false);
    viewModel()->setShowParameterPanel(true);
    viewModel()->setShowConfirmDialog(false);

    syncWipZoneToParameterPanel();
    transitionToState(State::EditParameters);
}

// ============================================================================
// PARAMETER PANEL ROUTING
// ============================================================================

void SectorScanZoneController::routeUpToParameterPanel()
{
    int currentField = m_paramViewModel->activeField();
    currentField = (currentField - 1 + 4) % 4; // 4 fields total (Enabled, ScanSpeed, ValidateButton, CancelButton)
    m_paramViewModel->setActiveField(currentField);
}

void SectorScanZoneController::routeDownToParameterPanel()
{
    int currentField = m_paramViewModel->activeField();
    currentField = (currentField + 1) % 4; // 4 fields total
    m_paramViewModel->setActiveField(currentField);
}

void SectorScanZoneController::routeSelectToParameterPanel()
{
    int currentField = m_paramViewModel->activeField();

    if (currentField == SectorScanParameterViewModel::Field::ValidateButton) {
        // Confirm save
        transitionToState(State::ConfirmSave);
    } else if (currentField == SectorScanParameterViewModel::Field::CancelButton) {
        // Cancel and return to select action
        resetWipZone();
        setupSelectActionUI();
    } else if (currentField == SectorScanParameterViewModel::Field::Enabled) {
        // Toggle enabled state
        m_paramViewModel->setIsEnabled(!m_paramViewModel->isEnabled());
    } else if (currentField == SectorScanParameterViewModel::Field::ScanSpeed) {
        // Start editing scan speed value
        m_paramViewModel->setIsEditingValue(true);
    }
}

// ============================================================================
// GEOMETRY CALCULATION
// ============================================================================

void SectorScanZoneController::calculateSectorGeometry()
{
    qDebug() << "Calculating sector geometry...";

    m_wipZone.startAzimuth = m_point1Az;
    m_wipZone.endAzimuth = m_point2Az;
    m_wipZone.scanElevation = (m_point1El + m_point2El) / 2.0f;

    // Calculate span
    float span = std::abs(m_wipZone.endAzimuth - m_wipZone.startAzimuth);
    if (span > 180.0f) {
        span = 360.0f - span;
    }

    qDebug() << "  Start:" << m_wipZone.startAzimuth << "End:" << m_wipZone.endAzimuth;
    qDebug() << "  Span:" << span << "degrees";

    updateWipZoneVisualization();
}

void SectorScanZoneController::validateSectorGeometry()
{
    // Normalize angles
    m_wipZone.startAzimuth = normalizeAzimuthTo360(m_wipZone.startAzimuth);
    m_wipZone.endAzimuth = normalizeAzimuthTo360(m_wipZone.endAzimuth);
    m_wipZone.scanElevation = normalizeElevation(m_wipZone.scanElevation);

    // Ensure reasonable scan rate
    if (m_wipZone.scanRateDegreesPerSecond <= 0.0f) {
        m_wipZone.scanRateDegreesPerSecond = 5.0f;  // Default 5 deg/sec
    }
}

// ============================================================================
// WIP ZONE MANAGEMENT
// ============================================================================

void SectorScanZoneController::resetWipZone()
{
    m_wipZone = AutoSectorScanZone{};
    m_wipZone.isEnabled = true;
    m_wipZone.scanSpeed = 5.0f;
    m_editingZoneId = -1;
    m_point1Defined = false;
    m_point1Az = m_point1El = 0.0f;
    m_point2Az = m_point2El = 0.0f;
}

void SectorScanZoneController::loadWipZoneFromSystem(int zoneId)
{
    const auto& data = stateModel()->data();

    for (const auto& zone : data.sectorScanZones) {
        if (zone.id == zoneId) {
            m_wipZone = zone;
            qDebug() << "Loaded scan zone #" << zoneId;
            return;
        }
    }

    qWarning() << "Scan zone" << zoneId << "not found!";
    resetWipZone();
}

void SectorScanZoneController::syncWipZoneToParameterPanel()
{
    m_paramViewModel->setIsEnabled(m_wipZone.isEnabled);
    m_paramViewModel->setScanSpeed(static_cast<int>(m_wipZone.scanSpeed));
}

void SectorScanZoneController::syncParameterPanelToWipZone()
{
    m_wipZone.isEnabled = m_paramViewModel->isEnabled();
    m_wipZone.scanSpeed = static_cast<float>(m_paramViewModel->scanSpeed());
}
