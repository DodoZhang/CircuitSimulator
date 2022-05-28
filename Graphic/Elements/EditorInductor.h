#ifndef EDITOR_INDUCTOR_H
#define EDITOR_INDUCTOR_H

#include "../EditorElement.h"

namespace Editor {

class Inductor : public Element
{
protected:
    double m_inductance;
    double m_initCurrent;
    ParametersInputWidget *m_inspector;

public:
    Inductor(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~Inductor();
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

#endif // EDITOR_INDUCTOR_H
