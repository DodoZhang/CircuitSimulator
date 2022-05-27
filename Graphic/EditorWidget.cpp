#include "EditorWidget.h"

#include <cmath>
#include <QPainter>
#include <QMenu>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "MainWindow.h"
#include "ParametersInputWidget.h"
#include "Simulation/Circuit.h"
#include "Simulation/Pin.h"
#include "Simulation/Net.h"
#include "EditorElement.h"
#include "EditorWire.h"
#include "Elements/EditorGround.h"
#include "Elements/EditorVCC.h"
#include "Elements/EditorResistor.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

int EditorWidget::CurrentProbe::count = 1;

EditorWidget::CurrentProbe::CurrentProbe(EditorWidget *editor, Editor::Element *element, int pin)
{
    this->element = element;
    this->pin = pin;
    label = "i" + QString::number(count ++);
    inspector = new ParametersInputWidget();
    inspector->addParameter(&label, tr("Label"), PIWItemType(QString));
    QObject::connect(inspector, &ParametersInputWidget::parameterChanged, editor, &EditorWidget::updateSlot);
}

EditorWidget::CurrentProbe::~CurrentProbe()
{
    delete inspector;
}

void EditorWidget::CurrentProbe::paint(QPainter *painter)
{
    painter->drawLine(QLineF(-0.25, -0.25, 0.25, 0.25));
    painter->drawLine(QLineF(-0.25, 0.25, 0.25, -0.25));
    painter->drawLine(QLineF(0, 0, 1, 0));
    painter->scale(0.0625, 0.0625);
    painter->drawText(QRectF(20, -8, 32, 16), Qt::AlignLeft, label);
}

int EditorWidget::VoltageProbe::count = 1;

EditorWidget::VoltageProbe::VoltageProbe(EditorWidget *editor, QPoint pos)
{
    this->pos = pos;
    label = "v" + QString::number(count ++);
    inspector = new ParametersInputWidget();
    inspector->addParameter(&label, tr("Label"), PIWItemType(QString));
    QObject::connect(inspector, &ParametersInputWidget::parameterChanged, editor, &EditorWidget::updateSlot);
}

EditorWidget::VoltageProbe::~VoltageProbe()
{
    delete inspector;
}

void EditorWidget::VoltageProbe::paint(QPainter *painter)
{
    painter->drawEllipse(QRectF(-0.25, -0.25, 0.5, 0.5));
    painter->drawLine(QLineF(0.177, -0.177, 1, -1));
    painter->drawLine(QLineF(1, -1, 1.5, -1));
    painter->scale(0.0625, 0.0625);
    painter->drawText(QRectF(28, -24, 32, 16), Qt::AlignLeft, label);
}

EditorWidget::EditorWidget(MainWindow *parent) : QWidget(parent)
{
    m_posx = m_posy = 0;
    m_scale = 16;
    m_pixmap = new QPixmap(width(), height());
    m_isDragingElement = false;
    m_isDragingScreen = false;
    m_selectedElement = nullptr;
    m_selectedWire = nullptr;
    m_selectedCurProbe = nullptr;
    m_selectedVolProbe = nullptr;
    m_unselectWhenRelease = false;
    m_placingWire = false;
    m_placeWireWhenRelease = false;
    m_placingCurProbe = false;

    setFocusPolicy(Qt::ClickFocus);

    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&parent->m_tickTime, tr("Tick Time"), PIWItemType(double));
    m_inspector->addParameter(&parent->m_playbackSpeed, tr("Playback Speed"), PIWItemType(double));
}

