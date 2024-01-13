#ifndef FM2OPERATORWIDGET_H
#define FM2OPERATORWIDGET_H

#include <QWidget>

#include "application.h"
#include "usertone/fm2operatorsettings.h"

namespace Ui {
class FM2OperatorWidget;
}

class FM2OperatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FM2OperatorWidget(QWidget *parent = nullptr);
    ~FM2OperatorWidget();

    const FM2OperatorSettings& settings() const;
    void set(const FM2OperatorSettings& settings);

signals:
    void changed();

private:
    Ui::FM2OperatorWidget *ui;

    FM2OperatorSettings _settings;

private slots:
    void envTypeChanged(const int index);
    void arDialChanged(const int value);
    void drDialChanged(const int value);
    void slDialChanged(const int value);
    void rrDialChanged(const int value);
    void multiDialChanged(const int value);
    void kslDialChanged(const int value);
    void amChanged();
    void vibChanged();
    void ksrChanged();
    void dChanged();
};

#endif // FM2OPERATORWIDGET_H
