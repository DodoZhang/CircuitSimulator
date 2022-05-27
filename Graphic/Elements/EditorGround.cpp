#include "EditorGround.h"

#include <QRect>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Net.h"
#include "Simulation/Elements/VoltageSource.h"

using namespace Editor;

Ground::Ground(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_inspector = new ParametersInputWidget();
}

Ground::~Ground()
{
    delete m_inspector;
}

QRect Ground::originalRect() const
{
    return QRect(-1, 0, 2, 2);
}

int Ground::pinCount() const
{
    return 1;
}

QPoint Ground::originalPinPos(int) const
{
    return QPoint(0, 0);
}

void Ground::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(0, 0, 0, 1);
    painter->drawLine(QLineF(-0.75, 1, 0.75, 1));
    painter->drawLine(QLineF(-0.5, 1.25, 0.5, 1.25));
    painter->drawLine(QLineF(-0.25, 1.5, 0.25, 1.5));
}

ParametersInputWidget *Ground::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Ground::createElement(CirSim::Circuit *circuit)
{
    auto *s = new CirSim::VoltageSource(circuit, 0);
    QVector<CirSim::Pin *> pins;
    if (circuit->ground()) CirSim::Net::merge(s->pin(0)->net(), circuit->ground());
    else circuit->setGround(s->pin(0)->net());
    pins.append(s->pin(1));
    return pins;
}

