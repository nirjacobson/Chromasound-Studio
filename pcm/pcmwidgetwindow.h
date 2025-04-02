#ifndef PCMWIDGETWINDOW_H
#define PCMWIDGETWINDOW_H

#include <QMainWindow>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
class PCMWidgetWindow;
}

class PCMWidgetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PCMWidgetWindow(QWidget *parent = nullptr, Application* app = nullptr);
    ~PCMWidgetWindow();

    void setSettings(PCMChannelSettings* settings);
    void doUpdate();

private:
    Ui::PCMWidgetWindow *ui;
    PCMChannelSettings* _settings;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

};

#endif // PCMWIDGETWINDOW_H
