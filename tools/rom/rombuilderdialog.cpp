#include "rombuilderdialog.h"
#include "ui_rombuilderdialog.h"

ROMBuilderDialog::ROMBuilderDialog(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ROMBuilderDialog)
    , _tableModel(this, _names, _paths)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->tableView->setModel(&_tableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableView->setDragEnabled(true);

    connect(ui->moveUpButton, &QPushButton::clicked, this, &ROMBuilderDialog::moveUp);
    connect(ui->moveDownButton, &QPushButton::clicked, this, &ROMBuilderDialog::moveDown);
    connect(ui->removeButton, &QPushButton::clicked, this, &ROMBuilderDialog::remove);
    connect(ui->addButton, &QPushButton::clicked, this, &ROMBuilderDialog::add);

    connect(ui->actionSave, &QAction::triggered, this, &ROMBuilderDialog::save);
    connect(ui->actionClose, &QAction::triggered, this, &QDialog::close);
}

ROMBuilderDialog::~ROMBuilderDialog()
{
    delete ui;
}

QByteArray ROMBuilderDialog::header() const
{
    QByteArray result;

    quint16 count = _names.size();

    result.append((char*)&count, sizeof(count));

    for (int i = 0; i < _names.size(); i++) {
        quint16 offset = _tableModel.offsetOf(i);

        result.append((char*)&offset, sizeof(offset));
        result.append(_names[i].toStdString().c_str());
        result.append((quint8)0x00);
    }

    return result;
}

void ROMBuilderDialog::recalculateSize()
{
    quint32 size = 0;
    QString suffix = "B";

    for (int i = 0; i < _paths.size(); i++) {
        size += QFileInfo(_paths[i]).size();
    }

    if (size >= 1024) {
        size /= 1024;
        suffix = "kB";
    }

    ui->label->setText(QString("Size: %1 %2").arg(size).arg(suffix));
}

void ROMBuilderDialog::add_(const QString& path)
{
    _tableModel.insertRow(QFileInfo(path).baseName(), path);

    recalculateSize();
}

void ROMBuilderDialog::moveUp()
{
    _tableModel.moveUp(ui->tableView->selectionModel()->selectedIndexes().first().row());
}

void ROMBuilderDialog::moveDown()
{
    _tableModel.moveDown(ui->tableView->selectionModel()->selectedIndexes().first().row());
}

void ROMBuilderDialog::remove()
{
    _tableModel.removeRow(ui->tableView->selectionModel()->selectedIndexes().first().row());

    recalculateSize();
}

void ROMBuilderDialog::add()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "PCM file (*.pcm)", nullptr, QFileDialog::DontUseNativeDialog);

    add_(path);
}

void ROMBuilderDialog::save()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save ROM"), "rom.chr", "Chromasound ROM (*.chr)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile romFile(path);
        romFile.open(QIODevice::WriteOnly);

        romFile.write(header());

        for (int i = 0; i < _paths.size(); i++) {
            QFile pcmFile(_paths[i]);
            pcmFile.open(QIODevice::ReadOnly);
            romFile.write(pcmFile.readAll());
        }

        romFile.close();
    }
}

void ROMBuilderDialog::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void ROMBuilderDialog::dropEvent(QDropEvent* event)
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

    for (auto it = paths.begin(); it != paths.end(); ++it) {
        QFile file(*it);
        QFileInfo fileInfo(file);

        if (fileInfo.suffix() == "pcm") {
            add_(*it);
        }
    }
}