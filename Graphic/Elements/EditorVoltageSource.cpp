#include "EditorVoltageSource.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/VoltageSource.h"

using namespace Editor;

VoltageSource::VoltageSource(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_voltage = 5;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_voltage, QObject::tr("Voltage"), PIWItemType(double));
}

VoltageSource::~VoltageSource()
{
    delete m_inspector;
}

QRect VoltageSource::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int VoltageSource::pinCount() const
{
    return 2;
}

QPoint VoltageSource::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void VoltageSource::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(-2, 0, -1, 0);
    painter->drawEllipse(-1, -1, 2, 2);
    painter->drawLine(QLineF(-0.7, 0, -0.4, 0));
    painter->drawLine(QLineF(0.4, 0, 0.7, 0));
    painter->drawLine(QLineF(0.55, -0.15, 0.55, 0.15));
    painter->drawLine(1, 0, 2, 0);
}

ParametersInputWidget *VoltageSource::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> VoltageSource::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::VoltageSource(circuit, m_voltage);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString VoltageSource::typeName()
{
    return elementName<VoltageSource>();
}

QJsonObject VoltageSource::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("voltage", m_voltage);
    return json;
}

void VoltageSource::fromJson(const QJsonObject &json)
{
    m_voltage = json["voltage"].toDouble();
    Element::fromJson(json);
}
