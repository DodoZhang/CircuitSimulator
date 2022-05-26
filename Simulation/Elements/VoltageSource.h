#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class VoltageSource : public Element
{
public:
    enum PinDefinition : int
    {
        Negative = 0,
        Positive = 1
    };

protected:
    Pin m_pins[2];
    CVR m_cvr[2];

public:
    VoltageSource(Circuit *circuit, double voltage);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;
    void setVoltage(double voltage);
    double voltage() const;

#ifdef QT_DEBUG
    QString debug() override;
#endif
};

}

#endif // VOLTAGESOURCE_H
