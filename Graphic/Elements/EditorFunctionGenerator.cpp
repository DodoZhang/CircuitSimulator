#include "EditorFunctionGenerator.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/FunctionGenerator.h"

using namespace Editor;

FunctionGenerator::FunctionGenerator(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation), m_function("5")
{
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_function, QObject::tr("Function"), PIWItemType(InfixExpression));
}

FunctionGenerator::~FunctionGenerator()
{
    delete m_inspector;
}

QRect FunctionGenerator::originalRect() const
{
    return QRect(-2, -1, 4, 2);
}

int FunctionGenerator::pinCount() const
{
    return 2;
}

QPoint FunctionGenerator::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(-2, 0);
    case 1: return QPoint(2, 0);
    default: return QPoint();
    }
}

void FunctionGenerator::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(-2, 0, -1, 0);
    painter->drawEllipse(-1, -1, 2, 2);
    painter->drawLine(QLineF(-0.7, 0, -0.4, 0));
    painter->drawLine(QLineF(0.4, 0, 0.7, 0));
    painter->drawLine(QLineF(0.55, -0.15, 0.55, 0.15));
    painter->drawLine(1, 0, 2, 0);
    painter->scale(0.0625, 0.0625);
    painter->drawText(QRectF(-15.5, -5, 32, 10), Qt::AlignCenter, "f");
}

ParametersInputWidget *FunctionGenerator::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> FunctionGenerator::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::FunctionGenerator(circuit, m_function.get());
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    return pins;
}

QString FunctionGenerator::typeName()
{
    return elementName<FunctionGenerator>();
}

QJsonObject FunctionGenerator::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("function", m_function.get());
    return json;
}

void FunctionGenerator::fromJson(const QJsonObject &json)
{
    m_function.set(json["function"].toString());
    Element::fromJson(json);
}
