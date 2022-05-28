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
    m_maxIterations = 64;
    m_maxAcceptableError = 1e-12;
    m_iterateLevel = 2;
    m_timer = new QBasicTimer();
    m_circuit = nullptr;

    m_editor = new EditorWidget(this);
    setCentralWidget(m_editor);
    m_oscilloscopeDock = new OscDockWidget(this);
    m_inspectorDock = new QDockWidget(this);
    m_inspectorDock->setWidget(m_editor->inspector());
    addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);
//    m_inspectorDock->setFloating(true);

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
    simJson.insert("max iterations", m_maxIterations);
    simJson.insert("max acceptable error", m_maxAcceptableError);
    simJson.insert("iterate level", m_iterateLevel);
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
    m_maxIterations = simJson["max iterations"].toInt();
    m_maxAcceptableError = simJson["max acceptable error"].toDouble();
    m_iterateLevel = simJson["iterate level"].toInt();
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
    menu.addAction(tr("Serialize"), this, [this]() { qDebug() << serialize(); });
    menu.addAction(tr("Deserialize"), this, [this]() {
        deserialize("{\"circuit\":{\"current probes\":[],\"elements\":[{\"function\":\"5*sin(3.1415926*t)\",\"position\":[36,29],\"rotation\":1,\"type\":\"Source/Function Generator\"},{\"position\":[32,17],\"resistance\":1000,\"rotation\":0,\"type\":\"Resistor\"},{\"position\":[36,24],\"resistance\":1,\"rotation\":1,\"type\":\"Resistor\"},{\"position\":[36,33],\"rotation\":0,\"type\":\"Source/Ground\"},{\"icbo\":1e-12,\"position\":[40,21],\"rotation\":0,\"type\":\"Transistor/Diode\"},{\"icbo\":1e-12,\"position\":[32,32],\"rotation\":0,\"type\":\"Transistor/Diode\"},{\"icbo\":1e-12,\"position\":[40,32],\"rotation\":0,\"type\":\"Transistor/Diode\"},{\"icbo\":1e-12,\"position\":[32,21],\"rotation\":0,\"type\":\"Transistor/Diode\"},{\"capacitance\":0.009999999776482582,\"initial voltage\":0,\"position\":[32,13],\"rotation\":0,\"type\":\"Capacity\"},{\"position\":[40,15],\"resistance\":10,\"rotation\":0,\"type\":\"Resistor\"}],\"voltage probes\":[{\"label\":\"vp\",\"position\":[36,15]},{\"label\":\"vn\",\"position\":[29,15]},{\"label\":\"vs\",\"position\":[36,21]}],\"wires\":[{\"begin\":{\"element\":0,\"pin\":1},\"end\":{\"element\":2,\"pin\":0},\"path\":[[36,27],[36,26]]},{\"begin\":{\"element\":3,\"pin\":0},\"end\":{\"element\":0,\"pin\":0},\"path\":[[36,33],[36,31]]},{\"begin\":{\"element\":8,\"pin\":1},\"end\":{\"element\":1,\"pin\":1},\"path\":[[34,13],[35,13],[35,17],[34,17]]},{\"begin\":{\"element\":9,\"pin\":0},\"end\":{\"element\":1,\"pin\":1},\"path\":[[38,15],[35,15],[35,17],[34,17]]},{\"begin\":{\"element\":8,\"pin\":0},\"end\":{\"element\":1,\"pin\":0},\"path\":[[30,13],[29,13],[29,17],[30,17]]},{\"begin\":{\"element\":7,\"pin\":1},\"end\":{\"element\":2,\"pin\":1},\"path\":[[34,21],[36,21],[36,22]]},{\"begin\":{\"element\":2,\"pin\":1},\"end\":{\"element\":4,\"pin\":0},\"path\":[[36,22],[36,21],[38,21]]},{\"begin\":{\"element\":0,\"pin\":0},\"end\":{\"element\":5,\"pin\":1},\"path\":[[36,31],[36,32],[34,32]]},{\"begin\":{\"element\":6,\"pin\":0},\"end\":{\"element\":0,\"pin\":0},\"path\":[[38,32],[36,32],[36,31]]},{\"begin\":{\"element\":9,\"pin\":1},\"end\":{\"element\":4,\"pin\":1},\"path\":[[42,15],[43,15],[43,21],[42,21]]},{\"begin\":{\"element\":9,\"pin\":1},\"end\":{\"element\":6,\"pin\":1},\"path\":[[42,15],[43,15],[43,32],[42,32]]},{\"begin\":{\"element\":1,\"pin\":0},\"end\":{\"element\":7,\"pin\":0},\"path\":[[30,17],[29,17],[29,21],[30,21]]},{\"begin\":{\"element\":1,\"pin\":0},\"end\":{\"element\":5,\"pin\":0},\"path\":[[30,17],[29,17],[29,32],[30,32]]}]},\"oscilloscope\":{\"div size\":[100,100],\"signals\":[{\"color\":[255,128,64],\"v/div\":2.5,\"value\":\"vs\"},{\"color\":[64,255,128],\"v/div\":2.5,\"value\":\"vp-vn\"}],\"t/div\":1},\"simulation\":{\"iterate level\":1,\"max acceptable error\":9.999999960041972e-13,\"max iterations\":64,\"playback speed\":1,\"tick time\":0.0010000000474974513}}");
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
