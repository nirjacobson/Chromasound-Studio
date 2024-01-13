#ifndef RHYTHMWIDGET_H
#define RHYTHMWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/rhythmchannelsettings.h"
#include "commands/editrhythmchannelsettingscommand.h"

namespace Ui {
class RhythmWidget;
}

class RhythmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RhythmWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~RhythmWidget();

    void setApplication(Application* app);

    void setSettings(RhythmChannelSettings* settings);
    void doUpdate();

private:
    Ui::RhythmWidget *ui;
    Application* _app;
    RhythmChannelSettings* _settings;

private slots:
    void selectionChanged(int index);
};

#endif // RHYTHMWIDGET_H
