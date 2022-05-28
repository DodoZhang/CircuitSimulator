#ifndef EDITOR_MOSFET_H
#define EDITOR_MOSFET_H

#include "../EditorElement.h"

namespace Editor {

class MOSFET: public Element
{
protected:
    enum ChannelType : bool
    {
        NChannel = false,
        PChannel = true
    } m_type;
    double m_turnOnVoltage, m_conductivityConstant, m_channelLengthModulation;
    ParametersInputWidget *m_inspector;

public:
    MOSFET(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    ~MOSFET();
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

protected slots:
    void parameterChanged(void *parameter);
};

} // namespace Editor

#endif // EDITOR_MOSFET_H
