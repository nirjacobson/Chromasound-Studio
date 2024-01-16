#include "oplimportdialog.h"
#include "ui_oplimportdialog.h"

OPLImportDialog::OPLImportDialog(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OPLImportDialog)
    , _tableModel(this, _patchNames, _patchOctaves)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->patchTableView->setModel(&_tableModel);
    ui->patchTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->patchTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    connect(ui->actionOpen, &QAction::triggered, this, &OPLImportDialog::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &OPLImportDialog::saveTriggered);
    connect(ui->actionSaveAll, &QAction::triggered, this, &OPLImportDialog::saveAllTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &OPLImportDialog::close);

    connect(ui->patchTableView, &TableView::keyPressed, this, &OPLImportDialog::keyPressedOnTable);

    connect(ui->sendButton, &QPushButton::pressed, this, &OPLImportDialog::sendTriggered);
}

OPLImportDialog::~OPLImportDialog()
{
    delete ui;
}

void OPLImportDialog::setApplication(Application* app)
{
    _app = app;
}

void OPLImportDialog::load(const QString& path)
{
    clear();

    quint8 ym2413[0x40];
    memset(ym2413, 0, 0x40);

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    char header[512];
    in.readRawData(header, VGM_HEADER_DATA_OFFSET + 4);

    quint32 relativeDataOffset = *(quint32*)&header[VGM_HEADER_DATA_OFFSET];
    if (relativeDataOffset == 0) {
        relativeDataOffset = 0x0C;
    }
    quint32 dataOffset = VGM_HEADER_DATA_OFFSET + relativeDataOffset;
    in.readRawData(header + VGM_HEADER_DATA_OFFSET + 4, relativeDataOffset - 4);
    quint8 command = 0x00;
    quint8 aa = 0x00;
    quint8 dd = 0x00;
    while (!in.atEnd() && command != 0x66) {
        in >> command;

        switch (command) {
            case 0x50:
                in >> dd;
                break;
            case 0xA0:
                in >> aa;
                in >> dd;
                break;
            case 0x51:
                in >> aa;
                in >> dd;
                ym2413[aa] = dd;
                break;
            case 0x61:
                in >> dd;
                in >> dd;
                break;
            default:
                break;
        }

            if (command == 0x51 && aa >= 0x20 && aa <= 0x28 && (dd & 0x10) > 0 && (ym2413[aa + 0x10] >> 4) == 0) {
            int channel = aa - 0x20;

            // create FM2Settings from memory
            FM2Settings channelSettings;

            quint8 datum = ym2413[0x00];
            channelSettings.operators()[0].setAm(datum & (1 << 7));
            channelSettings.operators()[0].setVib(datum & (1 << 6));
            channelSettings.operators()[0].envelopeSettings().setType(static_cast<FM2EnvelopeSettings::EGType>(datum & (1 << 5)));
            channelSettings.operators()[0].setKsr(datum & (1 << 4));
            channelSettings.operators()[0].setMulti(datum & 0x0F);

            datum = ym2413[0x01];
            channelSettings.operators()[1].setAm(datum & (1 << 7));
            channelSettings.operators()[1].setVib(datum & (1 << 6));
            channelSettings.operators()[1].envelopeSettings().setType(static_cast<FM2EnvelopeSettings::EGType>(datum & (1 << 5)));
            channelSettings.operators()[1].setKsr(datum & (1 << 4));
            channelSettings.operators()[1].setMulti(datum & 0x0F);

            datum = ym2413[0x02];
            channelSettings.operators()[0].setKsl(datum >> 6);
            channelSettings.setTl(datum & 0x3F);

            datum = ym2413[0x03];
            channelSettings.operators()[1].setKsl(datum >> 6);
            channelSettings.operators()[1].setD(datum & (1 << 4));
            channelSettings.operators()[0].setD(datum & (1 << 3));
            channelSettings.setFb(datum & 0x7);

            datum = ym2413[0x04];
            channelSettings.operators()[0].envelopeSettings().setAr(datum >> 4);
            channelSettings.operators()[0].envelopeSettings().setDr(datum & 0xF);

            datum = ym2413[0x05];
            channelSettings.operators()[1].envelopeSettings().setAr(datum >> 4);
            channelSettings.operators()[1].envelopeSettings().setDr(datum & 0x0F);

            datum = ym2413[0x06];
            channelSettings.operators()[0].envelopeSettings().setSl(datum >> 4);
            channelSettings.operators()[0].envelopeSettings().setRr(datum & 0xF);

            datum = ym2413[0x07];
            channelSettings.operators()[1].envelopeSettings().setSl(datum >> 4);
            channelSettings.operators()[1].envelopeSettings().setRr(datum & 0xF);

            // store if not already stored
            int idx = ensurePatch(channelSettings);

            // associate with frequency number just used
            quint16 dataH = dd & 0xF;
            quint8 octave = dataH >> 1;

            if (!_patchOctaves[idx].contains(octave)) {
                _patchOctaves[idx].append(octave);
            }
        }
    }

    for (QList<int>& octaves : _patchOctaves) {
        std::sort(octaves.begin(), octaves.end());
    }

    file.close();
}

