#ifndef EDITOR_RESISTANCE_H
#define EDITOR_RESISTANCE_H

#include "../EditorElement.h"

namespace Editor {

class Resistor : public Element
{
protected:
    double m_resistance;
    ParametersInputWidget *m_inspector;

public:
    Resistor(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~Resistor();
    int pinCount() const override;
    ParametersInputWidget *inspectorWidget() override;
    QVector<CirSim::Pin *> createElement(CirSim::Circuit *circuit) override;
    QString typeName() override;
    QJsonObject toJson() override;
    void fromJson(const QJsonObject &json) override;

protected:
    QRect originalRect() const override;
    QPoint originalPinPos(int index) const override;
    void paintEvent(QPainter *painter) override;
};

} // namespace Editor

#endif // EDITOR_RESISTANCE_H
