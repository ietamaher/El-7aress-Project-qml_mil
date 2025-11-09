#include "BaseZoneController.h"
#include "models/zonedefinitionviewmodel.h"
#include "models/zonemapviewmodel.h"
#include "models/domain/systemstatemodel.h"
#include <QDebug>
#include <QtMath>

BaseZoneController::BaseZoneController(QObject* parent)
    : QObject(parent)
    , m_currentState(State::Idle)
    , m_isActive(false)
    , m_viewModel(nullptr)
    , m_mapViewModel(nullptr)
    , m_stateModel(nullptr)
    , m_currentMenuIndex(0)
    , m_currentGimbalAz(0.0f)
    , m_currentGimbalEl(0.0f)
{
}

void BaseZoneController::setViewModel(ZoneDefinitionViewModel* viewModel)
{
    m_viewModel = viewModel;
}

void BaseZoneController::setMapViewModel(ZoneMapViewModel* mapViewModel)
{
    m_mapViewModel = mapViewModel;
}

void BaseZoneController::setStateModel(SystemStateModel* stateModel)
{
    m_stateModel = stateModel;
}

void BaseZoneController::initialize()
{
    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_mapViewModel);
    Q_ASSERT(m_stateModel);

    // Connect to model signals
    connect(m_stateModel, &SystemStateModel::gimbalPositionChanged,
            this, &BaseZoneController::onGimbalPositionChanged);
    connect(m_stateModel, &SystemStateModel::zonesChanged,
            this, &BaseZoneController::onZonesChanged);
    connect(m_stateModel, &SystemStateModel::colorStyleChanged,
            this, &BaseZoneController::onColorStyleChanged);

    // Initialize gimbal position
    const auto& data = m_stateModel->data();
    m_currentGimbalAz = data.gimbalAz;
    m_currentGimbalEl = data.gimbalEl;

    qDebug() << zoneTypeName() << "Controller initialized";
}

void BaseZoneController::show()
{
    qDebug() << zoneTypeName() << "Controller: show()";
    m_isActive = true;
    m_currentState = State::Idle;

    // Update gimbal position
    const auto& data = m_stateModel->data();
    m_currentGimbalAz = data.gimbalAz;
    m_currentGimbalEl = data.gimbalEl;
    m_viewModel->setGimbalPosition(m_currentGimbalAz, m_currentGimbalEl);
    m_mapViewModel->setGimbalPosition(m_currentGimbalAz, m_currentGimbalEl);

    // Update zones on map
    m_mapViewModel->updateZones(m_stateModel);
}

