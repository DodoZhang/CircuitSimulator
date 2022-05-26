#include "Resistor.h"

using namespace CirSim;

Resistor::Resistor(Circuit *circuit, double resistance)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1) },
      m_cvr { this, this }
{
    m_cvr[0].current[0] = resistance;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = -1;
    m_cvr[0].voltage[1] = 1;
    m_cvr[0].offset = 0;

    m_cvr[1].current[0] = 1;
    m_cvr[1].current[1] = 1;
    m_cvr[1].voltage[0] = 0;
    m_cvr[1].voltage[1] = 0;
    m_cvr[1].offset = 0;
}

int Resistor::pinCount() const
{
    return 2;
}

Pin *Resistor::pin(int index)
{
    return &m_pins[index];
}

const Pin *Resistor::pin(int index) const
{
    return &m_pins[index];
}

const CVR *Resistor::cvr(int index) const
{
    return &m_cvr[index];
}

void Resistor::setResistance(double resistance)
{
    m_cvr[0].current[0] = resistance;
}

double Resistor::resistance() const
{
    return m_cvr[0].current[0];
}

#ifdef QT_DEBUG
QString Resistor::debug()
{
    return "Resistance: R = " + QString::number(resistance()) + "Ohm";
}
#endif
