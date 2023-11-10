#ifndef PCMWIDGET_H
#define PCMWIDGET_H

#include <QMainWindow>
#include <QFileDialog>

#include "application.h"
#include "mdiarea/mdisubwindow.h"

namespace Ui {
class PCMWidget;
}

class PCMWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit PCMWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~PCMWidget();

    void setSettings(PCMChannelSettings* settings);

private:
    Ui::PCMWidget *ui;
    Application* _app;

    PCMChannelSettings* _settings;

private slots:
    void openTriggered();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);
};

#endif // PCMWIDGET_H
