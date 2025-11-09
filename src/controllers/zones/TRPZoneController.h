#ifndef TRPZONECONTROLLER_H
#define TRPZONECONTROLLER_H

#include "BaseZoneController.h"

class TRPParameterViewModel;

/**
 * @class TRPZoneController
 * @brief Controller for Target Reference Point (TRP) management
 *
 * Manages target reference points - preset aim points for quick targeting:
 * - Single-point aiming (azimuth/elevation)
 * - Named locations for rapid engagement
 * - Quick recall for frequently-used positions
 *
 * Workflow:
 * 1. User selects action (New/Modify/Delete)
 * 2. For New: Aim at target point
 * 3. Edit TRP parameters (name, description)
 * 4. Confirm and save
 *
 * Use Cases:
 * - Pre-designated targets
 * - Known threat positions
 * - Rally points or landmarks
 * - Quick-reaction engagement positions
 */
class TRPZoneController : public BaseZoneController
{
    Q_OBJECT

public:
    explicit TRPZoneController(QObject* parent = nullptr);

    // BaseZoneController interface
    ZoneType zoneType() const override { return ZoneType::TargetReferencePoint; }
    QString zoneTypeName() const override { return "TRP"; }

    void initialize() override;
    void setParameterViewModel(TRPParameterViewModel* paramViewModel);

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
    // State-specific input handling
    void handleSelectActionInput();
    void handleSelectExistingTRPInput();
    void handleAimingPointInput();
    void handleEditParametersInput();
    void handleConfirmSaveInput();
    void handleConfirmDeleteInput();

    // UI setup
    void setupSelectActionUI();
    void setupSelectExistingTRPUI(const QString& action);
    void setupAimingPointUI();
    void setupEditParametersUI();

    // Parameter panel routing
    void routeUpToParameterPanel();
    void routeDownToParameterPanel();
    void routeSelectToParameterPanel();

    // WIP TRP data
    void resetWipTRP();
    void loadWipTRPFromSystem(int trpId);
    void syncWipTRPToParameterPanel();
    void syncParameterPanelToWipTRP();

    // ViewModel
    TRPParameterViewModel* m_paramViewModel;

    // WIP data
    TargetReferencePoint m_wipTRP;
    int m_editingTRPId;  // -1 for new TRP, >=0 for editing
    bool m_isModifying;  // true = modify mode, false = delete mode
};

#endif // TRPZONECONTROLLER_H
