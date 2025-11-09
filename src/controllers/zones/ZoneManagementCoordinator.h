#ifndef ZONEMANAGEMENTCOORDINATOR_H
#define ZONEMANAGEMENTCOORDINATOR_H

#include <QObject>
#include "models/domain/systemstatedata.h"

// Forward declarations
class SafetyZoneController;
class SectorScanZoneController;
class TRPZoneController;
class ZoneDefinitionViewModel;
class ZoneMapViewModel;
class AreaZoneParameterViewModel;
class SectorScanParameterViewModel;
class TRPParameterViewModel;
class SystemStateModel;

/**
 * @class ZoneManagementCoordinator
 * @brief Facade for zone management - coordinates all zone type controllers
 *
 * This coordinator provides a single interface for QML to interact with
 * all zone types. It routes input to the appropriate specialized controller.
 *
 * Design Pattern: Facade Pattern
 *
 * Manages:
 * - SafetyZoneController (AreaZone - NoFire/NoTraverse)
 * - SectorScanZoneController (AutoSectorScanZone)
 * - TRPZoneController (TargetReferencePoint)
 *
 * QML Interface:
 * - Exposes unified methods for zone management
 * - Routes button inputs to active controller
 * - Manages controller lifecycle and transitions
 *
 * Benefits:
 * - Single point of access for QML
 * - Simplifies QML code (one controller vs three)
 * - Hides complexity of specialized controllers
 * - Easy to extend with new zone types
 */
class ZoneManagementCoordinator : public QObject
{
    Q_OBJECT

public:
    explicit ZoneManagementCoordinator(QObject* parent = nullptr);
    ~ZoneManagementCoordinator();

    // Initialization
    void initialize();
    void setViewModel(ZoneDefinitionViewModel* viewModel);
    void setMapViewModel(ZoneMapViewModel* mapViewModel);
    void setParameterViewModels(AreaZoneParameterViewModel* areaVM,
                                SectorScanParameterViewModel* sectorVM,
                                TRPParameterViewModel* trpVM);
    void setStateModel(SystemStateModel* stateModel);

public slots:
    // Main interface (called from QML)
    void show();
    void hide();

    // Input routing (called from QML or parent controller)
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onMenuValButtonPressed();

signals:
    void closed();
    void returnToMainMenu();

private slots:
    // Zone type selection
    void selectZoneType();

    // Controller lifecycle
    void onSafetyZoneFinished();
    void onSectorScanFinished();
    void onTRPFinished();

private:
    // UI setup
    void setupZoneTypeSelectionUI();
    void transitionToController(ZoneType type);

    // Controller management
    void deactivateAllControllers();

    // ViewModels
    ZoneDefinitionViewModel* m_viewModel;
    ZoneMapViewModel* m_mapViewModel;
    AreaZoneParameterViewModel* m_areaZoneParamViewModel;
    SectorScanParameterViewModel* m_sectorScanParamViewModel;
    TRPParameterViewModel* m_trpParamViewModel;

    // Models
    SystemStateModel* m_stateModel;

    // Specialized controllers
    SafetyZoneController* m_safetyZoneController;
    SectorScanZoneController* m_sectorScanController;
    TRPZoneController* m_trpController;

    // State
    enum class CoordinatorState {
        Idle,
        SelectingZoneType,
        ManagingSafetyZones,
        ManagingSectorScans,
        ManagingTRPs
    };

    CoordinatorState m_currentState;
    ZoneType m_selectedZoneType;

    // Zone type selection menu
    QStringList m_zoneTypeMenuItems;
    int m_zoneTypeMenuIndex;
};

#endif // ZONEMANAGEMENTCOORDINATOR_H
