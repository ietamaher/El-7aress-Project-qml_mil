#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QObject>
#include "models/domain/systemstatemodel.h"

class Plc21Device;

class LedController : public QObject
{
    Q_OBJECT
public:
    explicit LedController(SystemStateModel* systemStateModel, Plc21Device* plc21Device, QObject *parent = nullptr);

private slots:
    void onSystemStateChanged(const SystemStateData& data);

private:
    SystemStateModel* m_systemStateModel;
    Plc21Device* m_plc21Device;
};

#endif // LEDCONTROLLER_H

