#include "fmglobalswidget.h"
#include "ui_fmglobalswidget.h"

FMGlobalsWidget::FMGlobalsWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::FMGlobalsWidget),
    _app(app)
{
    ui->setupUi(this);

    ui->lfoWidget->set(_app->project().lfoMode());

    connect(ui->lfoWidget, &LFOWidget::changed, this, &FMGlobalsWidget::lfoModeChanged);
}

FMGlobalsWidget::~FMGlobalsWidget()
{
    delete ui;
}

void FMGlobalsWidget::doUpdate()
{
    ui->lfoWidget->set(_app->project().lfoMode());
}

void FMGlobalsWidget::lfoModeChanged()
{
    _app->undoStack().push(new EditProjectLFOModeCommand(_app->window(), _app->project(), ui->lfoWidget->setting()));
}

void FMGlobalsWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
