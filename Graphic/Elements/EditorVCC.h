#ifndef EDITOR_VCC_H
#define EDITOR_VCC_H

#include "../EditorElement.h"

#include <QString>

namespace Editor {

class VCC : public Element
{
protected:
    QString m_label;
    double m_voltage;
    ParametersInputWidget *m_inspector;

public:
    VCC(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~VCC();
    int pinCount() const override;
    ParametersInputWidget *inspectorWidget() override;
    QVector<CirSim::Pin *> createElement(CirSim::Circuit *circuit) override;

protected:
    QRect originalRect() const override;
    QPoint originalPinPos(int index) const override;
    void paintEvent(QPainter *painter) override;

private slots:
    void parameterChanged(void *parameter);
};

} // namespace Editor

#endif // EDITOR_VCC_H
