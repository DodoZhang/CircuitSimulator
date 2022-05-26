#ifndef CIRSIM_FUNCTIONGENERATOR_H
#define CIRSIM_FUNCTIONGENERATOR_H

#include "../Element.h"
#include "../Pin.h"
#include "InfixExpression.h"

#define SIN_WAVE(amp, freq) "(("#amp") * sin(6.2831853072 * ("#freq") * t))"
#define SQUARE_WAVE(amp, freq) "(("#amp") * (t % (1 / ("#freq")) * ("#freq") < 0.5 ? 1 : -1))"
#define TRAPEZOIDAL_WAVE(amp, freq, tran) "(("#amp") * (t % (1 / ("#freq")) * ("#freq") + ("#tran") / 4) % 1 < 0.5 ? "        \
                                          "((t % (1 / ("#freq")) * ("#freq") + ("#tran") / 4) % 1 < ("#tran") / 2 ? "         \
                                          "(t % (1 / ("#freq")) * ("#freq") + ("#tran") / 4) % 1 * 4 / ("#tran") - 1 : 1) : " \
                                          "(t % (1 / ("#freq")) * ("#freq") + ("#tran") / 4) % 1 - 0.5 < ("#tran") / 2 ? "    \
                                          "1 - ((t % (1 / ("#freq")) * ("#freq") + ("#tran") / 4) % 1 - 0.5) * 4 / ("#tran") : -1)"
#define TRIANGLE_WAVE(amp, freq) "(("#amp") * ((t % (0.5 / ("#freq")) * ("#freq") + 0.25) % 0.5 * 4 - 1) * " \
                                 "((t % (1 / ("#freq")) * ("#freq") + 0.25) % 1 < 0.5 ? 1 : -1))"
#define SAW_WAVE(amp, freq) "(("#amp") * (t % (1 / ("#freq")) * ("#freq") * 2 - 1))"

namespace CirSim {

class FunctionGenerator : public Element
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
    InfixExpression m_function;

private:
    QMap<QString, double> m_parameters;

public:
    FunctionGenerator(Circuit *circuit, const QString &function);
    int pinCount() const override;
    Pin *pin(int index) override;
    const Pin *pin(int index) const override;
    const CVR *cvr(int index) const override;
    bool setFunction(const QString &function);
    const QString &function() const;
    void tick(double time, double deltaTime) override;

#ifdef QT_DEBUG
    QString debug() override;
#endif
};

} // namespace CirSim

#endif // CIRSIM_FUNCTIONGENERATOR_H
