#include "EditorResistor.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/Resistor.h"

using namespace Editor;

Resistor::Resistor(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_resistance = 1000;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_resistance, QObject::tr("Resisitance"), PIWItemType(double));
}

Resistor::~Resistor()
{
    delete m_inspector;
}

QRect Resistor::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int Resistor::pinCount() const
{
    return 2;
}

QPoint Resistor::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void Resistor::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(-2, 0, -1, 0);
    painter->drawLine(QLineF(-1, -0.375, -1, 0.375));
    painter->drawLine(QLineF(-1, -0.375, 1, -0.375));
    painter->drawLine(QLineF(-1, 0.375, 1, 0.375));
    painter->drawLine(QLineF(1, -0.375, 1, 0.375));
    painter->drawLine(1, 0, 2, 0);
}

ParametersInputWidget *Resistor::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Resistor::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::Resistor(circuit, m_resistance);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString Resistor::typeName()
{
    return elementName<Resistor>();
}

QJsonObject Resistor::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("resistance", m_resistance);
    return json;
}

void Resistor::fromJson(const QJsonObject &json)
{
    Element::fromJson(json);
    m_resistance = json["resistance"].toDouble();
}
