#ifndef EDITOR_CAPACITY_H
#define EDITOR_CAPACITY_H

#include "../EditorElement.h"

namespace Editor {

class Capacity : public Element
{
protected:
    double m_capacitance;
    double m_initVoltage;
    ParametersInputWidget *m_inspector;

public:
    Capacity(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~Capacity();
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

#endif // EDITOR_CAPACITY_H
