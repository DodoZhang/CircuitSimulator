#include "OscilloscopeWidget.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>

#include "ParametersInputWidget.h"

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QWidget(parent)
{
    m_signalCount = 1;
    m_divw = 100;
    m_divh = 100;
    m_tpdiv = 1;
    m_scanLine = 0;
    m_time = 0;
    m_pixmap = new QPixmap(width(), height());
    m_inspector = new ParametersInputWidget();
    m_inspector->addParameter(&m_signalCount, tr("Signal Count"), PIWItemType(int));
    m_inspector->addParameter(&m_divw, tr("DIV Size//Width"), PIWItemType(int));
    m_inspector->addParameter(&m_divh, tr("DIV Size//Height"), PIWItemType(int));
    m_inspector->addParameter(&m_tpdiv, tr("t/div"), PIWItemType(double));

    setWindowTitle(tr("Oscilloscope"));
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMinimumWidth(m_divw);
    connect(m_inspector, &ParametersInputWidget::parameterChanged, this, &OscilloscopeWidget::parameterChanged);

    parameterChanged(&m_signalCount);

    initializePixmap();
}

OscilloscopeWidget::~OscilloscopeWidget()
{
    delete m_pixmap;
}

ParametersInputWidget *OscilloscopeWidget::inspectorWidget() { return m_inspector; }

QJsonObject OscilloscopeWidget::toJson()
{
    QJsonObject json;
    json.insert("div size", QJsonArray { m_divw, m_divh });
    json.insert("t/div", m_tpdiv);
    QJsonArray signalArray;
    for (int i = 0, s = m_signalParameters.count(); i < s; i ++)
    {
        QJsonObject signalObj;
        signalObj.insert("value", m_signalParameters[i]->value.get());
        signalObj.insert("v/div", m_signalParameters[i]->vpdiv);
        signalObj.insert("color", QJsonArray { m_signalParameters[i]->color.red(),
                                               m_signalParameters[i]->color.green(),
                                               m_signalParameters[i]->color.blue() });
        signalArray.append(signalObj);
    }
    json.insert("signals", signalArray);
    return json;
}

void OscilloscopeWidget::fromJson(const QJsonObject &json)
{
    m_divw = json["div size"].toArray().at(0).toInt();
    m_divh = json["div size"].toArray().at(1).toInt();
    m_tpdiv = json["t/div"].toDouble();
    parameterChanged(&m_divw);
    QJsonArray signalArray = json["signals"].toArray();
    m_signalCount = signalArray.count();
    parameterChanged(&m_signalCount);
    for (int i = 0; i < m_signalCount; i ++)
    {
        QJsonObject signalObj = signalArray[i].toObject();
        m_signalParameters[i]->value.set(signalObj["value"].toString());
        m_signalParameters[i]->vpdiv = signalObj["v/div"].toDouble();
        QJsonArray colorArray = signalObj["color"].toArray();
        m_signalParameters[i]->color = QColor(colorArray[0].toInt(), colorArray[1].toInt(), colorArray[2].toInt());
    }
    m_inspector->updateParameters();
}

void OscilloscopeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_pixmap);
}

void OscilloscopeWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    delete m_pixmap;
    m_pixmap = new QPixmap(width(), height());
    initializePixmap();
}

void OscilloscopeWidget::initializePixmap()
{
    QPainter painter(m_pixmap);
    painter.setPen(QColor(32, 32, 32));
    painter.setBrush(QBrush(QColor(32, 32, 32)));
    painter.drawRect(0, 0, width(), height());
    painter.setPen(QPen(QColor(192, 192, 192), 2));
    painter.drawLine(0, height() / 2, width(), height() / 2);
    painter.drawLine(1, 0, 1, height());
    painter.setPen(QPen(QColor(128, 128, 128), 1, Qt::DashLine));
    for (int i = height() / 2 - m_divh; i >= 0; i -= m_divh)
        painter.drawLine(0, i, width(), i);
    for (int i = height() / 2 + m_divh; i < height(); i += m_divh)
        painter.drawLine(0, i, width(), i);
    for (int i = m_divw; i < width(); i += m_divw)
        painter.drawLine(i, 0, i, height());

    m_scanLine = (int) (m_time / m_tpdiv * m_divw) % (width() / m_divw * m_divw);
}

