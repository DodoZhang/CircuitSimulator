#include "MainWindow.h"

#include <QDockWidget>
#include <QDateTime>

#include "Simulation/Circuit.h"
#include "Simulation/Elements/FunctionGenerator.h"
#include "Simulation/Elements/Resistance.h"
#include "Simulation/Elements/Capacity.h"
#include "Simulation/Elements/Inductor.h"
#include "Simulation/Elements/Diode.h"
#include "Simulation/Elements/MOSFET.h"

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

    auto *S = new CirSim::FunctionGenerator(m_circuit, "5 * cos(3.1415926 * (t % 64) * (t % 64) / 64)");
    auto *C = new CirSim::Capacity(m_circuit, 0.1);
    auto *L = new CirSim::Inductor(m_circuit, 2.5);
    auto *RC = new CirSim::Resistance(m_circuit, 5);
    auto *RL = new CirSim::Resistance(m_circuit, 5);

    S->pin(S->Positive)->connect(RC->pin(0));
    S->pin(S->Positive)->connect(RL->pin(0));
    RC->pin(1)->connect(C->pin(0));
    RL->pin(1)->connect(L->pin(0));
    C->pin(1)->connect(S->pin(S->Negative));
    L->pin(1)->connect(S->pin(S->Negative));
    m_circuit->setGround(S->pin(S->Negative)->net());

    m_currentProbes.append(CurrentProbe { C->pin(0), "iC" });
    m_currentProbes.append(CurrentProbe { L->pin(0), "iL" });
    m_voltageProbes.append(VoltageProbe { S->pin(S->Positive)->net(), "vS" });
    m_voltageProbes.append(VoltageProbe { C->pin(0)->net(), "vC" });
    m_voltageProbes.append(VoltageProbe { L->pin(0)->net(), "vL" });

    m_oscilloscope = new OscilloscopeWidget(this);
    setCentralWidget(m_oscilloscope);
    QDockWidget *dockWidget = new QDockWidget(this);
    m_oscilloscope->inspectorWidget()->setParameter("Signal Count", "3");
    m_oscilloscope->inspectorWidget()->setParameter("t/div", "1");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 1//Value", "vS");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 1//v/div", "2.5");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//Value", "iC");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//v/div", "1");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 2//Color", "64, 255, 128");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 3//Value", "iL");
    m_oscilloscope->inspectorWidget()->setParameter("Signal 3//v/div", "1");
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
