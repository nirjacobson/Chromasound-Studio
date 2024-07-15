#include "romglobalswidget.h"
#include "ui_romglobalswidget.h"

ROMGlobalsWidget::ROMGlobalsWidget(QWidget *parent, Application* app, const QString& romFilePath)
    : QWidget(parent)
    , ui(new Ui::ROMGlobalsWidget)
    , _app(app)
    , _rom(romFilePath)
    , _tableModel(this, _rom)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->tableView->setModel(&_tableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
}

ROMGlobalsWidget::~ROMGlobalsWidget()
{
    delete ui;
}

void ROMGlobalsWidget::setApplication(Application* app)
{
    _app = app;
}

void ROMGlobalsWidget::setROMFile(const QString& path)
{
    _romPath = path;

    doUpdate();

    emit updated();
}

const QString& ROMGlobalsWidget::romFile() const
{
    return _romPath;
}

void ROMGlobalsWidget::doUpdate()
{
    ui->stackedWidget->setCurrentIndex(!_romPath.isEmpty());

    ui->label->setText(QFileInfo(_app->project().resolve(_romPath)).fileName());

    _rom = ROM(_app->project().resolve(_romPath));
    _tableModel.doUpdate();
}

void ROMGlobalsWidget::open()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound ROM (*.rom)", nullptr, QFileDialog::DontUseNativeDialog);

    setROMFile(path);
}

void ROMGlobalsWidget::reset()
{
    setROMFile("");
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

    if (fileInfo.suffix() == "rom") {
        setROMFile(paths.first());
    }
}
