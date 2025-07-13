#include "pcmglobalswidget.h"
#include "ui_pcmglobalswidget.h"

PCMGlobalsWidget::PCMGlobalsWidget(QWidget *parent, Application* app, const QString& romFilePath)
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

PCMGlobalsWidget::~PCMGlobalsWidget()
{
    delete ui;
}

void PCMGlobalsWidget::setApplication(Application* app)
{
    _app = app;
}

void PCMGlobalsWidget::setROMFile(const QString& path)
{
    _romPath = path;

    doUpdate();

    emit updated();
}

const QString& PCMGlobalsWidget::romFile() const
{
    return _romPath;
}

void PCMGlobalsWidget::doUpdate()
{
    ui->stackedWidget->setCurrentIndex(!_romPath.isEmpty());

    ui->label->setText(QFileInfo(_romPath).fileName());

    _rom = ROM(_romPath);
    _tableModel.doUpdate();
}

void PCMGlobalsWidget::open()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Chromasound ROM (*.rom)", nullptr, QFileDialog::DontUseNativeDialog);

    setROMFile(path);
}

void PCMGlobalsWidget::reset()
{
    setROMFile("");
}


void PCMGlobalsWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void PCMGlobalsWidget::dropEvent(QDropEvent* event)
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

    QString path = paths.first();
#ifdef WIN32
    path.insert(1, ':');
#endif

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "rom") {
        setROMFile(path);
    }
}
