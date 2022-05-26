#ifndef EDITOR_RESISTANCE_H
#define EDITOR_RESISTANCE_H

#include "../EditorElement.h"

namespace Editor {

class Resistance : public Element
{
protected:
    double m_resistance;
    ParametersInputWidget *m_inspector;

public:
    Resistance(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North, double resistance = 1000);
    ~Resistance();
    int pinCount() const override;
    ParametersInputWidget *inspectorWidget() override;
    QVector<CirSim::Pin *> createElement(CirSim::Circuit *circuit) override;

protected:
    QRect originalRect() const override;
    QPoint originalPinPos(int index) const override;
    void paintEvent(QPainter *painter) override;
};

} // namespace Editor

#endif // EDITOR_RESISTANCE_H
