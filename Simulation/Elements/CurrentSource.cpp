#include "CurrentSource.h"

using namespace CirSim;

CurrentSource::CurrentSource(Circuit *circuit, double current)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1) },
      m_cvr { this, this }
{
    m_cvr[0].current[0] = 1;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = 0;
    m_cvr[0].voltage[1] = 0;
    m_cvr[0].offset = current;

    m_cvr[1].current[0] = 1;
    m_cvr[1].current[1] = 1;
    m_cvr[1].voltage[0] = 0;
    m_cvr[1].voltage[1] = 0;
    m_cvr[1].offset = 0;
}

int CurrentSource::pinCount() const
{
    return 2;
}

Pin *CurrentSource::pin(int index)
{
    return &m_pins[index];
}

const Pin *CurrentSource::pin(int index) const
{
    return &m_pins[index];
}

const CVR *CurrentSource::cvr(int index) const
{
    return &m_cvr[index];
}

void CurrentSource::setCurrent(double current)
{
    m_cvr[0].offset = current;
}

double CurrentSource::current() const
{
    return m_cvr[0].offset;
}

#ifdef QT_DEBUG
QString CurrentSource::debug()
{
    return "Current Source: I = " + QString::number(current()) + "A";
}
#endif
