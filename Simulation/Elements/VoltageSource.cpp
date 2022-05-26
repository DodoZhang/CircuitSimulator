#include "VoltageSource.h"

using namespace CirSim;

VoltageSource::VoltageSource(Circuit *circuit, double voltage)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1) },
      m_cvr { this, this }
{
    m_cvr[0].current[0] = 0;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = -1;
    m_cvr[0].voltage[1] = 1;
    m_cvr[0].offset = voltage;

    m_cvr[1].current[0] = 1;
    m_cvr[1].current[1] = 1;
    m_cvr[1].voltage[0] = 0;
    m_cvr[1].voltage[1] = 0;
    m_cvr[1].offset = 0;
}

int VoltageSource::pinCount() const
{
    return 2;
}

Pin *VoltageSource::pin(int index)
{
    return &m_pins[index];
}

const Pin *VoltageSource::pin(int index) const
{
    return &m_pins[index];
}

const CVR *VoltageSource::cvr(int index) const
{
    return &m_cvr[index];
}

void VoltageSource::setVoltage(double voltage)
{
    m_cvr[0].offset = voltage;
}

double VoltageSource::voltage() const
{
    return m_cvr[0].offset;
}

#ifdef QT_DEBUG
QString VoltageSource::debug()
{
    return "Voltage Source: V = " + QString::number(voltage()) + "V";
}
#endif
