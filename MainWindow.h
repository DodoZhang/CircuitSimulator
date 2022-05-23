#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>

namespace CirSim {
class Circuit;
class Pin;
class Net;
}

class OscilloscopeWidget;

typedef struct
{
    CirSim::Pin *pin;
    QString label;
} CurrentProbe;

typedef struct
{
    CirSim::Net *net;
    QString label;
} VoltageProbe;

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    double m_time;
    double m_tickTime;
    qint64 m_frameTime;
    double m_playbackSpeed;
    CirSim::Circuit *m_circuit;
    QList<CurrentProbe> m_currentProbes;
    QList<VoltageProbe> m_voltageProbes;
    OscilloscopeWidget *m_oscilloscope;

private:
    qint64 m_realTime;
    QBasicTimer *m_timer;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event) override;

public slots:
    void startSimulation();
    void endSimulation();
};
#endif // MAINWINDOW_H
