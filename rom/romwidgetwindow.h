#ifndef ROMWIDGETWINDOW_H
#define ROMWIDGETWINDOW_H

#include <QMainWindow>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
class ROMWidgetWindow;
}

class ROMWidgetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ROMWidgetWindow(QWidget *parent = nullptr, Application* app = nullptr);
    ~ROMWidgetWindow();

    void setSettings(ROMChannelSettings* settings);
    void doUpdate();

private:
    Ui::ROMWidgetWindow *ui;
    ROMChannelSettings* _settings;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

};

#endif // ROMWIDGETWINDOW_H
