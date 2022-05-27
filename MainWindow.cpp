#include "MainWindow.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QContextMenuEvent>

#include "Graphic/EditorWidget.h"

#include "Simulation/Circuit.h"

#include "Graphic/OscilloscopeWidget.h"
#include "Graphic/ParametersInputWidget.h"

#include "Simulation/Elements/FunctionGenerator.h"
#include "Simulation/Elements/Capacity.h"
#include "Simulation/Elements/Inductor.h"
#include "Simulation/Elements/Resistor.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

MainWindow::OscDockWidget::OscDockWidget(MainWindow *parent)
    : QDockWidget(parent)
{
    m_oscilloscope = new OscilloscopeWidget(this);
    setWidget(m_oscilloscope);
    setFloating(true);
    resize(800, 600);
    hide();
}

OscilloscopeWidget *MainWindow::OscDockWidget::oscilloscope()
{
    return m_oscilloscope;
}

void MainWindow::OscDockWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ((MainWindow *) parent())->setInspector(m_oscilloscope->inspectorWidget());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_frameTime = 20;
    m_tickTime = 1e-4;
    m_playbackSpeed = 1;
    m_timer = new QBasicTimer();
    m_circuit = nullptr;

    m_editor = new EditorWidget(this);
    setCentralWidget(m_editor);
    m_oscilloscopeDock = new OscDockWidget(this);
    m_inspectorDock = new QDockWidget(this);
    m_inspectorDock->setWidget(m_oscilloscopeDock->oscilloscope()->inspectorWidget());
    m_inspectorDock->setFloating(true);

//    startSimulation();
}

MainWindow::~MainWindow()
{
    stopSimulation();
    delete m_inspectorDock;
    delete m_oscilloscopeDock;
    delete m_timer;
    // TODO release other resources;
}

void MainWindow::setInspector(QWidget *inspector)
{
    m_inspectorDock->setWidget(inspector);
}

bool MainWindow::isSimulating()
{
    return m_timer->isActive();
}

QByteArray MainWindow::serialize()
{
    QJsonObject json;
    QJsonObject simJson;
    simJson.insert("tick time", m_tickTime);
    simJson.insert("playback speed", m_playbackSpeed);
    json.insert("simulation", simJson);
    json.insert("circuit", m_editor->toJson());
    json.insert("oscilloscope", m_oscilloscopeDock->oscilloscope()->toJson());
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

void MainWindow::deserialize(const QByteArray &bytes)
{
    QJsonObject json = QJsonDocument::fromJson(bytes).object();
    QJsonObject simJson = json["simulation"].toObject();
    m_tickTime = simJson["tick time"].toDouble();
    m_playbackSpeed = simJson["playback speed"].toDouble();
    m_editor->fromJson(json["circuit"].toObject());
    m_oscilloscopeDock->oscilloscope()->fromJson(json["oscilloscope"].toObject());
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    if (m_inspectorDock->isHidden()) menu.addAction(tr("Show Inspector"), m_inspectorDock, &QDockWidget::show);
    else menu.addAction(tr("Hide Inspector"), m_inspectorDock, &QDockWidget::hide);
    if (m_oscilloscopeDock->isHidden()) menu.addAction(tr("Show Oscilloscope"), m_oscilloscopeDock, &QDockWidget::show);
    else menu.addAction(tr("Hide Oscilloscope"), m_oscilloscopeDock, &QDockWidget::hide);
    menu.addSeparator();
    m_editor->createContextMenu(&menu, event->pos());
    menu.addSeparator();
    menu.addAction(tr("serialize"), this, [this]() { qDebug() << serialize(); });
    menu.addAction(tr("deserialize"), this, [this]() {
        deserialize("{\"circuit\":{\"current probes\":[{\"element\":2,\"label\":\"IM\",\"pin\":1},{\"element\":3,\"label\":\"IB\",\"pin\":1}],\"elements\":[{\"position\":[15,17],\"rotation\":3,\"type\":\"Source/Ground\"},{\"label\":\"VCC\",\"position\":[37,17],\"rotation\":3,\"type\":\"Source/VCC\",\"voltage\":10},{\"position\":[19,17],\"resistance\":20,\"rotation\":0,\"type\":\"Resistor\"},{\"position\":[29,19],\"resistance\":10,\"rotation\":0,\"type\":\"Resistor\"},{\"position\":[29,15],\"resistance\":10,\"rotation\":0,\"type\":\"Resistor\"}],\"voltage probes\":[{\"label\":\"VS\",\"position\":[33,15]},{\"label\":\"VR\",\"position\":[25,19]}],\"wires\":[{\"begin\":{\"element\":0,\"pin\":0},\"end\":{\"element\":2,\"pin\":0},\"path\":[[15,17],[17,17]]},{\"begin\":{\"element\":2,\"pin\":1},\"end\":{\"element\":4,\"pin\":0},\"path\":[[21,17],[23,17],[25,15],[27,15]]},{\"begin\":{\"element\":2,\"pin\":1},\"end\":{\"element\":3,\"pin\":0},\"path\":[[21,17],[23,17],[25,19],[27,19]]},{\"begin\":{\"element\":4,\"pin\":1},\"end\":{\"element\":1,\"pin\":0},\"path\":[[31,15],[33,15],[35,17],[37,17]]},{\"begin\":{\"element\":3,\"pin\":1},\"end\":{\"element\":1,\"pin\":0},\"path\":[[31,19],[33,19],[35,17],[37,17]]}]},\"oscilloscope\":{\"div size\":[100,100],\"signals\":[{\"color\":[255,128,64],\"v/div\":5,\"value\":\"VR\"},{\"color\":[64,255,128],\"v/div\":1,\"value\":\"IM\"}],\"t/div\":1},\"simulation\":{\"playback speed\":1,\"tick time\":0.0001}}");
    });
    menu.exec(event->globalPos());
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    qint64 nextRealTime = QDateTime::currentMSecsSinceEpoch() + m_frameTime;
    double nextTime = m_time + m_frameTime * m_playbackSpeed / 1000;
    for (; m_time < nextTime && QDateTime::currentMSecsSinceEpoch() < nextRealTime; m_time += m_tickTime)
    {
        m_circuit->tick(m_time, m_tickTime);
        QMap<QString, double> para;
        para.insert("t", m_time);
        for (auto iter = m_currentProbes.begin(); iter != m_currentProbes.end(); iter ++)
            para.insert(iter->label, m_circuit->current(iter->pin));
        for (auto iter = m_voltageProbes.begin(); iter != m_voltageProbes.end(); iter ++)
            para.insert(iter->label, m_circuit->voltage(iter->net));
        m_oscilloscopeDock->oscilloscope()->record(m_time, para);
    }
#ifdef QT_DEBUG
    if (m_time < nextTime) qDebug() << "Simulation Timeout! Please reduce the Playback Speed or the simulation speed will be unstable.";
#endif
    m_oscilloscopeDock->oscilloscope()->update();
}

void MainWindow::startSimulation()
{
    m_time = 0;
    m_realTime = 0;
    m_circuit->solve();
    m_timer->start(m_frameTime, this);
    m_oscilloscopeDock->show();
}

void MainWindow::stopSimulation()
{
    m_timer->stop();
}
