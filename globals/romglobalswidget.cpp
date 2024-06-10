#include "romglobalswidget.h"
#include "ui_romglobalswidget.h"

ROMGlobalsWidget::ROMGlobalsWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMGlobalsWidget)
    , _app(app)
    , _rom(app->project().romFile())
    , _tableModel(this, _rom)
{
    ui->setupUi(this);

    ui->tableView->setModel(&_tableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    ui->label->setText(QFileInfo(_app->project().romFile()).fileName());

    connect(ui->actionOpen, &QAction::triggered, this, &ROMGlobalsWidget::open);
    connect(ui->actionClose, &QAction::triggered, this, &QMainWindow::close);
}

ROMGlobalsWidget::~ROMGlobalsWidget()
{
    delete ui;
}

void ROMGlobalsWidget::doUpdate()
{
    ui->label->setText(QFileInfo(_app->project().romFile()).fileName());

    _rom = ROM(_app->project().romFile());
    _tableModel.doUpdate();
}

void ROMGlobalsWidget::open()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound ROM (*.chr)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        _app->undoStack().push(new SetProjectROMFileCommand(_app->window(), _app->project(), path));
    }
}

void ROMGlobalsWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}
