#include "Diode.h"

#include <cmath>

#include "../Circuit.h"

using namespace CirSim;

Diode::Diode(Circuit *circuit, double ICBO)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1) },
      m_cvr { this, this }
{
    m_ICBO = ICBO;

    m_cvr[0].current[Positive] = 1;
    m_cvr[0].current[Negative] = 0;
    m_cvr[0].voltage[Positive] = 0;
    m_cvr[0].voltage[Negative] = 0;
    m_cvr[0].offset = 0;

    m_cvr[1].current[Positive] = 1;
    m_cvr[1].current[Negative] = 1;
    m_cvr[1].voltage[Positive] = 0;
    m_cvr[1].voltage[Negative] = 0;
    m_cvr[1].offset = 0;
}

int Diode::pinCount() const
{
    return 2;
}

Pin *Diode::pin(int index)
{
    return &m_pins[index];
}

const Pin *Diode::pin(int index) const
{
    return &m_pins[index];
}

const CVR *Diode::cvr(int index) const
{
    return &m_cvr[index];
}

void Diode::setICBO(double ICBO)
{
    m_ICBO = ICBO;
}

double Diode::ICBO() const
{
    return m_ICBO;
}

#define CVR(voltage) (m_ICBO * (exp((voltage) / 26e-3) - 1))
#define ACC(voltage) (m_ICBO / 26e-3 * exp((voltage) / 26e-3))

void Diode::tick(double time, double deltaTime)
{
    iterate(time, deltaTime);
}

void Diode::iterate(double, double)
{
    double vol = m_circuit->voltage(pin(Positive)->net()) - m_circuit->voltage(pin(Negative)->net());
    double acc = ACC(vol);
//    m_cvr[0].current[Positive] = 1;
//    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[Positive] = -acc;
    m_cvr[0].voltage[Negative] = acc;
    m_cvr[0].offset = CVR(vol) - acc * vol;
}

double Diode::error()
{
    double delta = CVR(m_circuit->voltage(pin(0)->net()) - m_circuit->voltage(pin(1)->net())) - m_circuit->current(pin(0));
    return delta * delta;
}

double Diode::current(double voltage) const
{
    return m_ICBO * (exp(voltage / 26e-3) - 1);
}

double Diode::acConductivity(double voltage) const
{
    return m_ICBO / 26e-3 * exp(voltage / 26e-3);
}

#ifdef QT_DEBUG
QString Diode::debug()
{
    return "Diode";
}
#endif
