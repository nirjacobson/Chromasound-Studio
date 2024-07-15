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

    connect(ui->actionOpen, &QAction::triggered, this, &ROMGlobalsWindow::open);
    connect(ui->actionReset, &QAction::triggered, this, &ROMGlobalsWindow::reset);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
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

void ROMGlobalsWindow::open()
{
    if (ui->tabWidget->currentIndex() == 0) {
        ui->romGlobalsWidgetStatic->open();
    } else {
        ui->romGlobalsWidgetDynamic->open();
    }
}

void ROMGlobalsWindow::reset()
{
    if (ui->tabWidget->currentIndex() == 0) {
        ui->romGlobalsWidgetStatic->reset();
    } else {
        ui->romGlobalsWidgetDynamic->reset();
    }
}

void ROMGlobalsWindow::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
