#include "Resistance.h"

using namespace CirSim;

Resistance::Resistance(Circuit *circuit, double resistance)
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

int Resistance::pinCount() const
{
    return 2;
}

Pin *Resistance::pin(int index)
{
    return &m_pins[index];
}

const Pin *Resistance::pin(int index) const
{
    return &m_pins[index];
}

const CVR *Resistance::cvr(int index) const
{
    return &m_cvr[index];
}

void Resistance::setResistance(double resistance)
{
    m_cvr[0].current[0] = resistance;
}

double Resistance::resistance() const
{
    return m_cvr[0].current[0];
}
