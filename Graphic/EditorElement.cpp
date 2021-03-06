#include "EditorElement.h"

#include <QRect>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>

#include "ParametersInputWidget.h"
#include "EditorWidget.h"
#include "EditorWire.h"

using namespace Editor;

QMap<QString, Element *(*)(EditorWidget *)> Element::s_insFunc;
QMap<Element *(*)(EditorWidget *), QString> Element::s_eleName;
QMap<Element *(*)(EditorWidget *), QString> Element::s_dispName;

Element *Element::instantiateFromJson(EditorWidget *widget, QJsonObject json)
{
    Element *element = s_insFunc[json["type"].toString()](widget);
    element->fromJson(json);
    return element;
}

Element::Element(EditorWidget *widget, QPoint position, ElementRotation rotation)
    : QObject(widget)
{
    m_widget = widget;
    widget->m_elements.append(this);
    widget->m_wireMap.insert(this, QVector<Wire *>());
    m_position = position;
    m_rotation = rotation;
    m_widget->update();
}

Element::~Element()
{
    m_widget->m_elements.removeOne(this);
    QVector<Wire *> vec = m_widget->m_wireMap[this];
    for (int i = 0, s = vec.count(); i < s; i ++) delete vec[i];
    m_widget->m_wireMap.remove(this);
}

QPoint Element::position() const
{
    return m_position;
}

void Element::setPosition(QPoint position)
{
    m_position = position;
}

Element::ElementRotation Element::rotation() const
{
    return m_rotation;
}

void Element::setRotation(ElementRotation rotation)
{
    m_rotation = rotation;
}

QRect Element::rect()
{
    QRect ori = originalRect();
    switch (m_rotation & DirMask) {
    case North: break;
    case  West: ori = QRect(ori.y(), -ori.width() - ori.x(), ori.height(), ori.width()); break;
    case South: ori = QRect(-ori.width() - ori.x(), -ori.height() - ori.y(), ori.width(), ori.height()); break;
    case  East: ori = QRect(-ori.height() - ori.y(), ori.x(), ori.height(), ori.width()); break;
    }
    if (m_rotation & HorFliped) ori = QRect(-ori.width() - ori.x(), ori.y(), ori.width(), ori.height());
    if (m_rotation & VerFliped) ori = QRect(ori.x(), -ori.height() - ori.y(), ori.width(), ori.height());
    return ori.translated(m_position);
}

QPoint Element::pinPos(int index)
{
    QPoint tmp = originalPinPos(index);
    if (m_rotation & HorFliped) tmp.setX(-tmp.x());
    if (m_rotation & VerFliped) tmp.setY(-tmp.y());
    switch (m_rotation & DirMask) {
    case North: break;
    case  West: tmp = QPoint( tmp.y(), -tmp.x()); break;
    case South: tmp = QPoint(-tmp.x(), -tmp.y()); break;
    case  East: tmp = QPoint(-tmp.y(),  tmp.x()); break;
    }
    return tmp + m_position;
}

void Element::paint(QPainter *painter)
{
    painter->save();
    switch (m_rotation & DirMask) {
    case North: break;
    case  West: painter->rotate(-90); break;
    case South: painter->rotate(180); break;
    case  East: painter->rotate(90); break;
    }
    if (m_rotation & HorFliped) painter->scale(-1, 1);
    if (m_rotation & VerFliped) painter->scale(1, -1);
    paintEvent(painter);
    painter->restore();
}

QJsonObject Element::toJson()
{
    QJsonObject json;
    json.insert("type", typeName());
    json.insert("position", QJsonArray { m_position.x(), m_position.y() });
    json.insert("rotation", (int) m_rotation);
    return json;
}

void Element::fromJson(const QJsonObject &json)
{
    QJsonArray posArray = json["position"].toArray();
    m_position = QPoint(posArray[0].toInt(), posArray[1].toInt());
    m_rotation = (ElementRotation) json["rotation"].toInt();
    inspectorWidget()->updateParameters();
}
