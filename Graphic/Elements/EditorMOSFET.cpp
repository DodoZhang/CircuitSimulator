#include "EditorMOSFET.h"

#include <QRect>
#include <QJsonObject>
#include <QPainter>

#include "Graphic/EditorWidget.h"
#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Elements/MOSFET.h"

using namespace Editor;

MOSFET::MOSFET(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_type = NChannel;
    m_turnOnVoltage = 1;
    m_conductivityConstant = 1;
    m_channelLengthModulation = 0;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_type, QObject::tr("Channel Type"), [](const void *parameter) {
        return QString(*((ChannelType *) parameter) ? "P" : "N");
    }, [](void *parameter, const QString &str) {
        if (str == "n" || str == "N")
        {
            *((ChannelType *) parameter) = NChannel;
            return true;
        }
        if (str == "p" || str == "P")
        {
            *((ChannelType *) parameter) = PChannel;
            return true;
        }
        return false;
    });
    m_inspector->addParameter(&m_turnOnVoltage, QObject::tr("Turn On/Pinch Off Voltage"), PIWItemType(double));
    m_inspector->addParameter(&m_conductivityConstant, QObject::tr("Conductivity Constant"), PIWItemType(double));
    m_inspector->addParameter(&m_channelLengthModulation, QObject::tr("Channel Length Modulation"), PIWItemType(double));
    connect(m_inspector, &ParametersInputWidget::parameterChanged, this, &MOSFET::parameterChanged);
}

MOSFET::~MOSFET()
{
    delete m_inspector;
}

QRect MOSFET::originalRect() const
{
    return QRect(-1, -2, 3, 4);
}

int MOSFET::pinCount() const
{
    return 3;
}

QPoint MOSFET::originalPinPos(int index) const
{
    switch (index) {
    case 0: return QPoint(1, -2);
    case 1: return QPoint(-1, 1);
    case 2: return QPoint(1, 2);
    default: return QPoint();
    }
}

void MOSFET::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter->setBrush(QBrush(QColor(32, 32, 32)));
    painter->drawLine(QLineF(-1, 1, -0.25, 1));
    painter->drawLine(QLineF(-0.25, -1, -0.25, 1));
    painter->drawLine(QLineF(0, -0.9, 1, -0.9));
    painter->drawLine(QLineF(0, 0, 1, 0));
    painter->drawLine(QLineF(0, 0.9, 1, 0.9));
    painter->drawLine(QLineF(1, -2, 1, -0.9));
    painter->drawLine(QLineF(1, 0, 1, 2));
    if (m_type == NChannel)
    {
        if (m_turnOnVoltage >= 0)
            painter->drawLine(QLineF(0, -1.1, 0, 1.1));
        else
        {
            painter->drawLine(QLineF(0, -1.2, 0, -0.6));
            painter->drawLine(QLineF(0, -0.3, 0, 0.3));
            painter->drawLine(QLineF(0, 0.6, 0, 1.2));
        }
        static const QPointF arrowN[3] = {
            QPointF(0.4, 0.0),
            QPointF(0.6, 0.15),
            QPointF(0.6, -0.15)
        };
        painter->drawConvexPolygon(arrowN, 3);
    }
    else
    {
        if (m_turnOnVoltage < 0)
            painter->drawLine(QLineF(0, -1.1, 0, 1.1));
        else
        {
            painter->drawLine(QLineF(0, -1.2, 0, -0.6));
            painter->drawLine(QLineF(0, -0.3, 0, 0.3));
            painter->drawLine(QLineF(0, 0.6, 0, 1.2));
        }
        static const QPointF arrowP[3] = {
            QPointF(0.6, 0.0),
            QPointF(0.4, 0.15),
            QPointF(0.4, -0.15)
        };
        painter->drawConvexPolygon(arrowP, 3);
    }
}

ParametersInputWidget *MOSFET::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> MOSFET::createElement(CirSim::Circuit *circuit)
{
    auto *r = new CirSim::MOSFET(circuit, (CirSim::MOSFET::ChannelType) m_type, m_turnOnVoltage,
                                 m_conductivityConstant, m_channelLengthModulation);
    QVector<CirSim::Pin *> pins;
    pins.append(r->pin(0));
    pins.append(r->pin(1));
    pins.append(r->pin(2));
    return pins;
}

QString MOSFET::typeName()
{
    return elementName<MOSFET>();
}

QJsonObject MOSFET::toJson()
{
    QJsonObject json = Element::toJson();
    json.insert("type", (bool) m_type);
    json.insert("turn on voltage", m_turnOnVoltage);
    json.insert("conductivity constant", m_conductivityConstant);
    json.insert("channel length modulation", m_channelLengthModulation);
    return json;
}

void MOSFET::fromJson(const QJsonObject &json)
{
    m_type = (ChannelType) json["type"].toBool();
    m_turnOnVoltage = json["turn on voltage"].toDouble();
    m_conductivityConstant = json["conductivity constant"].toDouble();
    m_channelLengthModulation = json["channel length modulation"].toDouble();
    Element::fromJson(json);
}

void MOSFET::parameterChanged(void *)
{
    m_widget->update();
}
