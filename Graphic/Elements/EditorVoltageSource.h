#ifndef EDITOR_VOLTAGESOURCE_H
#define EDITOR_VOLTAGESOURCE_H

#include "../EditorElement.h"

namespace Editor {

class VoltageSource : public Element
{
protected:
    double m_voltage;
    ParametersInputWidget *m_inspector;

public:
    VoltageSource(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~VoltageSource();
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

#endif // EDITOR_VOLTAGESOURCE_H
