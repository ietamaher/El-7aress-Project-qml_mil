#ifndef OSDCONTROLLER_H
#define OSDCONTROLLER_H

#include <QObject>
#include "models/osdviewmodel.h"
#include "models/domain/systemstatemodel.h"

// Forward declaration for Phase 2
struct FrameData;

class OsdController : public QObject
{
    Q_OBJECT

public:
    explicit OsdController(QObject *parent = nullptr);
    void initialize();

public slots:
    // Phase 1: Direct from SystemStateModel (Active NOW)
    void onSystemStateChanged(const SystemStateData& data);

    // Phase 2: From CameraVideoStreamDevice (Add LATER when tracking is ready)
    // void onFrameDataReady(const FrameData& data);

private slots:
    void onColorStyleChanged(const QColor& color);

private:
    // Shared update logic used by both paths
    void updateViewModelFromSystemState(const SystemStateData& data);

    // ViewModels
    OsdViewModel* m_viewModel;

    // Models
    SystemStateModel* m_stateModel;
};

#endif // OSDCONTROLLER_H
