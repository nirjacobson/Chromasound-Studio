#include "romwidget.h"
#include "ui_romwidget.h"

ROMWidget::ROMWidget(QWidget *parent, Application* app, Channel::Type romType)
    : QWidget(parent)
    , ui(new Ui::ROMWidget)
    , _app(app)
    , _tableModel(this, app, _keys, _samples, romType)
    , _sampleDelegate(this, app, romType)
    , _romType(romType)
    , _settings(nullptr)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    ui->tableView->setModel(&_tableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegateForColumn(0, &_keyDelegate);
    ui->tableView->setItemDelegateForColumn(1, &_sampleDelegate);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    if (app) {
        QStringList items = ROM(app->project().resolve(_romType == Channel::Type::SPCM ? app->project().spcmFile() : app->project().dpcmFile())).names();
        ui->stackedWidget->setCurrentIndex(!items.empty());
    }

    connect(&_tableModel, &ROMWidgetTableModel::updated, this, &ROMWidget::tableModelUpdated);
    connect(ui->addButton, &QPushButton::clicked, this, &ROMWidget::addClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &ROMWidget::removeClicked);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ROMWidget::selectionChanged);

}

ROMWidget::~ROMWidget()
{
    delete ui;
}

void ROMWidget::setApplication(Application* app)
{
    _app = app;
    _sampleDelegate.setApplication(app);
    _tableModel.setApplication(app);

    QStringList items = ROM(app->project().resolve(_romType == Channel::Type::SPCM ? app->project().spcmFile() : app->project().dpcmFile())).names();
    ui->stackedWidget->setCurrentIndex(!items.empty());
}

void ROMWidget::setSettings(ROMChannelSettings* settings)
{
    _settings = settings;

    _tableModel.clear();
    for (auto it = _settings->keySampleMappings().begin(); it != _settings->keySampleMappings().end(); ++it) {
        _tableModel.insertRow(keyToString(it.key()), it.value());
    }
}

void ROMWidget::doUpdate()
{
    if (_settings) setSettings(_settings);

    QStringList items = ROM(_app->project().resolve(_romType == Channel::Type::SPCM ? _app->project().spcmFile() : _app->project().dpcmFile())).names();
    ui->stackedWidget->setCurrentIndex(!items.empty());
}

void ROMWidget::setROMType(const Channel::Type romType)
{
    _romType = romType;
    _sampleDelegate.setROMType(romType);
    _tableModel.setROMType(romType);

    doUpdate();
}

ROMWidget::SampleItemDelegate::SampleItemDelegate(QObject* parent, Application* app, Channel::Type romType)
    : QStyledItemDelegate(parent)
    , _app(app)
    , _romType(romType)
{

}

QWidget* ROMWidget::SampleItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* comboBox = new QComboBox(parent);

    QStringList items = ROM(_app->project().resolve(_romType == Channel::Type::SPCM ? _app->project().spcmFile() : _app->project().dpcmFile())).names();
    comboBox->addItems(items);

    return comboBox;
}

void ROMWidget::SampleItemDelegate::setApplication(Application* app)
{
    _app = app;
}

void ROMWidget::SampleItemDelegate::setROMType(const Channel::Type romType)
{
    _romType = romType;
}

void ROMWidget::SampleItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int sample = index.model()->data(index, Qt::EditRole).toInt();
    dynamic_cast<QComboBox*>(editor)->setCurrentIndex(sample);
}

void ROMWidget::SampleItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    int sample = dynamic_cast<QComboBox*>(editor)->currentIndex();

    model->setData(index, sample);
}

int ROMWidget::stringToKey(const QString& str)
{
    bool ok = true;

    int result = str.toInt(&ok);

    if (ok) return result;

    QMap<QChar, int> intervals = {
        {'C', 0},
        {'D', 2},
        {'E', 4},
        {'F', 5},
        {'G', 7},
        {'A', 9},
        {'B', 11}
    };

    int octave = QString(str[str.length() - 1]).toInt();

    QChar note = str.toUpper()[0];

    int key = (12 * octave) + intervals[note];

    if (str.length() == 3) {
        QChar step = str.toUpper()[1];

        if (step == 'B') {
            key--;
        } else if (step == '#') {
            key++;
        }
    }

    return key;
}

QString ROMWidget::keyToString(const int key)
{
    QMap<int, QChar> notes = {
        {0, 'C'},
        {2, 'D'},
        {4, 'E'},
        {5, 'F'},
        {7, 'G'},
        {9, 'A'},
        {11, 'B'}
    };

    int octave = key / 12;

    int interval = key % 12;

    QChar note;
    QChar step = 'N';
    if (notes.contains(interval)) {
        note = notes[interval];
    } else if (notes.contains(interval + 1)) {
        note = notes[interval + 1];
        step = 'b';
    } else if (notes.contains(interval - 1)) {
        note = notes[interval - 1];
        step = '#';
    }

    return (step == 'N')
               ? QString("%1%2").arg(note).arg(QString::number(octave))
               : QString("%1%2%3").arg(note).arg(step).arg(QString::number(octave));

}

void ROMWidget::tableModelUpdated()
{
    *_settings = ROMChannelSettings();

    for (int i = 0; i < _keys.size(); i++) {
        _settings->setSample(stringToKey(_keys[i]), _samples[i]);
    }

    doUpdate();
}

void ROMWidget::addClicked()
{
    _tableModel.insertRow();
}

void ROMWidget::removeClicked()
{
    _tableModel.removeRow(ui->tableView->selectionModel()->selectedIndexes().first().row());
}

void ROMWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    ui->removeButton->setEnabled(!selected.indexes().empty() && selected.indexes().first().row() >= 0);
}

void ROMWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "PCM Layout (*.lay)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodeSettings(_settings));
        file.close();
    }
}

void ROMWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "PCM Layout (*.lay)", nullptr, QFileDialog::DontUseNativeDialog);

    if (!path.isNull()) {
        ROMChannelSettings* settings = BSON::decodePCMLayout(path);
        *_settings = *settings;
        setSettings(_settings);
        delete settings;
    }
}

void ROMWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void ROMWidget::dropEvent(QDropEvent* event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString path(data);
    path = path.mid(QString("file://").length());
    path = path.replace("%20", " ");
    path = path.split("\r\n").at(0);

    QFile file(path);
    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "lay") {
        ROMChannelSettings* settings = BSON::decodePCMLayout(path);
        *_settings = *settings;
        setSettings(_settings);
        delete settings;

        event->acceptProposedAction();
    }
}

QWidget* ROMWidget::KeyItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    QValidator* validator = new QRegularExpressionValidator(QRegularExpression("^([A-Ga-g][b|#]?\\d)|\\d+$"), parent);

    lineEdit->setValidator(validator);

    return lineEdit;
}

void ROMWidget::KeyItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString key = index.model()->data(index).toString();
    dynamic_cast<QLineEdit*>(editor)->setText(key);
}

void ROMWidget::KeyItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QString key = dynamic_cast<QLineEdit*>(editor)->text();

    model->setData(index, key);
}
