#include "midipadsdialog.h"
#include "ui_midipadsdialog.h"

MIDIPadsDialog::MIDIPadsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MIDIPadsDialog)
    , _n(1)
    , _recording(false)
{
    ui->setupUi(this);

    connect(ui->recordButton, &QPushButton::clicked, this, &MIDIPadsDialog::recordTriggered);
}

MIDIPadsDialog::~MIDIPadsDialog()
{
    delete ui;
}

void MIDIPadsDialog::pad(quint8 id)
{
    if (!_recording) return;

    if (!_setting.isEmpty()) {
        _setting.append(",");
    }

    _setting.append(QString::number(id, 16));

    ui->label->setText(QString("Hit pad %1.").arg(_n++));
}

QString MIDIPadsDialog::setting() const
{
    return _setting;
}

void MIDIPadsDialog::recordTriggered()
{
    _recording = true;

    _setting.clear();

    ui->recordButton->setEnabled(false);

    ui->label->setText(QString("Hit pad %1.").arg(_n++));
}
