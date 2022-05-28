#ifndef EDITOR_FUNCTIONGENERATOR_H
#define EDITOR_FUNCTIONGENERATOR_H

#include "../EditorElement.h"
#include "InfixExpression.h"

namespace Editor {

class FunctionGenerator : public Element
{
protected:
    InfixExpression m_function;
    ParametersInputWidget *m_inspector;

public:
    FunctionGenerator(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~FunctionGenerator();
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

#endif // EDITOR_FUNCTIONGENERATOR_H
