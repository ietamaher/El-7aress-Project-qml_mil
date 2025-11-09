#ifndef SAFETYZONECONTROLLER_H
#define SAFETYZONECONTROLLER_H

#include "BaseZoneController.h"

class AreaZoneParameterViewModel;
class ZoneGeometryService;

/**
 * @class SafetyZoneController
 * @brief Controller for Area Zone management (NoFire and NoTraverse zones)
 *
 * Manages safety-critical zones that restrict weapon operation:
 * - No-Fire Zones: Areas where weapon discharge is prohibited
 * - No-Traverse Zones: Areas where gimbal cannot enter
 *
 * Workflow:
 * 1. User selects action (New/Modify/Delete)
 * 2. For New: Aim at Corner 1, then Corner 2
 * 3. Calculate zone geometry (width, height, center, angle)
 * 4. Edit zone parameters (name, type, enabled)
 * 5. Confirm and save
 *
 * Safety Notes:
 * - All AreaZones are safety-critical - validation is mandatory
 * - Zone geometry must be valid before save
 * - Overlapping zones are allowed (most restrictive takes precedence)
 */
class SafetyZoneController : public BaseZoneController
{
    Q_OBJECT

public:
    explicit SafetyZoneController(QObject* parent = nullptr);

    // BaseZoneController interface
    ZoneType zoneType() const override { return ZoneType::NoFire; }
    QString zoneTypeName() const override { return "SafetyZone"; }

    void initialize() override;
    void setParameterViewModel(AreaZoneParameterViewModel* paramViewModel);

public slots:
    void onMenuValButtonPressed() override;
    void onUpButtonPressed() override;
    void onDownButtonPressed() override;

protected:
    // Abstract method implementations
    void createNewZone() override;
    void loadZoneForModification(int zoneId) override;
    void performZoneDeletion(int zoneId) override;
    bool saveCurrentZone() override;
    void updateWipZoneVisualization() override;
    QStringList getExistingZoneNames() const override;
    int getZoneIdFromMenuIndex(int menuIndex) const override;

private:
    // Extended state machine (in addition to base states)
    enum class AreaZoneState {
        AimingCorner1,
        AimingCorner2
    };

    // State-specific input handling
    void handleSelectActionInput();
    void handleSelectExistingZoneInput();
    void handleAimingCorner1Input();
    void handleAimingCorner2Input();
    void handleEditParametersInput();
    void handleConfirmSaveInput();
    void handleConfirmDeleteInput();

    // UI setup
    void setupSelectActionUI();
    void setupSelectExistingZoneUI(const QString& action);
    void setupAimingCorner1UI();
    void setupAimingCorner2UI();
    void setupEditParametersUI();

    // Parameter panel routing
    void routeUpToParameterPanel();
    void routeDownToParameterPanel();
    void routeSelectToParameterPanel();

    // Geometry calculation
    void calculateZoneGeometry();
    void validateZoneGeometry();

    // WIP zone data
    void resetWipZone();
    void loadWipZoneFromSystem(int zoneId);
    void syncWipZoneToParameterPanel();
    void syncParameterPanelToWipZone();

    // ViewModel
    AreaZoneParameterViewModel* m_paramViewModel;

    // WIP data
    AreaZone m_wipZone;
    int m_editingZoneId;  // -1 for new zone, >=0 for editing
    bool m_isModifying;   // true = modify mode, false = delete mode

    // Aiming state
    bool m_corner1Defined;
    float m_corner1Az, m_corner1El;
    float m_corner2Az, m_corner2El;

    // Zone geometry service
    ZoneGeometryService* m_geometryService;
};

#endif // SAFETYZONECONTROLLER_H
