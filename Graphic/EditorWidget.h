#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QVector>
#include <QList>
#include <QMap>

class QMenu;

class MainWindow;
class ParametersInputWidget;

namespace Editor {
class Element;
class Wire;
}

class EditorWidget : public QWidget
{
    Q_OBJECT
    friend class Editor::Element;
    friend class Editor::Wire;
protected:
    class CurrentProbe
    {
    private:
        static int count;
    public:
        Editor::Element *element;
        int pin;
        QString label;
        ParametersInputWidget *inspector;
    public:
        CurrentProbe(EditorWidget *editor, Editor::Element *element = nullptr, int pin = 0);
        ~CurrentProbe();
        void paint(QPainter *painter);
        QJsonObject toJson();
        CurrentProbe *fromJson(EditorWidget *editor, const QJsonObject &json);
    };

    class VoltageProbe
    {
    private:
        static int count;
    public:
        QPoint pos;
        QString label;
        ParametersInputWidget *inspector;
    public:
        VoltageProbe(EditorWidget *editor, QPoint pos = QPoint());
        ~VoltageProbe();
        void paint(QPainter *painter);
        QJsonObject toJson();
        VoltageProbe *fromJson(const QJsonObject &json);
    };

protected:
    float m_posx, m_posy;
    float m_scale;
    QList<Editor::Element *> m_elements;
    QList<Editor::Wire *> m_wires;
    QMap<Editor::Element *, QVector<Editor::Wire *>> m_wireMap;
    QList<CurrentProbe *> m_currentProbes;
    QList<VoltageProbe *> m_voltageProbes;
    ParametersInputWidget *m_inspector;
    QPixmap *m_pixmap;

private:
    QPoint m_prevMousePos;
    QPoint m_oriElementPos;
    bool m_isDragingElement;
    bool m_isDragingScreen;
    Editor::Element *m_selectedElement;
    Editor::Wire *m_selectedWire;
    CurrentProbe *m_selectedCurProbe;
    VoltageProbe *m_selectedVolProbe;
    bool m_unselectWhenRelease;
    bool m_placingWire;
    bool m_placeWireWhenRelease;
    bool m_placingCurProbe;
    bool m_placeCurProbeWhenRelease;

public:
    explicit EditorWidget(MainWindow *parent = nullptr);
    ~EditorWidget();
    ParametersInputWidget *inspector();
    void createContextMenu(QMenu *menu, QPoint pos);
    QJsonObject toJson();
    void fromJson(const QJsonObject &json);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected slots:
    void rotateElementCCW();
    void rotateElementCW();
    void flipHorizontal();
    void flipVertical();
    void startPlacingWire();
    void startPlacingCurProbe();
    void startSimultaion();
    void stopSimultaion();
    void updateSlot();
};

#endif // EDITORWIDGET_H
