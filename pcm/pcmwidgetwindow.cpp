#include "pcmwidgetwindow.h"
#include "ui_pcmwidgetwindow.h"

PCMWidgetWindow::PCMWidgetWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::PCMWidgetWindow)
{
    ui->setupUi(this);

    ui->widget->setApplication(app);

    connect(ui->actionOpen, &QAction::triggered, ui->widget, &PCMWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->widget, &PCMWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    ui->menubar->setNativeMenuBar(false);
}

PCMWidgetWindow::~PCMWidgetWindow()
{
    delete ui;
}

void PCMWidgetWindow::setSettings(PCMChannelSettings* settings)
{
    _settings = settings;
    ui->widget->setSettings(settings);
}

void PCMWidgetWindow::doUpdate()
{
    ui->widget->doUpdate();
}

void PCMWidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
