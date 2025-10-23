#ifndef OSDCONTROLLER_H
#define OSDCONTROLLER_H

#include <QObject>

// Forward declarations
class OsdViewModel;
class SystemStateModel;
struct FrameData;
struct SystemStateData;

/**
 * @brief OsdController - Manages OSD updates from SystemStateModel
 *
 * This controller bridges SystemStateModel to OsdViewModel.
 *
 * PHASE 1 (Active NOW): Updates from SystemStateModel
 * PHASE 2 (Later): Can also update from CameraVideoStreamDevice FrameData
 */
class OsdController : public QObject
{
    Q_OBJECT

public:
    explicit OsdController(QObject *parent = nullptr);

    // Dependency injection (called by SystemController)
    void setViewModel(OsdViewModel* viewModel);
    void setStateModel(SystemStateModel* stateModel);

    // Initialize connections
    void initialize();

public slots:
    // PHASE 1: Direct from SystemStateModel (Active NOW)
    //void onSystemStateChanged(const SystemStateData& data);

    // PHASE 2: From CameraVideoStreamDevice (Uncomment when ready)
    void onFrameDataReady(const FrameData& data);

private slots:
    void onColorStyleChanged(const QColor& color);

private:
    // Shared update logic
    //void updateViewModelFromSystemState(const SystemStateData& data);

    // Dependencies (injected)
    OsdViewModel* m_viewModel;
    SystemStateModel* m_stateModel;
};

#endif // OSDCONTROLLER_H
