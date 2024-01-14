#include "fm2widgetwindow.h"
#include "ui_fm2widgetwindow.h"

FM2WidgetWindow::FM2WidgetWindow(QWidget* parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::FM2WidgetWindow),
    _app(app)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);

    connect(ui->fmWidget, &FM2Widget::changed, this, &FM2WidgetWindow::fmChanged);

    connect(ui->actionNew, &QAction::triggered, ui->fmWidget, &FM2Widget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, ui->fmWidget, &FM2Widget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->fmWidget, &FM2Widget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
}

FM2WidgetWindow::~FM2WidgetWindow()
{
    delete ui;
}

void FM2WidgetWindow::setSettings(FM2Settings* settings)
{
    _settings = settings;
    ui->fmWidget->setSettings(_settings);
}

void FM2WidgetWindow::doUpdate()
{
    ui->fmWidget->setSettings(_settings);
}

void FM2WidgetWindow::fmChanged()
{
    _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), *_settings, ui->fmWidget->settings()));
}

void FM2WidgetWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
