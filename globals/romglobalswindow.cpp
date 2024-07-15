#include "romglobalswindow.h"
#include "ui_romglobalswindow.h"

ROMGlobalsWindow::ROMGlobalsWindow(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMGlobalsWindow)
    , _app(app)
{
    ui->setupUi(this);

    ui->romGlobalsWidgetStatic->setApplication(app);
    ui->romGlobalsWidgetDynamic->setApplication(app);

    ui->romGlobalsWidgetStatic->setROMFile(app->project().resolve(app->project().spcmFile()));
    ui->romGlobalsWidgetDynamic->setROMFile(app->project().resolve(app->project().dpcmFile()));

    connect(ui->romGlobalsWidgetStatic, &ROMGlobalsWidget::updated, this, &ROMGlobalsWindow::staticUpdated);
    connect(ui->romGlobalsWidgetDynamic, &ROMGlobalsWidget::updated, this, &ROMGlobalsWindow::dynamicUpdated);
}

ROMGlobalsWindow::~ROMGlobalsWindow()
{
    delete ui;
}

void ROMGlobalsWindow::doUpdate()
{
    ui->romGlobalsWidgetStatic->doUpdate();
    ui->romGlobalsWidgetDynamic->doUpdate();
}

void ROMGlobalsWindow::staticUpdated()
{
    _app->undoStack().push(new SetProjectSPCMFileCommand(_app->window(), _app->project(), ui->romGlobalsWidgetStatic->romFile()));
}

void ROMGlobalsWindow::dynamicUpdated()
{
    _app->undoStack().push(new SetProjectDPCMFileCommand(_app->window(), _app->project(), ui->romGlobalsWidgetDynamic->romFile()));
}

void ROMGlobalsWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
