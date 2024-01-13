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
    explicit FM2Widget(QWidget *parent = nullptr, Application* app = nullptr);
    ~FM2Widget();

    void setApplication(Application* app);

    void setSettings(FM2Settings* settings);

    void doUpdate();

private:
    Ui::FM2Widget *ui;
    Application* _app;
    FM2Settings* _settings;

private slots:
    void tlDialChanged(const int value);
    void fbDialChanged(const int value);
};

#endif // FM2WIDGET_H
