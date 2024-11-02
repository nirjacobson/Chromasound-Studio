#ifndef PCMGLOBALSWINDOW_H
#define PCMGLOBALSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "application.h"
#include "commands/setprojectpcmfilecommand.h"
#include "commands/setpcmchannelsettingscommand.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
class PCMGlobalsWindow;
}

class PCMGlobalsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PCMGlobalsWindow(QWidget *parent = nullptr, Application* app = nullptr);
    ~PCMGlobalsWindow();

    void doUpdate();

private:
    Ui::PCMGlobalsWindow *ui;
    Application* _app;

private slots:
    void romUpdated();

    void open();
    void reset();

    void optimizePCM();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);
};

#endif // PCMGLOBALSWINDOW_H