EditorWidget::~EditorWidget()
{
    delete m_pixmap;
    delete m_inspector;
    for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end(); iter ++) delete *iter;
    for (auto iter = m_voltageProbes.begin(); iter != m_voltageProbes.end(); iter ++) delete *iter;
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

    // Unit Reference System
    painter.save();
    painter.scale(SCR_S(1, 1));
    painter.translate(-m_posx, -m_posy);

    // Paint Elements
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(192, 192, 192)));
    painter.drawRect(0, 0, 1, 1);
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
    {
        painter.save();
        painter.translate((*iter)->position());
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

    // Paint Probes
    painter.setPen(QPen(QColor(96, 96, 96), 0.125));
    painter.setBrush(Qt::NoBrush);
    for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end(); iter ++)
    {
        painter.save();
        painter.translate((*iter)->element->pinPos((*iter)->pin));
        if (m_selectedCurProbe == *iter) painter.setPen(QPen(QColor(64, 128, 255), 0.125));
        (*iter)->paint(&painter);
        painter.restore();
    }
    for (auto iter = m_voltageProbes.begin(); iter != m_voltageProbes.end(); iter ++)
    {
        painter.save();
        painter.translate((*iter)->pos);
        if (m_selectedVolProbe == *iter) painter.setPen(QPen(QColor(64, 128, 255), 0.125));
        (*iter)->paint(&painter);
        painter.restore();
    }

    // Screen Reference System
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

    // Paint Placing Current Probe
    if (m_placingCurProbe)
    {
        int uniX = round(UNI_X(m_prevMousePos.x())), uniY = round(UNI_Y(m_prevMousePos.y()));
        painter.setPen(QPen(QColor(64, 128, 255), 2));
        painter.drawLine(SCR_X(uniX - 0.25f), SCR_Y(uniY - 0.25f), SCR_X(uniX + 0.25f), SCR_Y(uniY + 0.25f));
        painter.drawLine(SCR_X(uniX - 0.25f), SCR_Y(uniY + 0.25f), SCR_X(uniX + 0.25f), SCR_Y(uniY - 0.25f));
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
        if (m_placingWire)
        {
            delete m_selectedWire;
            m_placingWire = false;
            setMouseTracking(false);
        }
        if (m_placingCurProbe)
        {
            m_placingCurProbe = false;
            setMouseTracking(false);
        }
        m_selectedElement = nullptr;
        m_selectedWire = nullptr;
        m_selectedCurProbe = nullptr;
        m_selectedVolProbe = nullptr;
        update();
        break;
    case Qt::Key_Backspace:
        if (m_selectedElement)
        {
            for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end();)
            {
                if ((*iter)->element == m_selectedElement)
                {
                    delete *iter;
                    iter = m_currentProbes.erase(iter);
                } else iter ++;
            }
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
        else if (m_selectedCurProbe)
        {
            m_currentProbes.removeOne(m_selectedCurProbe);
            delete m_selectedCurProbe;
            m_selectedCurProbe = nullptr;
            update();
        }
        else if (m_selectedVolProbe)
        {
            m_voltageProbes.removeOne(m_selectedVolProbe);
            delete m_selectedVolProbe;
            m_selectedVolProbe = nullptr;
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
        CurrentProbe *tmpSelectedCurProbe = nullptr;
        VoltageProbe *tmpSelectedVolProbe = nullptr;

        if (m_placingWire)
        {
            m_placeWireWhenRelease = true;
            m_isDragingScreen = true;
            goto finishEvent;
        }

        if (m_placingCurProbe)
        {
            m_placeCurProbeWhenRelease = true;
            m_isDragingScreen = true;
            goto finishEvent;
        }

        for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end(); iter ++)
        {
            QPoint pos = (*iter)->element->pinPos((*iter)->pin);
            float sqdis = (pos.x() - uniX) * (pos.x() - uniX) + (pos.y() - uniY) * (pos.y() - uniY);
            if (sqdis <= 1) tmpSelectedCurProbe = *iter;
        }
        if (tmpSelectedCurProbe)
        {
            m_selectedWire = nullptr;
            m_selectedElement = nullptr;
            m_selectedCurProbe = tmpSelectedCurProbe;
            m_selectedVolProbe = nullptr;
            m_isDragingScreen = true;
            ((MainWindow *) parent())->setInspector(m_selectedCurProbe->inspector);
            goto finishEvent;
        }

        for (auto iter = m_voltageProbes.begin(); iter != m_voltageProbes.end(); iter ++)
        {
            float sqdis = ((*iter)->pos.x() - uniX) * ((*iter)->pos.x() - uniX) + ((*iter)->pos.y() - uniY) * ((*iter)->pos.y() - uniY);
            if (sqdis <= 1) tmpSelectedVolProbe = *iter;
        }
        if (tmpSelectedVolProbe)
        {
            m_selectedWire = nullptr;
            m_selectedElement = nullptr;
            m_selectedCurProbe = nullptr;
            m_selectedVolProbe = tmpSelectedVolProbe;
            m_isDragingElement = true;
            m_oriElementPos = m_selectedVolProbe->pos;
            ((MainWindow *) parent())->setInspector(m_selectedVolProbe->inspector);
            goto finishEvent;
        }

        for (auto iter = m_wires.begin(); iter != m_wires.end(); iter ++)
            if ((*iter)->nearWire(uniX, uniY)) tmpSelectedWire = *iter;
        if (tmpSelectedWire)
        {
            m_selectedWire = tmpSelectedWire;
            m_selectedElement = nullptr;
            m_selectedCurProbe = nullptr;
            m_selectedVolProbe = nullptr;
            m_isDragingScreen = true;
            goto finishEvent;
        }

        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
            if ((*iter)->rect().contains(uniX, uniY)) tmpSelectedElement = *iter;
        if (tmpSelectedElement)
        {
            m_selectedWire = nullptr;
            m_selectedElement = tmpSelectedElement;
            m_selectedCurProbe = nullptr;
            m_selectedVolProbe = nullptr;
            m_isDragingElement = true;
            m_oriElementPos = m_selectedElement->position();
            ((MainWindow *) parent())->setInspector(m_selectedElement->inspectorWidget());
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
        if (m_selectedElement)
        {
            m_selectedElement->setPosition(m_oriElementPos + QPoint(dx, dy));
            for (auto iter = m_wireMap[m_selectedElement].begin(), end = m_wireMap[m_selectedElement].end();
                 iter != end; iter ++) (*iter)->updatePath();
        }
        else m_selectedVolProbe->pos = m_oriElementPos + QPoint(dx, dy);
        update();
    }
    else if (m_isDragingScreen)
    {
        m_posx += UNI_L(m_prevMousePos.x() - event->pos().x());
        m_posy += UNI_L(m_prevMousePos.y() - event->pos().y());
        m_prevMousePos = event->pos();
        m_unselectWhenRelease = false;
        m_placeWireWhenRelease = false;
        m_placeCurProbeWhenRelease = false;
        update();
    }
    else if (m_placingWire)
    {
        m_prevMousePos = event->pos();
        update();
    }
    else if (m_placingCurProbe)
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
        m_selectedCurProbe = nullptr;
        m_selectedVolProbe = nullptr;
        ((MainWindow *) parent())->setInspector(m_inspector);
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
                        goto finishPlacingWire;
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
                        goto finishPlacingWire;
                    }
        }
        finishPlacingWire:
        m_placeWireWhenRelease = false;
        update();
    }
    if (m_placeCurProbeWhenRelease)
    {
        QPoint uniPos = QPoint(round(UNI_X(m_prevMousePos.x())),round(UNI_Y(m_prevMousePos.y())));
        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
            for (int i = 0, s = (*iter)->pinCount(); i < s; i ++)
                if (uniPos == (*iter)->pinPos(i))
                {
                    m_selectedCurProbe = new CurrentProbe(this, *iter, i);
                    m_currentProbes.append(m_selectedCurProbe);
                    m_placingCurProbe = false;
                    setMouseTracking(false);
                    goto finishplacingCurProbe;
                }
        finishplacingCurProbe:
        m_placeCurProbeWhenRelease = false;
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
    if (((MainWindow *) parent())->isSimulating()) menu->addAction(tr("Stop Simulation"), this, &EditorWidget::stopSimultaion);
    else menu->addAction(tr("Start Simulation"), this, &EditorWidget::startSimultaion);
    menu->addSeparator();

    if (m_selectedElement)
    {
        menu->addAction(tr("Rotate 90°CCW"), this, &EditorWidget::rotateElementCCW);
        menu->addAction(tr("Rotate 90°CW"), this, &EditorWidget::rotateElementCW);
        menu->addAction(tr("Flip Horizontal"), this, &EditorWidget::flipHorizontal);
        menu->addAction(tr("Flip Vertical"), this, &EditorWidget::flipVertical);
        menu->addSeparator();
    }

    menu->addAction(tr("Place Wire"), this, &EditorWidget::startPlacingWire);
    QMenu *probeMenu = new QMenu(tr("Place Probe"), menu);
    probeMenu->addAction(tr("Voltage"), this, [this, pos]() { \
        m_voltageProbes.append(new VoltageProbe(this, QPoint(round(UNI_X(pos.x())), round(UNI_Y(pos.y()))))); \
    });
    probeMenu->addAction(tr("Current"), this, &EditorWidget::startPlacingCurProbe);
    menu->addMenu(probeMenu);
    QMenu *elementMenu = new QMenu(tr("Place Element"), menu);
#define REG_ELEMENT(element) elementMenu->addAction(tr(#element), this, [this, pos]() { \
                                 new Editor::element(this, QPoint(round(UNI_X(pos.x())), round(UNI_Y(pos.y())))); \
                             })
    REG_ELEMENT(Ground);
    REG_ELEMENT(VCC);
    REG_ELEMENT(Resistor);
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
    if (m_placingCurProbe) return;
    if (!m_placingWire)
    {
        m_placingWire = true;
        m_selectedElement = nullptr;
        m_selectedWire = new Editor::Wire(this);
        m_selectedCurProbe = nullptr;
        m_selectedVolProbe = nullptr;
        setMouseTracking(true);
    }
}

