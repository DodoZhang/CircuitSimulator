#ifndef CIRSIM_CAPACITY_H
#define CIRSIM_CAPACITY_H

#include "VoltageSource.h"

namespace CirSim {

class Capacity : public VoltageSource
{
protected:
    double m_capacitance;

public:
    Capacity(Circuit *circuit, double capacitance, double initVoltage = 0);
    void setCapacitance(double capacitance);
    double capacitance() const;
    void tick(double time, double deltaTime) override;
};

} // namespace CirSim

#endif // CIRSIM_CAPACITY_H
