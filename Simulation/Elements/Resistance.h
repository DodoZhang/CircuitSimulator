#ifndef CIRSIM_RESISTANCE_H
#define CIRSIM_RESISTANCE_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class Resistance : public Element
{
protected:
    Pin m_pins[2];
    CVR m_cvr[2];

public:
    Resistance(Circuit *circuit, double resistance);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;

    void setResistance(double resistance);
    double resistance() const;
};

} // namespace CirSim

#endif // CIRSIM_RESISTANCE_H
