#include "Capacity.h"

#include "../Circuit.h"

using namespace CirSim;

Capacity::Capacity(Circuit *circuit, double capacitance, double initVoltage)
    : VoltageSource(circuit, initVoltage)
{
    m_capacitance = capacitance;
}

void Capacity::setCapacitance(double capacitance)
{
    m_capacitance = capacitance;
}

double Capacity::capacitance() const
{
    return m_capacitance;
}

void Capacity::tick(double, double deltaTime)
{
    setVoltage(voltage() + m_circuit->current(pin(1)) * deltaTime / m_capacitance);
}
