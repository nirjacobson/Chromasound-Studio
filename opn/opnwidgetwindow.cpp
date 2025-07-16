#include "opnwidgetwindow.h"
#include "ui_opnwidgetwindow.h"

OPNWidgetWindow::OPNWidgetWindow(QWidget *parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::OPNWidgetWindow)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);

    connect(ui->fmWidget, &OPNWidget::keyPressed, this, &OPNWidgetWindow::keyPressed);
    connect(ui->fmWidget, &OPNWidget::keyReleased, this, &OPNWidgetWindow::keyReleased);

    connect(ui->actionNew, &QAction::triggered, ui->fmWidget, &OPNWidget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, ui->fmWidget, &OPNWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->fmWidget, &OPNWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    ui->menubar->setNativeMenuBar(false);
}

OPNWidgetWindow::~OPNWidgetWindow()
{
    delete ui;
}

void OPNWidgetWindow::setSettings(FMChannelSettings* settings)
{
    ui->fmWidget->setSettings(settings);
}

void OPNWidgetWindow::pressKey(const int key)
{
    ui->fmWidget->pressKey(key);
}

void OPNWidgetWindow::releaseKey(const int key)
{
    ui->fmWidget->releaseKey(key);
}

void OPNWidgetWindow::doUpdate()
{
    ui->fmWidget->doUpdate();
}

void OPNWidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
