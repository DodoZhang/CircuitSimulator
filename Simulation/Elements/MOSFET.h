#ifndef CIRSIM_MOSFET_H
#define CIRSIM_MOSFET_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class MOSFET : public Element
{
public:
    enum PinDefinition : int
    {
        Drain = 0,
        Gate = 1,
        Source = 2
    };
    enum ChannelType : bool
    {
        NChannel = false,
        PChannel = true
    };

protected:
    Pin m_pins[3];
    CVR m_cvr[3];
    ChannelType m_type;
    double m_Kn, m_vTN, m_lambda;

public:
    MOSFET(Circuit *circuit, ChannelType type, double turnOnVoltage = 1,
           double conductivityConstant = 1, double channelLengthModulation = 0);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;
    void setChannelType(ChannelType type);
    ChannelType channelType() const;
    void setConductivityConstant(double conductivityConstant);
    double conductivityConstant() const;
    void setTurnOnVoltage(double turnOnVoltage);
    double turnOnVoltage() const;
    void setPitchOffVoltage(double pitchOffVoltage);
    double pitchOffVoltage() const;
    void setChannelLengthModulation(double channelLengthModulation);
    double channelLengthModulation() const;
    void tick(double time, double deltaTime) override;
    void iterate(double time, double deltaTime) override;
    double error() override;
    double drainCurrent(double gateVoltage, double drainVoltage) const;
    double acConductivity(double gateVoltage, double drainVoltage) const;

#ifdef QT_DEBUG
    QString debug() override;
#endif
};

} // namespace CirSim

#endif // CIRSIM_MOSFET_H
