#include "EditorInductor.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/Inductor.h"

using namespace Editor;

Inductor::Inductor(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_inductance = 1;
    m_initCurrent = 0;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_inductance, QObject::tr("Inductance"), PIWItemType(double));
    m_inspector->addParameter(&m_initCurrent, QObject::tr("Initial Current"), PIWItemType(double));
}

Inductor::~Inductor()
{
    delete m_inspector;
}

QRect Inductor::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int Inductor::pinCount() const
{
    return 2;
}

QPoint Inductor::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void Inductor::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(QLineF(-2, 0, -1.5, 0));
    painter->drawArc(QRectF(-1.5, -0.375, 0.75, 0.75), 0, 2880);
    painter->drawArc(QRectF(-0.75, -0.375, 0.75, 0.75), 0, 2880);
    painter->drawArc(QRectF(0, -0.375, 0.75, 0.75), 0, 2880);
    painter->drawArc(QRectF(0.75, -0.375, 0.75, 0.75), 0, 2880);
    painter->drawLine(QLineF(1.5, 0, 2, 0));
}

ParametersInputWidget *Inductor::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Inductor::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::Inductor(circuit, m_inductance, m_initCurrent);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString Inductor::typeName()
{
    return elementName<Inductor>();
}

QJsonObject Inductor::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("inductance", m_inductance);
    json.insert("initial current", m_initCurrent);
    return json;
}

void Inductor::fromJson(const QJsonObject &json)
{
    m_inductance = json["inductance"].toDouble();
    m_initCurrent = json["initial current"].toDouble();
    Element::fromJson(json);
}
