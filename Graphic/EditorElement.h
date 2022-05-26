#ifndef EDITORELEMENT_H
#define EDITORELEMENT_H

#include <QPoint>

class QRect;
class QPainter;

namespace CirSim {

class Circuit;
class Element;
class Pin;
class Net;

}

class EditorWidget;
class ParametersInputWidget;

namespace Editor {

class Element
{
public:
    enum ElementRotation : unsigned short
    {
        North = 0,
        West = 1,
        South = 2,
        East = 3,
        HorFliped = 4,
        VerFliped = 8,
        DirMask = 3,
        FlipMask = 12
    };

protected:
    EditorWidget *m_widget;
    QPoint m_position;
    ElementRotation m_rotation;

public:
    Element(EditorWidget *widget, QPoint position = QPoint(), ElementRotation rotation = North);
    virtual ~Element();
    QPoint position() const;
    void setPosition(QPoint position);
    ElementRotation rotation() const;
    void setRotation(ElementRotation rotation);
    QRect rect();
    QPoint pinPos(int index);
    void paint(QPainter *painter);
    virtual int pinCount() const = 0;
    virtual ParametersInputWidget *inspectorWidget() = 0;
    virtual QVector<CirSim::Pin *> createElement(CirSim::Circuit *circuit) = 0;

protected:
    virtual QRect originalRect() const = 0;
    virtual QPoint originalPinPos(int index) const = 0;
    virtual void paintEvent(QPainter *painter) = 0;
};

}

#endif // EDITORELEMENT_H