void OscilloscopeWidget::record(double time, const QMap<QString, double> &values)
{
    m_time = time;
    while ((int) (time / m_tpdiv * m_divw) % (width() / m_divw * m_divw) != m_scanLine) forward();
    for (int i = 0; i < m_signalCount; i ++)
    {
        m_signalValues[i]->cur = m_signalParameters[i]->value.calc(values);
        if (m_signalValues[i]->cur > m_signalValues[i]->max)
            m_signalValues[i]->max = m_signalValues[i]->cur;
        if (m_signalValues[i]->cur < m_signalValues[i]->min)
            m_signalValues[i]->min = m_signalValues[i]->cur;
    }

    // Paint Signals
    QPainter painter(m_pixmap);
    for (int i = 0; i < m_signalCount; i ++)
    {
        int minY = height() / 2 - m_signalValues[i]->max / m_signalParameters[i]->vpdiv * m_divh;
        int maxY = height() / 2 - m_signalValues[i]->min / m_signalParameters[i]->vpdiv * m_divh;
        painter.setPen(m_signalParameters[i]->color);
        painter.drawLine(m_scanLine, minY, m_scanLine, maxY);
    }

    // update();
}

void OscilloscopeWidget::forward()
{
    QPainter painter(m_pixmap);
    // Repaint Background
    painter.setPen(QColor(32, 32, 32));
    painter.drawLine(m_scanLine, 0, m_scanLine, height());
    painter.setPen(QColor(192, 192, 192));
    painter.drawPoint(m_scanLine, height() / 2 - 1);
    painter.drawPoint(m_scanLine, height() / 2);
    if (m_scanLine < 2) painter.drawLine(m_scanLine, 0, m_scanLine, height());
    painter.setPen(QColor(128, 128, 128));
    if (m_scanLine % 6 < 4)
    {
        for (int i = height() / 2 - m_divh; i >= 0; i -= m_divh)
            painter.drawPoint(m_scanLine, i);
        for (int i = height() / 2 + m_divh; i < height(); i += m_divh)
            painter.drawPoint(m_scanLine, i);
    }
    if (m_scanLine % m_divw == 0 && m_scanLine != 0)
    {
        painter.setPen(QPen(QColor(128, 128, 128), 1, Qt::DashLine));
        painter.drawLine(m_scanLine, 0, m_scanLine, height());
    }

    // Paint Signals
    for (int i = 0; i < m_signalCount; i ++)
    {
        int minY = height() / 2 - m_signalValues[i]->max / m_signalParameters[i]->vpdiv * m_divh;
        int maxY = height() / 2 - m_signalValues[i]->min / m_signalParameters[i]->vpdiv * m_divh;
        painter.setPen(m_signalParameters[i]->color);
        painter.drawLine(m_scanLine, minY, m_scanLine, maxY);
    }

    if (m_signalCount)
    {
        for (int i = 0; i < m_signalCount; i ++)
        {
            m_signalValues[i]->min = m_signalValues[i]->cur;
            m_signalValues[i]->max = m_signalValues[i]->cur;
        }
    }

    m_scanLine ++;
    if (m_scanLine >= width() / m_divw * m_divw) m_scanLine = 0;

    // Paint Scan Line
    painter.setPen(QColor(192, 192, 192));
    painter.drawLine(m_scanLine, 0, m_scanLine, height());

    // update();
}

void OscilloscopeWidget::parameterChanged(void *parameter)
{
    if (parameter == &m_signalCount)
    {
        while (m_signalCount > m_signalParameters.count())
        {
            auto *sig = new SignalParameter { InfixExpression("0"), 1, QColor(255, 128, 64) };
            QString signalLabel = "Signal " + QString::number(m_signalParameters.count() + 1) + "//";
            m_inspector->addParameter(&sig->value, signalLabel + "Value", PIWItemType(InfixExpression));
            m_inspector->addParameter(&sig->vpdiv, signalLabel + "v/div", PIWItemType(double));
            m_inspector->addParameter(&sig->color, signalLabel + "Color", PIWItemType(QColor));
            m_signalParameters.append(sig);
            m_signalValues.append(new SignalValue { 0, 0, 0 } );
        }
        while (m_signalCount < m_signalParameters.count())
        {
            SignalParameter *sig = m_signalParameters.last();
            m_signalParameters.removeLast();
            m_signalValues.removeLast();
            delete sig;
            QString signalLabel = "Signal " + QString::number(m_signalParameters.count() + 1);
            m_inspector->removeParameter(signalLabel + "//Value");
            m_inspector->removeParameter(signalLabel + "//v/div");
            m_inspector->removeParameter(signalLabel + "//Color");
            m_inspector->removeParameter(signalLabel);
        }
    }
    else if (parameter == &m_divw || parameter == &m_divh)
    {
        setMinimumWidth(m_divw);
        initializePixmap();
    }
    else if (parameter == &m_tpdiv) initializePixmap();
    update();
}
