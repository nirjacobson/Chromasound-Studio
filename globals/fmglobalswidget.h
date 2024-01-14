#ifndef FMGLOBALSWIDGET_H
#define FMGLOBALSWIDGET_H

#include <QWidget>

#include "application.h"
#include "mdiarea/mdisubwindow.h"
#include "commands/editprojectlfomodecommand.h"

namespace Ui {
class FMGlobalsWidget;
}

class FMGlobalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FMGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~FMGlobalsWidget();

    void doUpdate();

private:
    Ui::FMGlobalsWidget *ui;
    Application* _app;

private slots:
    void lfoModeChanged();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);
};

#endif // FMGLOBALSWIDGET_H
