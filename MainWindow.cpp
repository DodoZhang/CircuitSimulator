#include "MainWindow.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenuBar>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMessageBox>

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
    : QDockWidget(tr("Oscilloscope"), parent)
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
    m_hasFilePath = false;

    QMenuBar *mb = menuBar();
    QMenu *fileMenu = new QMenu(tr("File"), mb);
    fileMenu->addAction(tr("New"), this, &MainWindow::newFile, QKeySequence::New);
    fileMenu->addAction(tr("Open"), this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addAction(tr("Save"), this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction(tr("Save As"), this, &MainWindow::saveAsFile, QKeySequence::SaveAs);
    mb->addMenu(fileMenu);

    m_editor = new EditorWidget(this);
    setCentralWidget(m_editor);
    m_oscilloscopeDock = new OscDockWidget(this);
    m_inspectorDock = new QDockWidget(tr("Inspector"), this);
    m_inspectorDock->setWidget(m_editor->inspector());
    addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);

    QMenu *windowMenu = new QMenu(tr("Window"), mb);
    windowMenu->addAction(tr("Inspector"), this, [this]() {
        if (m_inspectorDock->isHidden()) m_inspectorDock->show();
        else m_inspectorDock->hide();
    });
    windowMenu->addAction(tr("Oscilloscope"), this, [this]() {
        if (m_oscilloscopeDock->isHidden()) m_oscilloscopeDock->show();
        else m_oscilloscopeDock->hide();
    });
    mb->addMenu(windowMenu);

    setWindowTitle(tr("Untitled") + tr(" - Circuit Simulator"));
}

MainWindow::~MainWindow()
{
    stopSimulation();
    delete m_timer;
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
    if (!m_editor->createContextMenu(&menu))
    {
        event->ignore();
        return;
    }
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

void MainWindow::newFile()
{
    if (isSimulating())
    {
        stopSimulation();
        delete m_circuit;
        m_circuit = nullptr;
        m_currentProbes.clear();
        m_voltageProbes.clear();
    }
    m_hasFilePath = false;
    deserialize("{\"circuit\":{\"current probes\":[],\"elements\":[],\"voltage probes\":[],\"wires\":[]},"
                "\"oscilloscope\":{\"div size\":[100,100],\"signals\":[{\"color\":[255,128,64],\"v/div\":1,\"value\":\"0\"}],"
                "\"t/div\":1},\"simulation\":{\"iterate level\":2,\"max acceptable error\":1e-12,\"max iterations\":64,"
                "\"playback speed\":1,\"tick time\":0.0001}}");
    setWindowTitle(tr("Untitled") + tr(" - Circuit Simulator"));
}

void MainWindow::saveFile()
{
    QFile file;
    if (m_hasFilePath) file.setFileName(m_filePath);
    if (!m_hasFilePath || !file.exists())
    {
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), m_hasFilePath ? m_filePath : tr("Untitled") + ".cir", tr("Circuit File (*.cir)"));
        if (path.isNull()) return;
        int tmp = path.lastIndexOf('/');
        QDir::setCurrent(path.left(tmp));
        QString fileName = path.right(path.length() - tmp - 1);
        file.setFileName(fileName);
        m_filePath = path;
        m_hasFilePath = true;
        setWindowTitle(fileName.left(fileName.lastIndexOf('.')) + tr(" - Circuit Simulator"));
    }
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(serialize());
}

void MainWindow::saveAsFile()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File As"), m_hasFilePath ? m_filePath : "Untitled.cir", tr("Circuit File (*.cir)"));
    if (path.isNull()) return;
    int tmp = path.lastIndexOf('/');
    QDir::setCurrent(path.left(tmp));
    QString fileName = path.right(path.length() - tmp - 1);
    QFile file(fileName);
    m_filePath = path;
    m_hasFilePath = true;
    file.open(QFile::WriteOnly);
    file.write(serialize());
    setWindowTitle(fileName.left(fileName.lastIndexOf('.')) + tr(" - Circuit Simulator"));
}

void MainWindow::openFile()
{
    if (isSimulating())
    {
        stopSimulation();
        delete m_circuit;
        m_circuit = nullptr;
        m_currentProbes.clear();
        m_voltageProbes.clear();
    }
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), m_hasFilePath ? m_filePath : "../", tr("Circuit File (*.cir)"));
    if (path.isNull()) return;
    int tmp = path.lastIndexOf('/');
    QDir::setCurrent(path.left(tmp));
    QString fileName = path.right(path.length() - tmp - 1);
    QFile file(fileName);
    if (!file.exists()) return;
    m_filePath = path;
    m_hasFilePath = true;
    file.open(QFile::ReadOnly);
    deserialize(file.readAll());
    setWindowTitle(fileName.left(fileName.lastIndexOf('.')) + tr(" - Circuit Simulator"));
}
