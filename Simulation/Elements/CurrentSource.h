#ifndef CURRENTSOURCE_H
#define CURRENTSOURCE_H

#include "../Element.h"
#include "../Pin.h"

namespace CirSim {

class CurrentSource : public Element
{
public:
    enum PinDefinition : int
    {
        Inflow = 0,
        Outflow = 1
    };

protected:
    Pin m_pins[2];
    CVR m_cvr[2];

public:
    CurrentSource(Circuit *circuit, double current);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;

    void setCurrent(double current);
    double current() const;
};

}

#endif // CURRENTSOURCE_H