void OPLImportDialog::clear()
{
    _patchSettings.clear();
    _tableModel.clear();
}

int OPLImportDialog::ensurePatch(const FM2Settings& settings)
{
    for (int i = 0; i < _patchSettings.size(); i++) {
        if (_patchSettings[i] == settings) {
            return i;
        }
    }

    _patchSettings.append(settings);
    _tableModel.insertRow(QString("Patch %1").arg(_patchSettings.size(), 3, 10, QLatin1Char('0')), QList<int>());

    return _patchSettings.size() - 1;
}

void OPLImportDialog::openTriggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "VGM file (*.vgm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        load(path);
    }
}

void OPLImportDialog::saveTriggered()
{
    if (ui->patchTableView->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox messageBox;
        messageBox.setText("Please select a patch.");
        messageBox.exec();
        return;
    }

    int index = ui->patchTableView->selectionModel()->selectedIndexes().first().row();

    QString path = QFileDialog::getSaveFileName(this, tr("Save patch"), QString("%1.opl").arg(_patchNames[index]), "YM2413 Patch (*.opl)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile patchFile(path);
        patchFile.open(QIODevice::WriteOnly);
        patchFile.write(BSON::encodePatch(&_patchSettings[index]));
        patchFile.close();
    }
}

void OPLImportDialog::saveAllTriggered()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Save all patches"), "", QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile manifestFile(path + QDir::separator() + "Manifest");
        manifestFile.open(QIODevice::WriteOnly);
        QTextStream manifestStream(&manifestFile);

        for (int i = 0; i < _patchSettings.size(); i++) {
            QString patchFilename = QString("%1.opl").arg(_patchNames[i]);
            QFile patchFile(path + QDir::separator() + patchFilename);
            patchFile.open(QIODevice::WriteOnly);
            patchFile.write(BSON::encodePatch(&_patchSettings[i]));
            patchFile.close();

            manifestStream << patchFilename << "\t";
            QStringList octaves;
            for (int j = 0; j < _patchOctaves[i].size(); j++) {
                octaves.append(QString::number(_patchOctaves[i][j]));
            }
            manifestStream << octaves.join(", ");
            manifestStream << "\r\n";
        }

        manifestFile.close();
    }
}

void OPLImportDialog::keyPressedOnTable(int key)
{
    if (key == Qt::Key::Key_Delete) {
        if (ui->patchTableView->selectionModel()->selectedIndexes().isEmpty()) {
            return;
        }

        int index = ui->patchTableView->selectionModel()->selectedIndexes().first().row();

        _patchSettings.remove(index);
        _tableModel.removeRow(index);
    }
}

void OPLImportDialog::sendTriggered()
{
    if (ui->patchTableView->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox messageBox;
        messageBox.setText("Please select a patch.");
        messageBox.exec();
        return;
    }

    int selectedIndex = ui->patchTableView->selectionModel()->selectedIndexes().first().row();

    _app->undoStack().push(new EditFM2SettingsCommand(_app->window(), _app->project().userTone(), _patchSettings[selectedIndex]));
}

void OPLImportDialog::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void OPLImportDialog::dropEvent(QDropEvent* event)
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

    if (fileInfo.suffix() == "vgm") {
        load(paths.first());
    }
}
