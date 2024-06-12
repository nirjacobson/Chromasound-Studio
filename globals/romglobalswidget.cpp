#include "romglobalswidget.h"
#include "ui_romglobalswidget.h"

ROMGlobalsWidget::ROMGlobalsWidget(QWidget *parent, Application* app)
    : QMainWindow(parent)
    , ui(new Ui::ROMGlobalsWidget)
    , _app(app)
    , _rom(app->project().resolve(app->project().romFile()))
    , _tableModel(this, _rom)
{
    ui->setupUi(this);

    setAcceptDrops(true);

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
    ui->stackedWidget->setCurrentIndex(!_app->project().romFile().isEmpty());

    ui->label->setText(QFileInfo(_app->project().romFile()).fileName());

    _rom = ROM(_app->project().resolve(_app->project().romFile()));
    _tableModel.doUpdate();
}

void ROMGlobalsWidget::load(const QString& path)
{
    if (!path.isNull()) {
        _app->undoStack().push(new SetProjectROMFileCommand(_app->window(), _app->project(), path));
    }
}

void ROMGlobalsWidget::open()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound ROM (*.chr)", nullptr, QFileDialog::DontUseNativeDialog);

    load(path);
}

void ROMGlobalsWidget::closeEvent(QCloseEvent* event)
{
    MdiSubWindow* subwindow = dynamic_cast<MdiSubWindow*>(parent());
    subwindow->close();
}

void ROMGlobalsWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void ROMGlobalsWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString pathsString(data);
    QStringList paths = pathsString.split("\r\n");
    paths.removeDuplicates();
    paths.removeAll("");

    for (QString& str : paths) {
        str = str.mid(QString("file://").length());
        str = str.replace("%20", " ");
    }

    QFile file(paths.first());
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "chr") {
        load(paths.first());
    }
}
