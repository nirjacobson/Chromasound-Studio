#ifndef SSGWIDGET_H
#define SSGWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/ssgchannelsettings.h"
#include "commands/editssgchannelsettingscommand.h"

namespace Ui {
class SSGWidget;
}

class SSGWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SSGWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~SSGWidget();

    void setSettings(SSGChannelSettings* settings);
    void doUpdate();

private:
    Ui::SSGWidget *ui;
    Application* _app;
    SSGChannelSettings* _settings;

private slots:
    void toneLEDClicked(bool shift);
    void noiseLEDClicked(bool shift);
    void envLEDClicked(bool shift);
};

#endif // SSGWIDGET_H
