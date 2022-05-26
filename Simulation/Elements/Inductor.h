#ifndef CIRSIM_INDUCTOR_H
#define CIRSIM_INDUCTOR_H

#include "CurrentSource.h"

namespace CirSim {

class Inductor : public CurrentSource
{
protected:
    double m_inductance;

public:
    Inductor(Circuit *circuit, double inductance, double initCurrent = 0);
    void setInductance(double inductance);
    double inductance() const;
    void tick(double time, double deltaTime) override;

#ifdef QT_DEBUG
    QString debug() override;
#endif
};

} // namespace CirSim

#endif // CIRSIM_INDUCTOR_H
