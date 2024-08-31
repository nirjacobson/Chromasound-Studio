#include "opnimportdialog.h"
#include "ui_opnimportdialog.h"

OPNImportDialog::OPNImportDialog(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OPNImportDialog)
    , _tableModel(this, _patchNames, _patchOctaves)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->patchTableView->setModel(&_tableModel);
    ui->patchTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->patchTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->channelRadioButton->setChecked(true);

    connect(ui->actionOpen, &QAction::triggered, this, &OPNImportDialog::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &OPNImportDialog::saveTriggered);
    connect(ui->actionSaveAll, &QAction::triggered, this, &OPNImportDialog::saveAllTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &OPNImportDialog::close);

    connect(ui->patchTableView, &TableView::keyPressed, this, &OPNImportDialog::keyPressedOnTable);

    connect(ui->sendButton, &QPushButton::pressed, this, &OPNImportDialog::sendTriggered);

    ui->menubar->setNativeMenuBar(false);
}

OPNImportDialog::~OPNImportDialog()
{
    delete ui;
}

void OPNImportDialog::setApplication(Application* app)
{
    _app = app;
}

void OPNImportDialog::load(const QString& path) {
    if (path.endsWith("vgm")) {
        loadVGM(path);
    } else if (path.endsWith("opm")) {
        loadOPM(path);
    }
}

void OPNImportDialog::loadVGM(const QString& path)
{
    clear();

    quint8 ym2612_1[0xFF];
    quint8 ym2612_2[0xFF];
    memset(ym2612_1, 0, 0xFF);
    memset(ym2612_2, 0, 0xFF);

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
            case 0x52:
                in >> aa;
                in >> dd;
                ym2612_1[aa] = dd;
                break;
            case 0x53:
                in >> aa;
                in >> dd;
                ym2612_2[aa] = dd;
                break;
            case 0x61:
                in >> dd;
                in >> dd;
                break;
            case 0x67:
                in.skipRawData(1);
                quint8 type;
                in >> type;
                quint32 size;
                in >> size;

                in.skipRawData(size);
                break;
            case 0xE0:
                quint32 address;
                in >> address;
                break;
            default:
                break;
        }

        if (command == 0x52 && aa == 0x28 && (dd & 0xF0) > 0) {
            int channel = dd & 0x0F;
            if (channel > 2) channel--;

            quint8* mem = channel < 3 ? ym2612_1 : ym2612_2;

            // create FMChannelSettings from memory
            FMChannelSettings channelSettings;

            for (int i = 0; i < 4; i++) {
                int offset = i * 4 + (channel % 3);

                // DT & MUL
                quint8 data = mem[0x30 + offset];
                quint8 dt = data >> 4;
                quint8 mul = data & 0x0F;
                channelSettings.operators()[i].setDt(dt);
                channelSettings.operators()[i].setMul(mul);

                // TL
                quint8 tl = mem[0x40 + offset];
                channelSettings.operators()[i].envelopeSettings().setT1l(tl);

                // RS & AR
                data = mem[0x50 + offset];
                quint8 rs = data >> 6;
                quint8 ar = data & 0x1F;
                channelSettings.operators()[i].setRs(rs);
                channelSettings.operators()[i].envelopeSettings().setAr(ar);

                // AM & D1R
                data = mem[0x60 + offset];
                bool am = data >> 7;
                quint8 d1r = data & 0x1F;
                channelSettings.operators()[i].setAm(am);
                channelSettings.operators()[i].envelopeSettings().setD1r(d1r);

                // D2R
                quint8 d2r = mem[0x70 + offset];
                channelSettings.operators()[i].envelopeSettings().setD2r(d2r);

                // D1L & RR
                data = mem[0x80 + offset];
                quint8 t2l = (data >> 4) & 0x0F;
                quint8 rr = data & 0x0F;
                channelSettings.operators()[i].envelopeSettings().setT2l(t2l);
                channelSettings.operators()[i].envelopeSettings().setRr(rr);
            }

            int offset = channel % 3;

            // Feedback & algorithm
            quint8 data = mem[0xB0 + offset];
            int feedback = (data >> 3) & 0b111;
            int algorithm = data & 0b111;
            channelSettings.algorithm().setFeedback(feedback);
            channelSettings.algorithm().setAlgorithm(algorithm);

            // LFO
            data = mem[0xB4 + offset];
            int ams = (data >> 4) & 0b11;
            int fms = (data & 0b111);
            channelSettings.lfo().setAMS(ams);
            channelSettings.lfo().setFMS(fms);

            // store if not already stored
            int idx = ensurePatch(channelSettings);

            // associate with frequency number just used
            quint16 dataH = mem[0xA4 + offset];
            quint8 octave = dataH >> 3;

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

void OPNImportDialog::loadOPM(const QString& path)
{
    clear();

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }

        FMChannelSettings settings;

        QString name = line.mid(line.indexOf(QRegularExpression("\\s+")));

        file.readLine();

        line = file.readLine().trimmed();

        QStringList lineParts = line.split(QRegularExpression("\\s+"));

        settings.algorithm().setFeedback(lineParts[2].toInt());
        settings.algorithm().setAlgorithm(lineParts[3].toInt());
        settings.lfo().setAMS(lineParts[4].toInt());
        settings.lfo().setFMS(lineParts[5].toInt());

        for (int i = 0; i < 4; i++) {
            line = file.readLine().trimmed();

            lineParts = line.split(QRegularExpression("\\s+"));

            settings.operators()[i].envelopeSettings().setAr(lineParts[1].toInt());
            settings.operators()[i].envelopeSettings().setD1r(lineParts[2].toInt());
            settings.operators()[i].envelopeSettings().setD2r(lineParts[3].toInt());
            settings.operators()[i].envelopeSettings().setRr(lineParts[4].toInt());
            settings.operators()[i].envelopeSettings().setT2l(lineParts[5].toInt());
            settings.operators()[i].envelopeSettings().setT1l(lineParts[6].toInt());
            settings.operators()[i].setMul(lineParts[8].toInt());
            settings.operators()[i].setAm(lineParts[11].toInt() != 0);
        }

        ensurePatch(settings, name);
    }

    file.close();
}

