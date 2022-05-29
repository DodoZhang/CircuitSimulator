#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QMap>
#include <QBasicTimer>

class QTranslator;

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
    int m_maxIterations;
    double m_maxAcceptableError;
    int m_iterateLevel;
    CirSim::Circuit *m_circuit;
    QList<CurrentProbe> m_currentProbes;
    QList<VoltageProbe> m_voltageProbes;
    EditorWidget *m_editor;
    OscDockWidget *m_oscilloscopeDock;
    QDockWidget *m_inspectorDock;

private:
    qint64 m_realTime;
    QBasicTimer *m_timer;
    bool m_hasFilePath;
    QString m_filePath;
    QString m_language;
    QTranslator *m_translator;
    QAction *m_inspectorAction, *m_oscilloscopeAction;
    QMap<QString, QAction *> m_languageActions;

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
    void newFile();
    void saveFile();
    void saveAsFile();
    void openFile();

protected slots:
    void setLanguage(const QString language);
    void loadSettings();
    void saveSettings();
};
#endif // MAINWINDOW_H
