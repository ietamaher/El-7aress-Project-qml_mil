#ifndef SECTORSCAN ZONECONTROLLER_H
#define SECTORSCAN ZONECONTROLLER_H

#include "BaseZoneController.h"

class SectorScanParameterViewModel;

/**
 * @class SectorScanZoneController
 * @brief Controller for Auto Sector Scan zone management
 *
 * Manages automatic sector scanning zones:
 * - Defines azimuth start/end points for scanning
 * - Configures elevation range
 * - Sets scan rate (degrees per second)
 * - Enables/disables scan patterns
 *
 * Workflow:
 * 1. User selects action (New/Modify/Delete)
 * 2. For New: Aim at start point, then end point
 * 3. Calculate scan sector (azimuth span, center)
 * 4. Edit scan parameters (name, elevation, rate)
 * 5. Confirm and save
 *
 * Scan Pattern:
 * - Gimbal sweeps from start_az to end_az at constant rate
 * - Holds at elevation for entire scan
 * - Reverses direction at boundaries (ping-pong pattern)
 */
class SectorScanZoneController : public BaseZoneController
{
    Q_OBJECT

public:
    explicit SectorScanZoneController(QObject* parent = nullptr);

    // BaseZoneController interface
    ZoneType zoneType() const override { return ZoneType::AutoSectorScan; }
    QString zoneTypeName() const override { return "SectorScan"; }

    void initialize() override;
    void show();
    void setParameterViewModel(SectorScanParameterViewModel* paramViewModel);

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
    void handleSelectExistingZoneInput();
    void handleAimingPoint1Input();
    void handleAimingPoint2Input();
    void handleEditParametersInput();
    void handleConfirmSaveInput();
    void handleConfirmDeleteInput();

    // UI setup
    void setupSelectActionUI();
    void setupSelectExistingZoneUI(const QString& action);
    void setupAimingPoint1UI();
    void setupAimingPoint2UI();
    void setupEditParametersUI();

    // Parameter panel routing
    void routeUpToParameterPanel();
    void routeDownToParameterPanel();
    void routeSelectToParameterPanel();

    // Sector calculation
    void calculateSectorGeometry();
    void validateSectorGeometry();

    // WIP zone data
    void resetWipZone();
    void loadWipZoneFromSystem(int zoneId);
    void syncWipZoneToParameterPanel();
    void syncParameterPanelToWipZone();

    // ViewModel
    SectorScanParameterViewModel* m_paramViewModel;

    // WIP data
    AutoSectorScanZone m_wipZone;
    int m_editingZoneId;  // -1 for new zone, >=0 for editing
    bool m_isModifying;   // true = modify mode, false = delete mode

    // Aiming state
    bool m_point1Defined;
    float m_point1Az, m_point1El;
    float m_point2Az, m_point2El;
};

#endif // SECTORSCAN ZONECONTROLLER_H