void OPNImportDialog::clear()
{
    _patchSettings.clear();
    _tableModel.clear();
}

int OPNImportDialog::ensurePatch(const FMChannelSettings& settings, const QString& name)
{
    for (int i = 0; i < _patchSettings.size(); i++) {
        if (_patchSettings[i] == settings) {
            return i;
        }
    }

    _patchSettings.append(settings);
    _tableModel.insertRow(name.isEmpty() ? QString("Patch %1").arg(_patchSettings.size(), 3, 10, QLatin1Char('0')) : name, QList<int>());

    return _patchSettings.size() - 1;
}

void OPNImportDialog::openTriggered()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "VGM files (*.vgm);;OPM files (*.opm)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        load(path);
    }
}

void OPNImportDialog::saveTriggered() {
    if (ui->patchTableView->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox messageBox;
        messageBox.setText("Please select a patch.");
        messageBox.exec();
        return;
    }

    int index = ui->patchTableView->selectionModel()->selectedIndexes().first().row();

    QString path = QFileDialog::getSaveFileName(this, tr("Save patch"), QString("%1.opn").arg(_patchNames[index]), "YM2612 Patch (*.opn)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile patchFile(path);
        patchFile.open(QIODevice::WriteOnly);
        patchFile.write(BSON::encodeSettings(&_patchSettings[index]));
        patchFile.close();
    }
}

void OPNImportDialog::saveAllTriggered()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Save all patches"), "", QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile manifestFile(path + QDir::separator() + "Manifest");
        manifestFile.open(QIODevice::WriteOnly);
        QTextStream manifestStream(&manifestFile);

        for (int i = 0; i < _patchSettings.size(); i++) {
            QString patchFilename = QString("%1.opn").arg(_patchNames[i]);
            QFile patchFile(path + QDir::separator() + patchFilename);
            patchFile.open(QIODevice::WriteOnly);
            patchFile.write(BSON::encodeSettings(&_patchSettings[i]));
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

void OPNImportDialog::keyPressedOnTable(int key)
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

void OPNImportDialog::sendTriggered()
{
    if (ui->patchTableView->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox messageBox;
        messageBox.setText("Please select a patch.");
        messageBox.exec();
        return;
    }

    int selectedIndex = ui->patchTableView->selectionModel()->selectedIndexes().first().row();

    if (ui->newChannelRadioButton->isChecked()) {
        _app->undoStack().push(new AddFMChannelCommand(_app->window(), _patchSettings[selectedIndex], _patchNames[selectedIndex]));
    } else if (ui->channelRadioButton->isChecked()) {
        int channelIndex = ui->channelSpinBox->value() - 1;

        if (channelIndex < _app->project().channelCount()) {
            Channel& channel = _app->project().getChannel(channelIndex);
            _app->undoStack().push(new SetFMChannelCommand(_app->window(), channel, _patchSettings[selectedIndex], _patchNames[selectedIndex]));
        } else {
            _app->undoStack().push(new AddFMChannelCommand(_app->window(), _patchSettings[selectedIndex], _patchNames[selectedIndex]));
        }
    }
}

void OPNImportDialog::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void OPNImportDialog::dropEvent(QDropEvent* event)
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

    if (fileInfo.suffix() == "vgm" || fileInfo.suffix() == "opm") {
        load(paths.first());
    }
}
