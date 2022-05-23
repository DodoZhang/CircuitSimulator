#include "Inductor.h"

#include "../Circuit.h"

using namespace CirSim;

Inductor::Inductor(Circuit *circuit, double inductance, double initCurrent)
    : CurrentSource(circuit, initCurrent)
{
    m_inductance = inductance;
}

void Inductor::setInductance(double inductance)
{
    m_inductance = inductance;
}

double Inductor::inductance() const
{
    return m_inductance;
}

void Inductor::tick(double, double deltaTime)
{
    setCurrent(current() + (m_circuit->voltage(pin(0)->net()) - m_circuit->voltage(pin(1)->net())) * deltaTime / m_inductance);
}
