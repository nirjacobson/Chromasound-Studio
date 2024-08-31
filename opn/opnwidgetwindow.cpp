#include "opnwidgetwindow.h"
#include "ui_opnwidgetwindow.h"

FMWidgetWindow::FMWidgetWindow(QWidget *parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::FMWidgetWindow)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);

    connect(ui->fmWidget, &OPNWidget::keyPressed, this, &FMWidgetWindow::keyPressed);
    connect(ui->fmWidget, &OPNWidget::keyReleased, this, &FMWidgetWindow::keyReleased);

    connect(ui->actionNew, &QAction::triggered, ui->fmWidget, &OPNWidget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, ui->fmWidget, &OPNWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->fmWidget, &OPNWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    ui->menubar->setNativeMenuBar(false);
}

FMWidgetWindow::~FMWidgetWindow()
{
    delete ui;
}

void FMWidgetWindow::setSettings(FMChannelSettings* settings)
{
    ui->fmWidget->setSettings(settings);
}

void FMWidgetWindow::pressKey(const int key)
{
    ui->fmWidget->pressKey(key);
}

void FMWidgetWindow::releaseKey(const int key)
{
    ui->fmWidget->releaseKey(key);
}

void FMWidgetWindow::doUpdate()
{
    ui->fmWidget->doUpdate();
}

void FMWidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
