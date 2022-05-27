#ifndef EDITORELEMENT_H
#define EDITORELEMENT_H

#include <QObject>
#include <QMap>
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

class Element : public QObject
{
    Q_OBJECT
    friend class ::EditorWidget;
    friend class Wire;
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

private:
    static QMap<QString, Element *(*)(EditorWidget *)> s_insFunc;
    // Qt suggested to use QHash here, which would cause error
    static QMap<Element *(*)(EditorWidget *), QString> s_eleName;

public:
    template<typename T>
    static Element *instantiateElement(EditorWidget *widget)
    {
        return new T(widget);
    }
    template<typename T>
    static void registerElement(QString name)
    {
        s_insFunc.insert(name, &instantiateElement<T>);
        s_eleName.insert(&instantiateElement<T>, name);
    }
    static const QMap<QString, Element *(*)(EditorWidget *)> &elementMap()
    {
        return s_insFunc;
    }
    template<typename T>
    static QString elementName()
    {
        return s_eleName[&instantiateElement<T>];
    }
    static Element *instantiateFromJson(EditorWidget *widget, QJsonObject json);

protected:
    EditorWidget *m_widget;
    QPoint m_position;
    ElementRotation m_rotation;
    int m_index;

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
    virtual QString typeName() = 0;
    virtual QJsonObject toJson();
    virtual void fromJson(const QJsonObject &json);

protected:
    virtual QRect originalRect() const = 0;
    virtual QPoint originalPinPos(int index) const = 0;
    virtual void paintEvent(QPainter *painter) = 0;
};

}

#endif // EDITORELEMENT_H
