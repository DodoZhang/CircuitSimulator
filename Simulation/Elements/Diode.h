#ifndef CIRSIM_DIODE_H
#define CIRSIM_DIODE_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class Diode : public Element
{
public:
    enum PinDefinition : int
    {
        Positive = 0,
        Negative = 1
    };

protected:
    Pin m_pins[2];
    CVR m_cvr[2];
    double m_ICBO;

public:
    Diode(Circuit *circuit, double ICBO = 1e-12);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;
    void setICBO(double ICBO);
    double ICBO() const;
    void tick(double time, double deltaTime) override;
    void iterate(double time, double deltaTime) override;
    double error() override;
    double current(double voltage) const;
    double acConductivity(double voltage) const;
};

} // namespace CirSim

#endif // CIRSIM_DIODE_H
