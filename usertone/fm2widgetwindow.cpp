#include "fm2widgetwindow.h"
#include "ui_fm2widgetwindow.h"

FM2WidgetWindow::FM2WidgetWindow(QWidget* parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::FM2WidgetWindow)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);
}

FM2WidgetWindow::~FM2WidgetWindow()
{
    delete ui;
}

void FM2WidgetWindow::setSettings(FM2Settings* settings)
{
    ui->fmWidget->setSettings(settings);
}

void FM2WidgetWindow::doUpdate()
{
    ui->fmWidget->doUpdate();
}

void FM2WidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
