#ifndef OSCILLOSCOPEWIDGET_H
#define OSCILLOSCOPEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QList>

#include "InfixExpression.h"

class ParametersInputWidget;

class OscilloscopeWidget : public QWidget
{
    Q_OBJECT
protected:
    typedef struct
    {
        InfixExpression value;
        double vpdiv;
        QColor color;
    } SignalParameter;

    typedef struct
    {
        double max;
        double cur;
        double min;
    } SignalValue;

protected:
    int m_signalCount;
    int m_divw, m_divh;
    double m_tpdiv;
    QVector<SignalParameter *> m_signalParameters;
    QVector<SignalValue *> m_signalValues;
    int m_scanLine;
    double m_time;
    ParametersInputWidget *m_inspector;
    QPixmap *m_pixmap;

public:
    OscilloscopeWidget(QWidget *parent = nullptr);
    ~OscilloscopeWidget();
    ParametersInputWidget *inspectorWidget();
    void record(double time, const QMap<QString, double> &values = QMap<QString, double>());
    QJsonObject toJson();
    void fromJson(const QJsonObject &json);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void initializePixmap();

private slots:
    void forward();

protected slots:
    void parameterChanged(void *parameter);
};

#endif // OSCILLOSCOPEWIDGET_H
