#ifndef CIRSIM_RESISTANCE_H
#define CIRSIM_RESISTANCE_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class Resistor : public Element
{
protected:
    Pin m_pins[2];
    CVR m_cvr[2];

public:
    Resistor(Circuit *circuit, double resistance);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;

    void setResistance(double resistance);
    double resistance() const;

#ifdef QT_DEBUG
    QString debug() override;
#endif
};

} // namespace CirSim

#endif // CIRSIM_RESISTANCE_H
