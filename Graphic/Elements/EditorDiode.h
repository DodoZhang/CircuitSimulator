#ifndef EDITOR_DIODE_H
#define EDITOR_DIODE_H

#include "../EditorElement.h"

namespace Editor {

class Diode : public Element
{
protected:
    double m_ICBO;
    ParametersInputWidget *m_inspector;

public:
    Diode(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~Diode();
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

#endif // EDITOR_DIODE_H
