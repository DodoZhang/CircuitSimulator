#ifndef EDITOR_WIRE_H
#define EDITOR_WIRE_H

#include <QObject>
#include <QVector>
#include <QPoint>

class EditorWidget;

namespace Editor {

class Element;

class Wire : QObject
{
    Q_OBJECT
protected:
    Element *m_elements[2];
    int m_pins[2];
    QVector<QPoint> m_path;
    EditorWidget *m_widget;

public:
    Wire(EditorWidget *widget);
    ~Wire();
    void startRecording(Element *element, int pin);
    void recordNode(QPoint pos);
    void stopRecording(Element *element, int pin);
    Element *element(int index) const;
    int pin(int index) const;
    QVector<QPoint> path();
    const QVector<QPoint> path() const;
    bool onWire(QPoint point);
    bool nearWire(float x, float y);
    void updatePath();
};

} // namespace Editor

#endif // EDITOR_WIRE_H
