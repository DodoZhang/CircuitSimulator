#include "ParametersInputWidget.h"

#include "InfixExpression.h"

ParametersInputWidget::PIWItem::PIWItem(ParametersInputWidget *parent,
                                        const QStringList &strings,
                                        void *parameter,
                                        QString (*display)(const void *),
                                        bool (*update)(void *, const QString &))
    : QTreeWidgetItem(parent, strings)
{
    m_parameter = parameter;
    m_display = display;
    m_update = update;
}

ParametersInputWidget::PIWItem::PIWItem(PIWItem *parent,
                                        const QStringList &strings,
                                        void *parameter,
                                        QString (*display)(const void *),
                                        bool (*update)(void *, const QString &))
    : QTreeWidgetItem(parent, strings)
{
    m_parameter = parameter;
    m_display = display;
    m_update = update;
}

ParametersInputWidget::ParametersInputWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setColumnCount(2);
    setHeaderLabels({ tr("Key"), tr("Value") });
    connect(this, &QTreeWidget::itemChanged, this, &ParametersInputWidget::itemEdited);
}

ParametersInputWidget::~ParametersInputWidget()
{
    for (auto iter = m_items.begin(); iter != m_items.end(); iter ++) delete iter.value();
}

void ParametersInputWidget::addParameter(void *parameter, const QString &label,
                                    QString (*display)(const void *),
                                    bool (*update)(void *, const QString &))
{
    QStringList parts = label.split("//");
    QString tmp = "";
    PIWItem *parent = nullptr;
    for (int i = 0; i < parts.count() - 1; i ++)
    {
        tmp += parts[i];
        if (!m_items.contains(tmp))
        {
            if (parent)
            {
                parent = new PIWItem(parent, { parts[i], "" });
                m_items.insert(tmp, parent);
            }
            else
            {
                parent = new PIWItem(this, { parts[i], "" });
                m_items.insert(tmp, parent);
            }
        }
        else parent = m_items.value(tmp);
        tmp += "//";
    }
    if (parent)
    {
        parent = new PIWItem(parent, { parts.last(), display(parameter) }, parameter, display, update);
        openPersistentEditor(parent, 1);
        m_items.insert(label, parent);
    }
    else
    {
        parent = new PIWItem(this, { parts.last(), display(parameter) }, parameter, display, update);
        openPersistentEditor(parent, 1);
        m_items.insert(label, parent);
    }
}

void ParametersInputWidget::removeParameter(const QString &label)
{
    PIWItem *item = m_items[label];
    delete item;
    m_items.remove(label);
}

bool ParametersInputWidget::setParameter(const QString &label, const QString &value)
{
    if (!m_items.contains(label)) return false;
    PIWItem *piwitem = m_items[label];
    bool ok = piwitem->m_update(piwitem->m_parameter, value);
    piwitem->setText(1, piwitem->m_display(piwitem->m_parameter));
    if (ok) emit parameterChanged(piwitem->m_parameter);
    return ok;
}

QString ParametersInputWidget::display_int(const void *parameter)
{
    return QString::number(*(int *) parameter);
}

bool ParametersInputWidget::update_int(void *parameter, const QString &str)
{
    bool ok;
    int tmp = str.toInt(&ok);
    if (ok) *(int *) parameter = tmp;
    return ok;
}

QString ParametersInputWidget::display_float(const void *parameter)
{
    return QString::number(*(float *) parameter);
}

bool ParametersInputWidget::update_float(void *parameter, const QString &str)
{
    bool ok;
    float tmp = str.toFloat(&ok);
    if (ok) *(float *) parameter = tmp;
    return ok;
}

QString ParametersInputWidget::display_double(const void *parameter)
{
    return QString::number(*(double *) parameter);
}

bool ParametersInputWidget::update_double(void *parameter, const QString &str)
{
    bool ok;
    float tmp = str.toDouble(&ok);
    if (ok) *(double *) parameter = tmp;
    return ok;
}

QString ParametersInputWidget::display_QString(const void *parameter)
{
    return *(QString *) parameter;
}

bool ParametersInputWidget::update_QString(void *parameter, const QString &str)
{
    *(QString *) parameter = str;
    return true;
}

QString ParametersInputWidget::display_QColor(const void *parameter)
{
    return QString::number((*(QColor *) parameter).red()) + ", " +
           QString::number((*(QColor *) parameter).green()) + ", " +
           QString::number((*(QColor *) parameter).blue());
}

bool ParametersInputWidget::update_QColor(void *parameter, const QString &str)
{
    QStringList parts = str.split(",");
    if (parts.count() != 3) return false;
    QColor tmp;
    bool ok;
    tmp.setRed(parts[0].toInt(&ok));
    if (!ok) return false;
    tmp.setGreen(parts[1].toInt(&ok));
    if (!ok) return false;
    tmp.setBlue(parts[2].toInt(&ok));
    if (!ok) return false;
    *(QColor *) parameter = tmp;
    return true;
}

QString ParametersInputWidget::display_InfixExpression(const void *parameter)
{
    return ((InfixExpression *) parameter)->get();
}

bool ParametersInputWidget::update_InfixExpression(void *parameter, const QString &str)
{
    return ((InfixExpression *) parameter)->set(str);
}

void ParametersInputWidget::itemEdited(QTreeWidgetItem* item, int)
{
    PIWItem *piwitem = (PIWItem *) item;
    bool ok =piwitem->m_update(piwitem->m_parameter, piwitem->text(1));
    piwitem->setText(1, piwitem->m_display(piwitem->m_parameter));
    if (ok) emit parameterChanged(piwitem->m_parameter);
}

