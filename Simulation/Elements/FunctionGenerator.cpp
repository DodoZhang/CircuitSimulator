#include "FunctionGenerator.h"

using namespace CirSim;

FunctionGenerator::FunctionGenerator(Circuit *circuit, const QString &function)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1) },
      m_cvr { this, this },
      m_function(function)
{
    m_cvr[0].current[0] = 0;
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

int FunctionGenerator::pinCount() const
{
    return 2;
}

Pin *FunctionGenerator::pin(int index)
{
    return &m_pins[index];
}

const Pin *FunctionGenerator::pin(int index) const
{
    return &m_pins[index];
}

const CVR *FunctionGenerator::cvr(int index) const
{
    return &m_cvr[index];
}

bool FunctionGenerator::setFunction(const QString &function)
{
    return m_function.set(function);
}

const QString &FunctionGenerator::function() const
{
    return m_function.get();
}

void FunctionGenerator::tick(double time, double)
{
    m_parameters.insert("t", time);
    m_cvr[0].offset = m_function.calc(m_parameters);
}
