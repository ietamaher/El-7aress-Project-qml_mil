#include "ledcontroller.h"
#include "hardware/devices/plc21device.h"

LedController::LedController(SystemStateModel* systemStateModel, Plc21Device* plc21Device, QObject *parent)
    : QObject(parent),
    m_systemStateModel(systemStateModel),
    m_plc21Device(plc21Device)
{
    connect(m_systemStateModel, &SystemStateModel::dataChanged, this, &LedController::onSystemStateChanged);
}

void LedController::onSystemStateChanged(const SystemStateData &data)
{
    if (m_plc21Device) {
        m_plc21Device->setGunArmedLed(data.gunArmed);
        m_plc21Device->setStationEnabledLed(data.stationEnabled);
        m_plc21Device->setStationInput1Led(data.stationInput1);
        m_plc21Device->setPanelBacklight(data.osdColorStyle == ColorStyle::Red);
    }
}

