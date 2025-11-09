#ifndef BASEZONECONTROLLER_H
#define BASEZONECONTROLLER_H

#include <QObject>
#include <QStringList>
#include "models/domain/systemstatedata.h"

class ZoneDefinitionViewModel;
class ZoneMapViewModel;
class SystemStateModel;

/**
 * @class BaseZoneController
 * @brief Abstract base class for all zone type controllers
 *
 * Provides common functionality for zone management:
 * - State machine framework
 * - Menu navigation (up/down/select)
 * - Gimbal position tracking
 * - UI setup helpers
 * - Model connections
 *
 * Subclasses implement zone-specific logic:
 * - SafetyZoneController (AreaZone)
 * - SectorScanZoneController (AutoSectorScanZone)
 * - TRPZoneController (TargetReferencePoint)
 *
 * Design Pattern: Template Method Pattern
 */
class BaseZoneController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Common state machine states
     * Subclasses can extend with zone-specific states
     */
    enum class State {
        Idle,
        SelectAction,           // New, Modify, Delete
        SelectExistingZone,     // Select zone to modify/delete
        AimingPoint,            // Aiming at target point(s)
        EditParameters,         // Editing zone parameters
        ConfirmSave,            // Confirm save operation
        ConfirmDelete,          // Confirm delete operation
        ShowMessage             // Display message to user
    };
    Q_ENUM(State)

    explicit BaseZoneController(QObject* parent = nullptr);
    virtual ~BaseZoneController() = default;

    // Initialization
    void setViewModel(ZoneDefinitionViewModel* viewModel);
    void setMapViewModel(ZoneMapViewModel* mapViewModel);
    void setStateModel(SystemStateModel* stateModel);
    virtual void initialize();

    // Zone type identifier
    virtual ZoneType zoneType() const = 0;
    virtual QString zoneTypeName() const = 0;

    // Controller lifecycle
    virtual void show();
    virtual void hide();
    bool isActive() const { return m_isActive; }

public slots:
    // Input handling (delegates to subclass if needed)
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onMenuValButtonPressed();

signals:
    void finished();
    void messageDisplayed(const QString& message);
    void zoneCreated(ZoneType type);
    void zoneModified(ZoneType type, int zoneId);
    void zoneDeleted(ZoneType type, int zoneId);

protected slots:
    // Model updates
    void onGimbalPositionChanged(float az, float el);
    void onZonesChanged();
    void onColorStyleChanged(const QColor& color);

protected:
    // ========================================================================
    // ABSTRACT METHODS - Must be implemented by subclasses
    // ========================================================================

    /**
     * @brief Create a new zone of this type
     * Called when user selects "New" from menu
     */
    virtual void createNewZone() = 0;

    /**
     * @brief Load existing zone for modification
     * @param zoneId ID of zone to modify
     */
    virtual void loadZoneForModification(int zoneId) = 0;

    /**
     * @brief Delete zone of this type
     * @param zoneId ID of zone to delete
     */
    virtual void performZoneDeletion(int zoneId) = 0;

    /**
     * @brief Save current zone to system state
     * @return true if save successful
     */
    virtual bool saveCurrentZone() = 0;

    /**
     * @brief Update work-in-progress zone visualization on map
     */
    virtual void updateWipZoneVisualization() = 0;

    /**
     * @brief Get list of existing zones for selection menu
     * @return QStringList of zone names
     */
    virtual QStringList getExistingZoneNames() const = 0;

    /**
     * @brief Get zone ID from menu selection index
     * @param menuIndex Selected menu index
     * @return Zone ID
     */
    virtual int getZoneIdFromMenuIndex(int menuIndex) const = 0;

    // ========================================================================
    // COMMON FUNCTIONALITY - Available to all subclasses
    // ========================================================================

    // State machine
    void transitionToState(State newState);
    State currentState() const { return m_currentState; }

    // Menu navigation
    void navigateMenuUp();
    void navigateMenuDown();
    int currentMenuIndex() const { return m_currentMenuIndex; }
    const QStringList& currentMenuItems() const { return m_currentMenuItems; }
    void setMenuItems(const QStringList& items);
    QString selectedMenuItem() const;

    // Gimbal position access
    float currentGimbalAz() const { return m_currentGimbalAz; }
    float currentGimbalEl() const { return m_currentGimbalEl; }

    // UI helpers
    void setupMenuUI(const QString& title, const QStringList& menuItems);
    void setupMessageUI(const QString& message);
    void setupConfirmUI(const QString& title, const QString& question);
    void showErrorMessage(const QString& error);
    void showSuccessMessage(const QString& success);

    // Angle normalization
    float normalizeAzimuthTo360(float az) const;
    float normalizeElevation(float el) const;

    // ViewModels access
    ZoneDefinitionViewModel* viewModel() { return m_viewModel; }
    ZoneMapViewModel* mapViewModel() { return m_mapViewModel; }
    SystemStateModel* stateModel() { return m_stateModel; }

private:
    // State
    State m_currentState;
    bool m_isActive;

    // ViewModels
    ZoneDefinitionViewModel* m_viewModel;
    ZoneMapViewModel* m_mapViewModel;

    // Models
    SystemStateModel* m_stateModel;

    // Menu navigation
    QStringList m_currentMenuItems;
    int m_currentMenuIndex;

    // Current gimbal position
    float m_currentGimbalAz;
    float m_currentGimbalEl;
};

#endif // BASEZONECONTROLLER_H
