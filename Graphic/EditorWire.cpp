#include "EditorWire.h"

#include "EditorWidget.h"
#include "EditorElement.h"

using namespace Editor;

Wire::Wire(EditorWidget *widget)
{
    m_widget = widget;
    m_widget->m_wires.append(this);
    m_elements[0] = nullptr;
    m_elements[1] = nullptr;
    m_widget->update();
}

Wire::~Wire()
{
    m_widget->m_wires.removeOne(this);
    if (m_elements[0]) m_widget->m_wireMap[m_elements[0]].removeOne(this);
    if (m_elements[1]) m_widget->m_wireMap[m_elements[1]].removeOne(this);
}

Element *Wire::element(int index) const
{
    return m_elements[index];
}

int Wire::pin(int index) const
{
    return m_pins[index];
}

void Wire::startRecording(Element *element, int pin)
{
    m_elements[0] = element;
    m_pins[0] = pin;
    m_path.append(element->pinPos(pin));
    m_widget->m_wireMap[element].append(this);
    m_widget->update();
}

void Wire::recordNode(QPoint pos)
{
    m_path.append(pos);
    m_widget->update();
}

void Wire::stopRecording(Element *element, int pin)
{
    m_elements[1] = element;
    m_pins[1] = pin;
    m_path.append(element->pinPos(pin));
    m_widget->m_wireMap[element].append(this);
    m_widget->update();
}

QVector<QPoint> Wire::path()
{
    return m_path;
}

const QVector<QPoint> Wire::path() const
{
    return m_path;
}

bool Wire::onWire(QPoint point)
{
    for (int i = 0, s = m_path.count() - 1; i < s; i ++)
    {
        if (m_path[i].x() == m_path[i + 1].x())
        {
            if (point.y() < m_path[i].y() && point.y() < m_path[i + 1].y()) continue;
            if (point.y() > m_path[i].y() && point.y() > m_path[i + 1].y()) continue;
        }
        else
        {
            if (point.x() < m_path[i].x() && point.x() < m_path[i + 1].x()) continue;
            if (point.x() > m_path[i].x() && point.x() > m_path[i + 1].x()) continue;
        }
        QPoint d1 = m_path[i + 1] - m_path[i];
        QPoint d2 = point - m_path[i];
        if (d1.x() * d2.y() == d1.y() * d2.x()) return true;
    }
    return false;
}

bool Wire::nearWire(float x, float y)
{
    for (int i = 0, s = m_path.count() - 1; i < s; i ++)
    {
        if (m_path[i].x() == m_path[i + 1].x())
        {
            if (y + 0.25f < m_path[i].y() && y + 0.25f < m_path[i + 1].y()) continue;
            if (y - 0.25f > m_path[i].y() && y - 0.25f > m_path[i + 1].y()) continue;
        }
        else
        {
            if (x + 0.25f < m_path[i].x() && x + 0.25f < m_path[i + 1].x()) continue;
            if (x - 0.25f > m_path[i].x() && x - 0.25f > m_path[i + 1].x()) continue;
        }

        float A = m_path[i + 1].y() - m_path[i].y(), B = m_path[i + 1].x() - m_path[i].x();
        float DX = A * (x - m_path[i].x()) - B * (y - m_path[i].y());
        if (DX * DX / (A * A + B * B) < 0.25f) return true;
    }
    return false;
}

void Wire::updatePath()
{
    m_path.first() = m_elements[0]->pinPos(m_pins[0]);
    m_path.last() = m_elements[1]->pinPos(m_pins[1]);
}
