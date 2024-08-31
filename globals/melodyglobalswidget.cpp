#include "melodyglobalswidget.h"
#include "ui_melodyglobalswidget.h"

MelodyGlobalsWidget::MelodyGlobalsWidget(QWidget* parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::MelodyGlobalsWidget),
    _app(app)
{
    ui->setupUi(this);

    ui->fmWidget->setApplication(app);
    ui->patchsetWidget->setApplication(app);

    connect(ui->fmWidget, &OPLWidget::changed, this, &MelodyGlobalsWidget::fmChanged);
    connect(ui->patchsetWidget, &MelodyPatchsetWidget::changed, this, &MelodyGlobalsWidget::patchsetChanged);

    connect(ui->actionNew, &QAction::triggered, ui->fmWidget, &OPLWidget::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, ui->fmWidget, &OPLWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, ui->fmWidget, &OPLWidget::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

    ui->menubar->setNativeMenuBar(false);
}

MelodyGlobalsWidget::~MelodyGlobalsWidget()
{
    delete ui;
}

void MelodyGlobalsWidget::setSettings(OPLSettings* settings)
{
    _settings = settings;
    ui->fmWidget->setSettings(_settings);
}

void MelodyGlobalsWidget::doUpdate()
{
    ui->fmWidget->setSettings(_settings);
    ui->patchsetWidget->setPatchset(_app->project().opllType());
}

void MelodyGlobalsWidget::patchsetChanged()
{
    _app->undoStack().push(new EditProjectOPLLTypeCommand(_app->window(), _app->project(), ui->patchsetWidget->selectedPatchset()));
}

void MelodyGlobalsWidget::fmChanged()
{
    _app->undoStack().push(new EditOPLSettingsCommand(_app->window(), *_settings, ui->fmWidget->settings()));
}

void MelodyGlobalsWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
