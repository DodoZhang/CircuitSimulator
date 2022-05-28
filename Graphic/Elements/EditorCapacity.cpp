#include "EditorCapacity.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/Capacity.h"

using namespace Editor;

Capacity::Capacity(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_capacitance = 1;
    m_initVoltage = 0;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_capacitance, QObject::tr("Capacitance"), PIWItemType(double));
    m_inspector->addParameter(&m_initVoltage, QObject::tr("Initial Voltage"), PIWItemType(double));
}

Capacity::~Capacity()
{
    delete m_inspector;
}

QRect Capacity::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int Capacity::pinCount() const
{
    return 2;
}

QPoint Capacity::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void Capacity::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(QLineF(-2, 0, -0.2, 0));
    painter->drawLine(QLineF(-0.2, -0.8, -0.2, 0.8));
    painter->drawLine(QLineF(0.2, -0.8, 0.2, 0.8));
    painter->drawLine(QLineF(0.2, 0, 2, 0));
}

ParametersInputWidget *Capacity::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Capacity::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::Capacity(circuit, m_capacitance, m_initVoltage);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString Capacity::typeName()
{
    return elementName<Capacity>();
}

QJsonObject Capacity::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("capacitance", m_capacitance);
    json.insert("initial voltage", m_initVoltage);
    return json;
}

void Capacity::fromJson(const QJsonObject &json)
{
    m_capacitance = json["capacitance"].toDouble();
    m_initVoltage = json["initial voltage"].toDouble();
    Element::fromJson(json);
}
