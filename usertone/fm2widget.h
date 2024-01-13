#ifndef FM2WIDGET_H
#define FM2WIDGET_H

#include <QWidget>

#include "application.h"
#include "fm2settings.h"
#include "commands/editfm2settingscommand.h"

namespace Ui {
class FM2Widget;
}

class FM2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit FM2Widget(QWidget *parent = nullptr);
    ~FM2Widget();

    const FM2Settings& settings() const;
    void set(const FM2Settings& settings);

signals:
    void changed();

private:
    Ui::FM2Widget *ui;
    FM2Settings _settings;

private slots:
    void operator1Changed();
    void operator2Changed();
    void tlDialChanged(const int value);
    void fbDialChanged(const int value);
};

#endif // FM2WIDGET_H
