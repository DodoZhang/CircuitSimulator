#ifndef PARAMETERSINPUTWIDGET_H
#define PARAMETERSINPUTWIDGET_H

#include <QTreeWidget>

#define PIWItemType(type) &ParametersInputWidget::display_##type, &ParametersInputWidget::update_##type

class ParametersInputWidget : public QTreeWidget
{
    Q_OBJECT
protected:
    class PIWItem : public QTreeWidgetItem
    {
    public:
        void *m_parameter;
        QString (*m_display)(const void *);
        bool (*m_update)(void *, const QString &);

    public:
        PIWItem(ParametersInputWidget *parent,
                const QStringList &strings,
                void *parameter = nullptr,
                QString (*display)(const void *) = nullptr,
                bool (*update)(void *, const QString &) = nullptr);
        PIWItem(PIWItem *parent,
                const QStringList &strings,
                void *parameter = nullptr,
                QString (*display)(const void *) = nullptr,
                bool (*update)(void *, const QString &) = nullptr);
    };

protected:
    QMap<QString, PIWItem *> m_items;
    QMap<QTreeWidgetItem, PIWItem *> m_map;

public:
    explicit ParametersInputWidget(QWidget *parent = nullptr);
    ~ParametersInputWidget();

    void addParameter(void *parameter, const QString &label,
                 QString (*display)(const void *),
                 bool (*update)(void *, const QString &));
    void removeParameter(const QString &label);

    bool setParameter(const QString &label, const QString &value);

    static QString display_int(const void *parameter);
    static bool update_int(void *parameter, const QString &str);
    static QString display_float(const void *parameter);
    static bool update_float(void *parameter, const QString &str);
    static QString display_double(const void *parameter);
    static bool update_double(void *parameter, const QString &str);
    static QString display_QString(const void *parameter);
    static bool update_Qstring(void *parameter, const QString &str);
    static QString display_QColor(const void *parameter);
    static bool update_QColor(void *parameter, const QString &str);
    static QString display_InfixExpression(const void *parameter);
    static bool update_InfixExpression(void *parameter, const QString &str);

signals:
    void parameterChanged(void *parameter);

protected slots:
    void itemEdited(QTreeWidgetItem *item, int column);
};

#endif // PARAMETERSINPUTWIDGET_H
