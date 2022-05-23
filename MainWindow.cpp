#include "MainWindow.h"

#include <QDockWidget>
#include <QDateTime>

#include "Simulation/Circuit.h"
#include "Simulation/Elements/FunctionGenerator.h"
#include "Simulation/Elements/Resistance.h"
#include "Simulation/Elements/Capacity.h"
#include "Simulation/Elements/Inductor.h"
#include "Simulation/Elements/Diode.h"

#include "Graphic/OscilloscopeWidget.h"
#include "Graphic/ParametersInputWidget.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_frameTime = 20;
    m_tickTime = 1e-4;
    m_playbackSpeed = 1;
    m_timer = new QBasicTimer();

    m_circuit = new CirSim::Circuit();

    auto *S = new CirSim::FunctionGenerator(m_circuit, SIN_WAVE(5, 1));
    auto *RS = new CirSim::Resistance(m_circuit, 1);
    auto *D1 = new CirSim::Diode(m_circuit);
    auto *D2 = new CirSim::Diode(m_circuit);
    auto *D3 = new CirSim::Diode(m_circuit);
    auto *D4 = new CirSim::Diode(m_circuit);
    auto *C = new CirSim::Capacity(m_circuit, 0.1);
    auto *RL = new CirSim::Resistance(m_circuit, 100);

    S->pin(S->Positive)->connect(RS->pin(0));
    RS->pin(1)->connect(D1->pin(D1->Positive));
    S->pin(S->Negative)->connect(D2->pin(D2->Positive));
    S->pin(S->Negative)->connect(D3->pin(D3->Negative));
    RS->pin(1)->connect(D4->pin(D4->Negative));
    RL->pin(0)->connect(D1->pin(D1->Negative));
    RL->pin(0)->connect(D2->pin(D2->Negative));
    RL->pin(1)->connect(D3->pin(D3->Positive));
    RL->pin(1)->connect(D4->pin(D4->Positive));
    RL->pin(0)->connect(C->pin(0));
    RL->pin(1)->connect(C->pin(1));
    m_circuit->setGround(S->pin(S->Negative)->net());

    m_currentProbes.append(CurrentProbe { S->pin(S->Negative), "IS" });
    m_voltageProbes.append(VoltageProbe { RS->pin(1)->net(), "VS" });
    m_voltageProbes.append(VoltageProbe { RL->pin(0)->net(), "VRL0" });
    m_voltageProbes.append(VoltageProbe { RL->pin(1)->net(), "VRL1" });

    m_oscilloscope = new OscilloscopeWidget(this);
    setCentralWidget(m_oscilloscope);
    QDockWidget *dockWidget = new QDockWidget(this);
    m_oscilloscope->inspectorWidget()->setParameter("Signal Count", "3");
    m_oscilloscope->inspectorWidget()->setParameter("t/div", "0.25");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 1//Value", "VS");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 1//v/div", "2.5");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//Value", "VRL0 - VRL1");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//v/div", "2.5");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//Color", "64, 255, 128");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 3//Value", "IS");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 3//v/div", "0.1");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 3//Color", "64, 128, 255");
    dockWidget->setWidget(m_oscilloscope->inspectorWidget());
    dockWidget->setFloating(true);
    dockWidget->show();

    startSimulation();
}

MainWindow::~MainWindow()
{
    endSimulation();
    delete m_timer;
    // TODO release other resources;
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
        m_oscilloscope->record(m_time, para);
    }
#ifdef QT_DEBUG
    if (m_time < nextTime) qDebug() << "Simulation Timeout! Please reduce the Playback Speed or the simulation speed will be unstable.";
#endif
    m_oscilloscope->update();
}

void MainWindow::startSimulation()
{
    m_time = 0;
    m_realTime = 0;
    m_circuit->solve();
    m_timer->start(m_frameTime, this);
}

void MainWindow::endSimulation()
{
    m_timer->stop();
}
