#ifndef EDITOR_CURRENTSOURCE_H
#define EDITOR_CURRENTSOURCE_H

#include "../EditorElement.h"

namespace Editor {

class CurrentSource : public Element
{
protected:
    double m_current;
    ParametersInputWidget *m_inspector;

public:
    CurrentSource(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~CurrentSource();
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

#endif // EDITOR_CURRENTSOURCE_H
