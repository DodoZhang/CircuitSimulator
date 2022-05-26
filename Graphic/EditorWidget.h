#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QVector>
#include <QList>
#include <QMap>

class QMenu;

namespace Editor {

class Element;
class Wire;
typedef QPoint CurrentProbe;
typedef QPoint VoltageProbe;

}

class EditorWidget : public QWidget
{
    Q_OBJECT
    friend class Editor::Element;
    friend class Editor::Wire;
protected:
    float m_posx, m_posy;
    float m_scale;
    QList<Editor::Element *> m_elements;
    QList<Editor::Wire *> m_wires;
    QMap<Editor::Element *, QVector<Editor::Wire *>> m_wireMap;
    QPixmap *m_pixmap;

private:
    QPoint m_prevMousePos;
    QPoint m_oriElementPos;
    bool m_isDragingElement;
    bool m_isDragingScreen;
    Editor::Element *m_selectedElement;
    Editor::Wire *m_selectedWire;
    bool m_unselectWhenRelease;
    bool m_placingWire;
    bool m_placeWireWhenRelease;

public:
    explicit EditorWidget(QWidget *parent = nullptr);
    void createContextMenu(QMenu *menu, QPoint pos);

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
};

#endif // EDITORWIDGET_H
