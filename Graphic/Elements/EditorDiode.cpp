#include "EditorDiode.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/Diode.h"

using namespace Editor;

Diode::Diode(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_ICBO = 1e-12;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_ICBO, QObject::tr("ICBO"), PIWItemType(double));
}

Diode::~Diode()
{
    delete m_inspector;
}

QRect Diode::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int Diode::pinCount() const
{
    return 2;
}

QPoint Diode::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void Diode::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter->setBrush(QBrush(QColor(32, 32, 32)));
    painter->drawLine(-2, 0, 2, 0);
    static const QPointF points[3] = {
        QPointF(-0.5, -0.5),
        QPointF(-0.5, 0.5),
        QPointF(0.5, 0)
    };
    painter->drawConvexPolygon(points, 3);
    painter->drawLine(QLineF(0.5, -0.5, 0.5, 0.5));
}

ParametersInputWidget *Diode::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> Diode::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::Diode(circuit, m_ICBO);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString Diode::typeName()
{
    return elementName<Diode>();
}

QJsonObject Diode::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("icbo", m_ICBO);
    return json;
}

void Diode::fromJson(const QJsonObject &json)
{
    m_ICBO = json["icbo"].toDouble();
    Element::fromJson(json);
}
