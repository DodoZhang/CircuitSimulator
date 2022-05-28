#include "EditorCurrentSource.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/CurrentSource.h"

using namespace Editor;

CurrentSource::CurrentSource(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_current = 1;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_current, QObject::tr("Current"), PIWItemType(double));
}

CurrentSource::~CurrentSource()
{
    delete m_inspector;
}

QRect CurrentSource::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int CurrentSource::pinCount() const
{
    return 2;
}

QPoint CurrentSource::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void CurrentSource::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(-2, 0, -1, 0);
    painter->drawEllipse(-1, -1, 2, 2);
    painter->drawLine(QLineF(-0.6, 0, 0.6, 0));
    painter->setBrush(QBrush(QColor(32, 32, 32)));
    static const QPointF points[3] = {
        QPointF(0.6, 0.0),
        QPointF(0.4, 0.15),
        QPointF(0.4, -0.15)
    };
    painter->drawConvexPolygon(points, 3);
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(1, 0, 2, 0);
}

ParametersInputWidget *CurrentSource::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> CurrentSource::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::CurrentSource(circuit, m_current);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString CurrentSource::typeName()
{
    return elementName<CurrentSource>();
}

QJsonObject CurrentSource::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("current", m_current);
    return json;
}

void CurrentSource::fromJson(const QJsonObject &json)
{
    m_current = json["current"].toDouble();
    Element::fromJson(json);
}
