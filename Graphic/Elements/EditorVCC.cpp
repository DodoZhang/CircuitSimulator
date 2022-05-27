#include "EditorVCC.h"

#include <QRect>
#include <QPainter>

#include "../EditorWidget.h"
#include "Graphic/ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Net.h"
#include "Simulation/Elements/VoltageSource.h"

using namespace Editor;

VCC::VCC(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : Element(widget, position, rotation)
{
    m_label = "VCC";
    m_voltage = 5;
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_label, QObject::tr("Label"), PIWItemType(QString));
    m_inspector->addParameter(&m_voltage, QObject::tr("Voltage"), PIWItemType(double));
    connect(m_inspector, &ParametersInputWidget::parameterChanged, this, &VCC::parameterChanged);
}

VCC::~VCC()
{
    delete m_inspector;
}

QRect VCC::originalRect() const
{
    return QRect(-1, -2, 2, 2);
}

int VCC::pinCount() const
{
    return 1;
}

QPoint VCC::originalPinPos(int) const
{
    return QPoint(0, 0);
}

void VCC::paintEvent(QPainter *painter)
{
    painter->setPen(QPen(QColor(32, 32, 32), 0.125));
    painter->drawLine(0, -1, 0, 0);
    painter->drawLine(QLineF(-0.75, -1, 0.75, -1));
    painter->translate(0, -1);
    painter->scale(0.0625, 0.0625);
    painter->drawText(QRectF(-16, -16, 32, 16), Qt::AlignCenter, m_label);
}

ParametersInputWidget *VCC::inspectorWidget()
{
    return m_inspector;
}

QVector<CirSim::Pin *> VCC::createElement(CirSim::Circuit *circuit)
{
    auto *s = new CirSim::VoltageSource(circuit, m_voltage);
    QVector<CirSim::Pin *> pins;
    if (circuit->ground()) CirSim::Net::merge(s->pin(0)->net(), circuit->ground());
    else circuit->setGround(s->pin(0)->net());
    pins.append(s->pin(1));
    return pins;
}

void VCC::parameterChanged(void *paramenter)
{
    if (paramenter == &m_label) m_widget->update();
}
