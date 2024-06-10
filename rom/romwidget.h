#ifndef ROMWIDGET_H
#define ROMWIDGET_H

#include <QWidget>

#include "application.h"
#include "formats/rom.h"
#include "commands/editromchannelsettingscommand.h"

namespace Ui {
class ROMWidget;
}

class ROMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ROMWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~ROMWidget();

    void setApplication(Application* app);

    void setSettings(ROMChannelSettings* settings);
    void doUpdate();

private:
    Ui::ROMWidget *ui;
    Application* _app;

    ROMChannelSettings* _settings;

private slots:
    void selectionChanged(int index);
};

#endif // ROMWIDGET_H
