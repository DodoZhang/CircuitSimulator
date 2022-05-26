#include "EditorWidget.h"

#include <cmath>
#include <QPainter>
#include <QMenu>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "EditorElement.h"
#include "EditorWire.h"
#include "Elements/EditorResistance.h"

EditorWidget::EditorWidget(QWidget *parent) : QWidget(parent)
{
    m_posx = m_posy = 0;
    m_scale = 16;
    m_pixmap = new QPixmap(width(), height());
    m_isDragingElement = false;
    m_isDragingScreen = false;
    m_selectedElement = nullptr;
    m_selectedWire = nullptr;
    m_unselectWhenRelease = false;
    m_placingWire = false;
    m_placeWireWhenRelease = false;

    auto *R1 = new Editor::Resistance(this, QPoint(3, 3));
    auto *R2 = new Editor::Resistance(this, QPoint(8, 8), Editor::Element::West);
    auto *W = new Editor::Wire(this);
    W->startRecording(R1, 1);
    W->recordNode(QPoint(8, 3));
    W->stopRecording(R2, 1);

    setFocusPolicy(Qt::ClickFocus);
}

#define SCR_X(ux) (((ux) - m_posx) * m_scale)
#define SCR_Y(uy) (((uy) - m_posy) * m_scale)
#define SCR_P(ux, uy) SCR_X(ux), SCR_Y(uy)
#define SCR_L(ul) ((ul) * m_scale)
#define SCR_S(uw, uh) SCR_L(uw), SCR_L(uh)
#define UNI_X(px) ((px) / m_scale + m_posx)
#define UNI_Y(py) ((py) / m_scale + m_posy)
#define UNI_P(px, py) UNI_X(px), UNI_Y(py)
#define UNI_L(pl) ((pl) / m_scale)
#define UNI_S(pw, ph) UNI_L(pw), UNI_L(ph)

void EditorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(m_pixmap);
    painter.setPen(QColor(240, 240, 208));
    painter.setBrush(QBrush(QColor(240, 240, 208)));
    painter.drawRect(0, 0, width(), height());
    painter.setPen(QColor(192, 192, 192));
    float tmpx = floor(m_posx) * m_scale - m_posx * m_scale;
    float tmpy = floor(m_posy) * m_scale - m_posy * m_scale;
    for(int ix = 1; ix * m_scale + tmpx < width(); ix ++)
        for(int iy = 1; iy * m_scale + tmpy < width(); iy ++)
            painter.drawRect(ix * m_scale + tmpx - 1, iy * m_scale + tmpy - 1, 1, 1);

    // Paint Elements
    painter.save();
    painter.scale(SCR_S(1, 1));
    painter.translate(-m_posx, -m_posy);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(192, 192, 192)));
    painter.drawRect(0, 0, 1, 1);
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
    {
        painter.save();
        painter.translate((*iter)->position().x(), (*iter)->position().y());
        (*iter)->paint(&painter);
        painter.restore();
    }

    // Paint Wire
    painter.setPen(QPen(QColor(32, 32, 32), 0.125));
    for (auto iter = m_wires.begin(); iter != m_wires.end(); iter ++)
    {
        for (int i = 0, s = (*iter)->path().count() - 1; i < s; i ++)
        {
            if (m_selectedWire == (*iter))
                painter.setPen(QPen(QColor(64, 128, 255), 0.125));
            painter.drawLine((*iter)->path()[i], (*iter)->path()[i + 1]);
            if (m_selectedWire == (*iter))
                painter.setPen(QPen(QColor(32, 32, 32), 0.125));
        }
    }
    painter.restore();

    // Paint Selection Rect
    painter.setPen(QPen(QColor(32, 32, 32), 1, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    if (m_selectedElement)
    {
        QRect rect = m_selectedElement->rect();
        painter.drawRect(SCR_X(rect.x()) - 1, SCR_Y(rect.y()) - 1, SCR_L(rect.width()) + 1, SCR_L(rect.height()) + 1);
    }

    // Paint Placing Wire
    if (m_placingWire)
    {
        int uniX = round(UNI_X(m_prevMousePos.x())), uniY = round(UNI_Y(m_prevMousePos.y()));
        painter.setPen(QPen(QColor(32, 32, 32), 1, Qt::DashLine));
        painter.drawLine(0, SCR_Y(uniY), width(), SCR_Y(uniY));
        painter.drawLine(SCR_X(uniX), 0, SCR_X(uniX), height());
        if (m_selectedWire->element(0))
        {
            painter.setPen(QPen(QColor(64, 128, 255), 2, Qt::DashLine));
            QPoint last = m_selectedWire->path().constLast();
            painter.drawLine(SCR_X(last.x()), SCR_Y(last.y()), SCR_X(uniX), SCR_Y(uniY));
        }
        painter.setPen(QPen(QColor(32, 32, 32), 2));
        painter.drawLine(SCR_X(uniX - 0.5f), SCR_Y(uniY), SCR_X(uniX + 0.5f), SCR_Y(uniY));
        painter.drawLine(SCR_X(uniX), SCR_Y(uniY - 0.5f), SCR_X(uniX), SCR_Y(uniY + 0.5f));
    }

    painter.end();
    QPainter(this).drawPixmap(0, 0, *m_pixmap);
}

void EditorWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    delete m_pixmap;
    m_pixmap = new QPixmap(width(), height());
}

void EditorWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_R:
        if (event->modifiers() & Qt::ShiftModifier) rotateElementCW();
        else rotateElementCCW();
        break;
    case Qt::Key_X: flipHorizontal(); break;
    case Qt::Key_Y: flipVertical(); break;
    case Qt::Key_W: startPlacingWire(); break;
    case Qt::Key_Escape:
        m_selectedElement = nullptr;
        if (m_placingWire)
        {
            delete m_selectedWire;
            m_placingWire = false;
            setMouseTracking(false);
        }
        m_selectedWire = nullptr;
        update();
        break;
    case Qt::Key_Backspace:
        if (m_selectedElement)
        {
            delete m_selectedElement;
            m_selectedElement = nullptr;
            update();
        }
        else if (m_selectedWire)
        {
            if (m_placingWire) break;
            delete m_selectedWire;
            m_selectedWire = nullptr;
            update();
        }
        break;
    default: event->ignore();
    }
}

void EditorWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        float uniX = UNI_X(event->pos().x()), uniY = UNI_Y(event->pos().y());
        Editor::Wire *tmpSelectedWire = nullptr;
        Editor::Element *tmpSelectedElement = nullptr;

        if (m_placingWire)
        {
            m_placeWireWhenRelease = true;
            m_isDragingScreen = true;
            goto finishEvent;
        }

        for (auto iter = m_wires.begin(); iter != m_wires.end(); iter ++)
            if ((*iter)->nearWire(uniX, uniY)) tmpSelectedWire = *iter;
        if (tmpSelectedWire)
        {
            m_selectedWire = tmpSelectedWire;
            m_selectedElement = nullptr;
            m_isDragingScreen = true;
            goto finishEvent;
        }

        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
            if ((*iter)->rect().contains(uniX, uniY)) tmpSelectedElement = *iter;
        if (tmpSelectedElement)
        {
            m_selectedElement = tmpSelectedElement;
            m_selectedWire = nullptr;
            m_isDragingElement = true;
            m_oriElementPos = m_selectedElement->position();
            goto finishEvent;
        }

        m_isDragingScreen = true;
        m_unselectWhenRelease = true;

        finishEvent:
        m_prevMousePos = event->pos();
        update();
    }
}

void EditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragingElement)
    {
        int dx = round(UNI_L(event->pos().x() - m_prevMousePos.x()));
        int dy = round(UNI_L(event->pos().y() - m_prevMousePos.y()));
        m_selectedElement->setPosition(m_oriElementPos + QPoint(dx, dy));
        for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
             iter != end; iter ++) (*iter)->updatePath();
        update();
    }
    else if (m_isDragingScreen)
    {
        m_posx += UNI_L(m_prevMousePos.x() - event->pos().x());
        m_posy += UNI_L(m_prevMousePos.y() - event->pos().y());
        m_prevMousePos = event->pos();
        m_unselectWhenRelease = false;
        m_placeWireWhenRelease = false;
        update();
    }
    else if (m_placingWire)
    {
        m_prevMousePos = event->pos();
        update();
    }
}

void EditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_isDragingElement = false;
    m_isDragingScreen = false;
    if (m_unselectWhenRelease)
    {
        m_selectedElement = nullptr;
        m_selectedWire = nullptr;
        m_unselectWhenRelease = false;
        update();
    }
    if (m_placeWireWhenRelease)
    {
        QPoint uniPos = QPoint(round(UNI_X(m_prevMousePos.x())),round(UNI_Y(m_prevMousePos.y())));
        if (m_selectedWire->element(0))
        {
            for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
                for (int i = 0, s = (*iter)->pinCount(); i < s; i ++)
                    if (uniPos == (*iter)->pinPos(i))
                    {
                        m_selectedWire->stopRecording(*iter, i);
                        m_placingWire = false;
                        setMouseTracking(false);
                        goto finished;
                    }
            m_selectedWire->recordNode(uniPos);
        }
        else
        {
            for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
                for (int i = 0, s = (*iter)->pinCount(); i < s; i ++)
                    if (uniPos == (*iter)->pinPos(i))
                    {
                        m_selectedWire->startRecording(*iter, i);
                        goto finished;
                    }
        }
        finished: m_placeWireWhenRelease = false;
        update();
    }
}

void EditorWidget::wheelEvent(QWheelEvent *event)
{
    float oriScale = m_scale;
    m_scale *= pow(2, event->angleDelta().y() / 480.0f);
    if (m_scale < 8) m_scale = 8;
    if (m_scale > 32) m_scale = 32;
    m_posx += event->position().x() / oriScale - event->position().x() / m_scale;
    m_posy += event->position().y() / oriScale - event->position().y() / m_scale;
    update();
}

void EditorWidget::createContextMenu(QMenu *menu, QPoint pos)
{
    if (m_selectedElement)
    {
        menu->addAction(tr("Rotate 90°CCW"), this, &EditorWidget::rotateElementCCW);
        menu->addAction(tr("Rotate 90°CW"), this, &EditorWidget::rotateElementCW);
        menu->addAction(tr("Flip Horizontal"), this, &EditorWidget::flipHorizontal);
        menu->addAction(tr("Flip Vertical"), this, &EditorWidget::flipVertical);
        menu->addSeparator();
    }
    menu->addAction(tr("Place Wire"), this, &EditorWidget::startPlacingWire);
    QMenu *elementMenu = new QMenu(tr("Place Element"), menu);
    elementMenu->addAction(tr("Resistance"), this, [this, pos]() {
        new Editor::Resistance(this, QPoint(round(UNI_X(pos.x())), round(UNI_Y(pos.y()))));
    });
    menu->addMenu(elementMenu);
}

void EditorWidget::rotateElementCCW()
{
    if (m_selectedElement)
    {
        Editor::Element::ElementRotation rot = m_selectedElement->rotation();
        rot = (Editor::Element::ElementRotation) (((rot + 1) & Editor::Element::DirMask) | (rot & Editor::Element::FlipMask));
        m_selectedElement->setRotation(rot);
        for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
             iter != end; iter ++) (*iter)->updatePath();
        update();
    }
}

void EditorWidget::rotateElementCW()
{
    if (m_selectedElement)
    {
        Editor::Element::ElementRotation rot = m_selectedElement->rotation();
        rot = (Editor::Element::ElementRotation) (((rot - 1) & Editor::Element::DirMask) | (rot & Editor::Element::FlipMask));
        m_selectedElement->setRotation(rot);
        for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
             iter != end; iter ++) (*iter)->updatePath();
        update();
    }
}

void EditorWidget::flipHorizontal()
{
    if (m_selectedElement)
    {
        m_selectedElement->setRotation((Editor::Element::ElementRotation) (m_selectedElement->rotation() ^ Editor::Element::HorFliped));
        for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
             iter != end; iter ++) (*iter)->updatePath();
        update();
    }
}

void EditorWidget::flipVertical()
{
    if (m_selectedElement)
    {
        m_selectedElement->setRotation((Editor::Element::ElementRotation) (m_selectedElement->rotation() ^ Editor::Element::VerFliped));
        for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
             iter != end; iter ++) (*iter)->updatePath();
        update();
    }
}

void EditorWidget::startPlacingWire()
{
    if (!m_placingWire)
    {
        m_selectedElement = nullptr;
        m_placingWire = true;
        m_selectedWire = new Editor::Wire(this);
        setMouseTracking(true);
    }
}