void EditorWidget::startPlacingCurProbe()
{
    if (m_placingWire) return;
    if (!m_placingCurProbe)
    {
        m_placingCurProbe = true;
        m_selectedElement = nullptr;
        m_selectedWire = nullptr;
        m_selectedCurProbe = nullptr;
        m_selectedVolProbe = nullptr;
        setMouseTracking(true);
    }
}

void EditorWidget::startSimultaion()
{
    CirSim::Circuit *cir = new CirSim::Circuit();
    QHash<Editor::Element *, QVector<CirSim::Pin *>> pinMap;
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
        pinMap.insert(*iter, (*iter)->createElement(cir));
    for (auto iter = m_wires.begin(); iter != m_wires.end(); iter ++)
        pinMap[(*iter)->element(0)][(*iter)->pin(0)]
                ->connect(pinMap[(*iter)->element(1)][(*iter)->pin(1)]);
    auto *mw = (MainWindow *) parent();
    mw->m_circuit = cir;
    for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end(); iter ++)
        mw->m_currentProbes.append(::CurrentProbe { pinMap[(*iter)->element][(*iter)->pin], (*iter)->label });
    for (auto iter = m_voltageProbes.begin(); iter != m_voltageProbes.end(); iter ++)
    {
        for (auto iterWire = m_wires.begin(); iterWire != m_wires.end(); iterWire ++)
        {
            if ((*iterWire)->onWire((*iter)->pos))
            {
                mw->m_voltageProbes.append(::VoltageProbe { pinMap[(*iterWire)->element(0)][(*iterWire)->pin(0)]->net(), (*iter)->label });
                goto foundNet;
            }
        }
#ifdef QT_DEBUG
    qDebug() << "Invalid Voltage Probe: " + (*iter)->label;
#endif
        foundNet: continue;
    }
#ifdef QT_DEBUG
    qDebug("%s", mw->m_circuit->debug().toStdString().c_str());
#endif
    mw->startSimulation();
}

void EditorWidget::stopSimultaion()
{
    auto *mw = (MainWindow *) parent();
    mw->stopSimulation();
    delete mw->m_circuit;
    mw->m_circuit = nullptr;
    mw->m_currentProbes.clear();
    mw->m_voltageProbes.clear();
}

void EditorWidget::updateSlot()
{
    update();
}
