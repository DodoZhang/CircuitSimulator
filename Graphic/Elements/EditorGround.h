#ifndef EDITOR_GROUND_H
#define EDITOR_GROUND_H

#include "../EditorElement.h"

namespace Editor {

class Ground : public Element
{
protected:
    ParametersInputWidget *m_inspector;

public:
    Ground(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~Ground();
    int pinCount() const override;
    ParametersInputWidget *inspectorWidget() override;
    QVector<CirSim::Pin *> createElement(CirSim::Circuit *circuit) override;
    QString typeName() override;

protected:
    QRect originalRect() const override;
    QPoint originalPinPos(int index) const override;
    void paintEvent(QPainter *painter) override;
};

} // namespace Editor

#endif // EDITOR_GROUND_H
