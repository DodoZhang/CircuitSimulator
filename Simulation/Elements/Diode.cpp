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

    m_cvr[0].current[0] = 1;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = 0;
    m_cvr[0].voltage[1] = 0;
    m_cvr[0].offset = 0;

    m_cvr[1].current[0] = 1;
    m_cvr[1].current[1] = 1;
    m_cvr[1].voltage[0] = 0;
    m_cvr[1].voltage[1] = 0;
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
#define ACR(voltage) (m_ICBO / 26e-3 * exp((voltage) / 26e-3))

void Diode::tick(double, double)
{
    double vol = m_circuit->voltage(pin(0)->net()) - m_circuit->voltage(pin(1)->net());
    double acr = ACR(vol);
    m_cvr[0].current[0] = 1;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = -acr;
    m_cvr[0].voltage[1] = acr;
    m_cvr[0].offset = CVR(vol) - acr * vol;
}

void Diode::iterate(double, double)
{
    double vol = m_circuit->voltage(pin(0)->net()) - m_circuit->voltage(pin(1)->net());
    double acr = ACR(vol);
    m_cvr[0].current[0] = 1;
    m_cvr[0].current[1] = 0;
    m_cvr[0].voltage[0] = -acr;
    m_cvr[0].voltage[1] = acr;
    m_cvr[0].offset = CVR(vol) - acr * vol;
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

double Diode::acResistance(double voltage) const
{
    return m_ICBO / 26e-3 * exp(voltage / 26e-3);
}
