#include "romwidgetwindow.h"
#include "ui_romwidgetwindow.h"

ROMWidgetWindow::ROMWidgetWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMWidgetWindow)
{
    ui->setupUi(this);

    ui->widget->setApplication(app);

    connect(ui->actionOpen, &QAction::triggered, ui->widget, &ROMWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->widget, &ROMWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    ui->menubar->setNativeMenuBar(false);
}

ROMWidgetWindow::~ROMWidgetWindow()
{
    delete ui;
}

void ROMWidgetWindow::setSettings(PCMChannelSettings* settings)
{
    _settings = settings;
    ui->widget->setSettings(settings);
}

void ROMWidgetWindow::doUpdate()
{
    ui->widget->doUpdate();
}

void ROMWidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
