#include "MainWindow.h"

#include <QApplication>
#include <QTranslator>
#include <QDateTime>
#include <QStandardPaths>
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
    : QDockWidget(parent)
{
    m_oscilloscope = new OscilloscopeWidget(this);
    setWidget(m_oscilloscope);
    setFloating(true);
    resize(800, 600);
    setMinimumSize(400, 400);
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
    m_language = "en";

    loadSettings();

    QMenuBar *mb = menuBar();
    QMenu *fileMenu = new QMenu(tr("File"), mb);
    fileMenu->addAction(tr("New"), this, &MainWindow::newFile, QKeySequence::New);
    fileMenu->addAction(tr("Open"), this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addAction(tr("Save"), this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction(tr("Save As"), this, &MainWindow::saveAsFile, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    QMenu *exampleMenu = new QMenu(tr("Open Example"), fileMenu);
    exampleMenu->addAction(tr("Capacitor Inductor Characteristics"), this, [this]() {
        openExample(tr("Capacitor Inductor Characteristics"), "CapacitorInductorCharacteristics.cir");
    });
    exampleMenu->addAction(tr("Full Bridge Rectifier"), this, [this]() {
        openExample(tr("Full Bridge Rectifier"), "FullBridgeRectifier.cir");
    });
    exampleMenu->addAction(tr("MOSFET Amplifier Circuit"), this, [this]() {
        openExample(tr("MOSFET Amplifier Circuit"), "MOSFETAmplifierCircuit.cir");
    });
    fileMenu->addMenu(exampleMenu);
    mb->addMenu(fileMenu);

    m_editor = new EditorWidget(this);
    setCentralWidget(m_editor);
    m_oscilloscopeDock = new OscDockWidget(this);
    m_oscilloscopeDock->setWindowTitle(tr("Oscilloscope"));
    m_inspectorDock = new QDockWidget(tr("Inspector"), this);
    m_inspectorDock->setWidget(m_editor->inspector());
    m_inspectorDock->setMinimumSize(200, 200);
    addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);

    QMenu *windowMenu = new QMenu(tr("Window"), mb);
    m_inspectorAction = windowMenu->addAction(tr("Inspector"), this, [this]() {
        if (m_inspectorDock->isHidden()) m_inspectorDock->show();
        else m_inspectorDock->hide();
        m_inspectorAction->setChecked(!m_inspectorDock->isHidden());
    });
    m_inspectorAction->setCheckable(true);
    m_inspectorAction->setChecked(!m_inspectorDock->isHidden());
    m_oscilloscopeAction = windowMenu->addAction(tr("Oscilloscope"), this, [this]() {
        if (m_oscilloscopeDock->isHidden()) m_oscilloscopeDock->show();
        else m_oscilloscopeDock->hide();
        m_oscilloscopeAction->setChecked(!m_oscilloscopeDock->isHidden());
    });
    m_oscilloscopeAction->setCheckable(true);
    m_oscilloscopeAction->setChecked(!m_oscilloscopeDock->isHidden());
    windowMenu->addSeparator();

    QMenu *languageMenu = new QMenu(tr("Language"), windowMenu);
    QAction *tmpAction;
    tmpAction = languageMenu->addAction(tr("English"), this, [this]() { setLanguage("en"); });
    tmpAction->setCheckable(true);
    if (m_language == "en") tmpAction->setChecked(true);
    m_languageActions.insert("en", tmpAction);
    tmpAction = languageMenu->addAction(tr("Chinese"), this, [this]() { setLanguage("zh_CN"); });
    tmpAction->setCheckable(true);
    if (m_language == "zh_CN") tmpAction->setChecked(true);
    m_languageActions.insert("zh_CN", tmpAction);
    windowMenu->addMenu(languageMenu);
    mb->addMenu(windowMenu);

    resize(1200, 800);
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
    m_inspectorDock->setWidget(m_editor->inspector());
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_inspectorAction);
    menu.addAction(m_oscilloscopeAction);
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
        QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/CircuitSimulator";
        QString path = QFileDialog::getSaveFileName(this, tr("Save File"), m_hasFilePath ? m_filePath : docPath + "/" + tr("Untitled") + ".cir", tr("Circuit File (*.cir)"));
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
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/CircuitSimulator";
    QString path = QFileDialog::getSaveFileName(this, tr("Save File As"), m_hasFilePath ? m_filePath : docPath + "/" + "Untitled.cir", tr("Circuit File (*.cir)"));
    if (path.isNull()) return;
    int tmp = path.lastIndexOf('/');
    QDir::setCurrent(path.left(tmp));
    QString fileName = path.right(path.length() - tmp - 1);
    QFile file(fileName);
    m_filePath = path;
    m_hasFilePath = true;
    file.open(QFile::WriteOnly | QIODevice::Text);
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
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/CircuitSimulator";
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), m_hasFilePath ? m_filePath : docPath, tr("Circuit File (*.cir)"));
    if (path.isNull()) return;
    int tmp = path.lastIndexOf('/');
    QDir::setCurrent(path.left(tmp));
    QString fileName = path.right(path.length() - tmp - 1);
    QFile file(fileName);
    if (!file.exists()) return;
    m_filePath = path;
    m_hasFilePath = true;
    file.open(QFile::ReadOnly | QIODevice::Text);
    deserialize(file.readAll());
    setWindowTitle(fileName.left(fileName.lastIndexOf('.')) + tr(" - Circuit Simulator"));
}

void MainWindow::openExample(const QString &name, const QString &path)
{
    if (isSimulating())
    {
        stopSimulation();
        delete m_circuit;
        m_circuit = nullptr;
        m_currentProbes.clear();
        m_voltageProbes.clear();
    }
    QFile file(":/Examples/" + path);
    m_hasFilePath = false;
    file.open(QFile::ReadOnly | QIODevice::Text);
    deserialize(file.readAll());
    setWindowTitle(name + tr(" - Circuit Simulator"));
}

void MainWindow::setLanguage(const QString &language)
{
    if (language == m_language) return;
    QMessageBox msgBox;
    msgBox.setText(tr("You need to restart the application to change the language."));
    msgBox.exec();
    m_languageActions[m_language]->setChecked(false);
    m_language = language;
    m_languageActions[m_language]->setChecked(true);
    saveSettings();
}

void MainWindow::loadSettings()
{
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/CircuitSimulator";
    QDir docDir(docPath);
    if (!docDir.exists()) docDir.mkdir(docPath);
    QDir::setCurrent(docPath);
    QFile settingsFile(".settings");
    if (settingsFile.exists())
    {
        settingsFile.open(QFile::ReadOnly | QIODevice::Text);
        QJsonObject obj = QJsonDocument::fromJson(settingsFile.readAll()).object();
        m_language = obj["language"].toString("en");
        if (m_language != "en")
        {
            m_translator = new QTranslator();
            m_translator->load(":/Translations/CiruitSimulator." + m_language + ".qm");
            QApplication::installTranslator(m_translator);
        }
    }
    else saveSettings();
}

void MainWindow::saveSettings()
{
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/CircuitSimulator";
    QDir docDir(docPath);
    if (!docDir.exists()) docDir.mkdir(docPath);
    QDir::setCurrent(docPath);
    QFile settingsFile(".settings");
    settingsFile.open(QFile::WriteOnly | QIODevice::Text);
    QJsonObject obj;
    obj.insert("language", m_language);
    settingsFile.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}