void BaseZoneController::hide()
{
    qDebug() << zoneTypeName() << "Controller: hide()";
    m_isActive = false;
    m_mapViewModel->clearWipZone();
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

void BaseZoneController::onUpButtonPressed()
{
    if (!m_isActive) return;

    switch (m_currentState) {
        case State::SelectAction:
        case State::SelectExistingZone:
            navigateMenuUp();
            break;

        case State::EditParameters:
            // Subclass handles parameter navigation
            break;

        default:
            break;
    }
}

void BaseZoneController::onDownButtonPressed()
{
    if (!m_isActive) return;

    switch (m_currentState) {
        case State::SelectAction:
        case State::SelectExistingZone:
            navigateMenuDown();
            break;

        case State::EditParameters:
            // Subclass handles parameter navigation
            break;

        default:
            break;
    }
}

void BaseZoneController::onMenuValButtonPressed()
{
    if (!m_isActive) return;

    // Subclasses override to handle their specific logic
    qDebug() << zoneTypeName() << "Controller: MenuVal pressed in state" << (int)m_currentState;
}

// ============================================================================
// MODEL UPDATE SLOTS
// ============================================================================

void BaseZoneController::onGimbalPositionChanged(float az, float el)
{
    if (!m_isActive) return;

    m_currentGimbalAz = az;
    m_currentGimbalEl = el;
    m_viewModel->setGimbalPosition(az, el);
    m_mapViewModel->setGimbalPosition(az, el);
}

void BaseZoneController::onZonesChanged()
{
    if (!m_isActive) return;

    // Update zone map visualization
    m_mapViewModel->updateZones(m_stateModel);
}

void BaseZoneController::onColorStyleChanged(const QColor& color)
{
    if (!m_isActive) return;

    m_viewModel->setAccentColor(color);
}

// ============================================================================
// STATE MACHINE
// ============================================================================

void BaseZoneController::transitionToState(State newState)
{
    qDebug() << zoneTypeName() << "Controller: State transition"
             << (int)m_currentState << "→" << (int)newState;
    m_currentState = newState;
}

// ============================================================================
// MENU NAVIGATION
// ============================================================================

void BaseZoneController::navigateMenuUp()
{
    if (m_currentMenuItems.isEmpty()) return;

    m_currentMenuIndex--;
    if (m_currentMenuIndex < 0) {
        m_currentMenuIndex = m_currentMenuItems.size() - 1;
    }
    m_viewModel->setCurrentMenuIndex(m_currentMenuIndex);
}

void BaseZoneController::navigateMenuDown()
{
    if (m_currentMenuItems.isEmpty()) return;

    m_currentMenuIndex++;
    if (m_currentMenuIndex >= m_currentMenuItems.size()) {
        m_currentMenuIndex = 0;
    }
    m_viewModel->setCurrentMenuIndex(m_currentMenuIndex);
}

void BaseZoneController::setMenuItems(const QStringList& items)
{
    m_currentMenuItems = items;
    m_currentMenuIndex = 0;
    m_viewModel->setMenuItems(items);
    m_viewModel->setCurrentMenuIndex(0);
}

QString BaseZoneController::selectedMenuItem() const
{
    if (m_currentMenuIndex >= 0 && m_currentMenuIndex < m_currentMenuItems.size()) {
        return m_currentMenuItems[m_currentMenuIndex];
    }
    return QString();
}

// ============================================================================
// UI HELPERS
// ============================================================================

void BaseZoneController::setupMenuUI(const QString& title, const QStringList& menuItems)
{
    m_viewModel->setTitle(title);
    m_viewModel->setInstructionText("");
    m_viewModel->setShowMenu(true);
    m_viewModel->setShowParameterPanel(false);
    m_viewModel->setShowConfirmButtons(false);
    setMenuItems(menuItems);
}

void BaseZoneController::setupMessageUI(const QString& message)
{
    m_viewModel->setTitle("Zone Management");
    m_viewModel->setInstructionText(message);
    m_viewModel->setShowMenu(false);
    m_viewModel->setShowParameterPanel(false);
    m_viewModel->setShowConfirmButtons(false);
}

void BaseZoneController::setupConfirmUI(const QString& title, const QString& question)
{
    m_viewModel->setTitle(title);
    m_viewModel->setInstructionText(question);
    m_viewModel->setShowMenu(false);
    m_viewModel->setShowParameterPanel(false);
    m_viewModel->setShowConfirmButtons(true);
}

void BaseZoneController::showErrorMessage(const QString& error)
{
    qWarning() << zoneTypeName() << "Error:" << error;
    setupMessageUI("ERROR: " + error);
    transitionToState(State::ShowMessage);
    emit messageDisplayed(error);
}

void BaseZoneController::showSuccessMessage(const QString& success)
{
    qInfo() << zoneTypeName() << "Success:" << success;
    setupMessageUI(success);
    transitionToState(State::ShowMessage);
    emit messageDisplayed(success);
}

// ============================================================================
// ANGLE NORMALIZATION
// ============================================================================

float BaseZoneController::normalizeAzimuthTo360(float az) const
{
    // Normalize azimuth to [0, 360)
    az = std::fmod(az, 360.0f);
    if (az < 0.0f) {
        az += 360.0f;
    }
    return az;
}

float BaseZoneController::normalizeElevation(float el) const
{
    // Clamp elevation to typical range [-90, 90]
    // or use your system's specific limits
    return qBound(-90.0f, el, 90.0f);
}
