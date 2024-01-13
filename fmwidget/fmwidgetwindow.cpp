#include "fmwidgetwindow.h"
#include "ui_fmwidgetwindow.h"

FMWidgetWindow::FMWidgetWindow(QWidget *parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::FMWidgetWindow)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);
    
    connect(ui->fmWidget, &FM4Widget::keyPressed, this, &FMWidgetWindow::keyPressed);
    connect(ui->fmWidget, &FM4Widget::keyReleased, this, &FMWidgetWindow::keyReleased);
    
    connect(ui->actionNew, &QAction::triggered, ui->fmWidget, &FM4Widget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, ui->fmWidget, &FM4Widget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->fmWidget, &FM4Widget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
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
