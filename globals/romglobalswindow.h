#ifndef ROMGLOBALSWINDOW_H
#define ROMGLOBALSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "application.h"
#include "commands/setprojectpcmfilecommand.h"
#include "commands/setromchannelsettingscommand.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
class ROMGlobalsWindow;
}

class ROMGlobalsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ROMGlobalsWindow(QWidget *parent = nullptr, Application* app = nullptr);
    ~ROMGlobalsWindow();

    void doUpdate();

private:
    Ui::ROMGlobalsWindow *ui;
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

#endif // ROMGLOBALSWINDOW_H
