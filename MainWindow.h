#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QBasicTimer>

namespace CirSim {
class Circuit;
class Pin;
class Net;
}

class EditorWidget;
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
    friend class EditorWidget;
protected:
    class OscDockWidget : public QDockWidget
    {
    protected:
        OscilloscopeWidget *m_oscilloscope;
    public:
        OscDockWidget(MainWindow *parent);
        OscilloscopeWidget *oscilloscope();
    protected:
        void mousePressEvent(QMouseEvent *event) override;
    };
protected:
    double m_time;
    double m_tickTime;
    qint64 m_frameTime;
    double m_playbackSpeed;
    CirSim::Circuit *m_circuit;
    QList<CurrentProbe> m_currentProbes;
    QList<VoltageProbe> m_voltageProbes;
    EditorWidget *m_editor;
    OscDockWidget *m_oscilloscopeDock;
    QDockWidget *m_inspectorDock;

private:
    qint64 m_realTime;
    QBasicTimer *m_timer;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setInspector(QWidget *inspector);
    bool isSimulating();
    QByteArray serialize();
    void deserialize(const QByteArray &bytes);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

public slots:
    void startSimulation();
    void stopSimulation();
};
#endif // MAINWINDOW_H
