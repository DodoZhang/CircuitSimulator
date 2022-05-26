#include "EditorResistance.h"

#include <QRect>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/Resistance.h"

using namespace Editor;

Resistance::Resistance(EditorWidget *widget, QPoint position, ElementRotation rotation, double resistance)
    : Element(widget, position, rotation)
{
    m_resistance = resistance;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_resistance, QObject::tr("Resisitance"), PIWItemType(double));
}

Resistance::~Resistance()
{
    delete m_inspector;
}

QRect Resistance::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int Resistance::pinCount() const
{
    return 2;
}

QPoint Resistance::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void Resistance::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(-2, 0, -1, 0);
    painter->drawLine(QLineF(-1, -0.375, -1, 0.375));
    painter->drawLine(QLineF(-1, -0.375, 1, -0.375));
    painter->drawLine(QLineF(-1, 0.375, 1, 0.375));
    painter->drawLine(QLineF(1, -0.375, 1, 0.375));
    painter->drawLine(1, 0, 2, 0);
}

ParametersInputWidget *Resistance::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Resistance::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::Resistance(circuit, m_resistance);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}
