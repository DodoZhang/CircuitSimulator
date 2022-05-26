#include "MOSFET.h"

#include <cmath>

#include "../Circuit.h"

using namespace CirSim;

MOSFET::MOSFET(Circuit *circuit, ChannelType type, double turnOnVoltage,
               double conductivityConstant, double channelLengthModulation)
    : Element(circuit),
      m_pins { Pin(this, 0), Pin(this, 1), Pin(this, 2) },
      m_cvr { this, this, this }
{
    m_type = type;
    m_vTN = turnOnVoltage;
    m_Kn = conductivityConstant / 1000;
    m_lambda = channelLengthModulation;

    m_cvr[0].current[Drain] = 1;
    m_cvr[0].current[Gate] = 0;
    m_cvr[0].current[Source] = 0;
    m_cvr[0].voltage[Drain] = 0;
    m_cvr[0].voltage[Gate] = 0;
    m_cvr[0].voltage[Source] = 0;
    m_cvr[0].offset = 0;

    m_cvr[1].current[Drain] = 0;
    m_cvr[1].current[Gate] = 1;
    m_cvr[1].current[Source] = 0;
    m_cvr[1].voltage[Drain] = 0;
    m_cvr[1].voltage[Gate] = 0;
    m_cvr[1].voltage[Source] = 0;
    m_cvr[1].offset = 0;

    m_cvr[2].current[Drain] = 1;
    m_cvr[2].current[Gate] = 1;
    m_cvr[2].current[Source] = 1;
    m_cvr[2].voltage[Drain] = 0;
    m_cvr[2].voltage[Gate] = 0;
    m_cvr[2].voltage[Source] = 0;
    m_cvr[2].offset = 0;
}

int MOSFET::pinCount() const
{
    return 3;
}

Pin *MOSFET::pin(int index)
{
    return &m_pins[index];
}

const Pin *MOSFET::pin(int index) const
{
    return &m_pins[index];
}

const CVR *MOSFET::cvr(int index) const
{
    return &m_cvr[index];
}

void MOSFET::setChannelType(ChannelType type)
{
    m_type = type;
}

MOSFET::ChannelType MOSFET::channelType() const
{
    return m_type;
}

void MOSFET::setConductivityConstant(double conductivityConstant)
{
    m_Kn = conductivityConstant / 1000;
}

double MOSFET::conductivityConstant() const
{
    return m_Kn * 1000;
}

void MOSFET::setTurnOnVoltage(double turnOnVoltage)
{
    m_vTN = turnOnVoltage;
}

double MOSFET::turnOnVoltage() const
{
    return m_vTN;
}

void MOSFET::setPitchOffVoltage(double pitchOffVoltage)
{
    m_vTN = pitchOffVoltage;
}

double MOSFET::pitchOffVoltage() const
{
    return m_vTN;
}

void MOSFET::setChannelLengthModulation(double channelLengthModulation)
{
    m_lambda = channelLengthModulation;
}

double MOSFET::channelLengthModulation() const
{
    return m_lambda;
}

void MOSFET::tick(double time, double deltaTime)
{
    iterate(time, deltaTime);
}

void MOSFET::iterate(double, double)
{
    double vGS = m_circuit->voltage(pin(Gate)->net()) - m_circuit->voltage(pin(Source)->net());
    double vDS = m_circuit->voltage(pin(Drain)->net()) - m_circuit->voltage(pin(Source)->net());
    double acc = acConductivity(vGS, vDS);
//    m_cvr[0].current[Drain] = 1;
//    m_cvr[0].current[Gate] = 0;
//    m_cvr[0].current[Source] = 0;
    m_cvr[0].voltage[Drain] = -acc;
//    m_cvr[0].voltage[Gate] = 0;
    m_cvr[0].voltage[Source] = acc;
    m_cvr[0].offset = drainCurrent(vGS, vDS) - acc * vDS;
}

double MOSFET::error()
{
    double vGS = m_circuit->voltage(pin(Gate)->net()) - m_circuit->voltage(pin(Source)->net());
    double vDS = m_circuit->voltage(pin(Drain)->net()) - m_circuit->voltage(pin(Source)->net());
    double delta = drainCurrent(vGS, vDS) - m_circuit->current(pin(Drain));
    return delta * delta;
}

double MOSFET::drainCurrent(double vGS, double vDS) const
{
    if (m_type == PChannel)
    {
        vDS = -vDS;
        double detv = m_vTN - vGS;
        if (detv < 0) return 0;
        if (vDS < 0) return 0;
        double tmp = m_lambda * detv - 2;
        tmp = tmp * tmp;
        if (vDS < (4 - 2 * m_lambda * detv) * detv / tmp)
            return -m_Kn * (2 * detv - tmp / 4 * vDS) * vDS;
        else return -m_Kn * detv * detv * (1 + m_lambda * vDS);
    }
    else
    {
        double detv = vGS - m_vTN;
        if (detv < 0) return 0;
        if (vDS < 0) return 0;
        double tmp = m_lambda * detv - 2;
        tmp = tmp * tmp;
        if (vDS < (4 - 2 * m_lambda * detv) * detv / tmp)
            return m_Kn * (2 * detv - tmp / 4 * vDS) * vDS;
        else return m_Kn * detv * detv * (1 + m_lambda * vDS);
    }
}

double MOSFET::acConductivity(double vGS, double vDS) const
{
    if (m_type == PChannel)
    {
        vDS = -vDS;
        double detv = m_vTN - vGS;
        if (detv < 0) return 0;
        if (vDS < 0) return 0;
        double tmp = m_lambda * detv - 2;
        tmp = tmp * tmp;
        if (vDS < (4 - 2 * m_lambda * detv) * detv / tmp)
            return -m_Kn * (2 * detv - tmp / 2 * vDS);
        else return -m_Kn * detv * detv * m_lambda;
    }
    else
    {
        double detv = vGS - m_vTN;
        if (detv < 0) return 0;
        if (vDS < 0) return 0;
        double tmp = m_lambda * detv - 2;
        tmp = tmp * tmp;
        if (vDS < (4 - 2 * m_lambda * detv) * detv / tmp)
            return m_Kn * (2 * detv - tmp / 2 * vDS);
        else return m_Kn * detv * detv * m_lambda;
    }
}

#ifdef QT_DEBUG
QString MOSFET::debug()
{
    return "MOSFET: Kn = " + QString::number(m_Kn * 1000) + "mA/V";
}
#endif
