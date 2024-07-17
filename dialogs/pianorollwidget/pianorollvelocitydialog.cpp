#include "pianorollvelocitydialog.h"
#include "ui_pianorollvelocitydialog.h"

PianoRollVelocityDialog::PianoRollVelocityDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PianoRollVelocityDialog)
{
    ui->setupUi(this);

    connect(ui->setRadioButton, &QRadioButton::clicked, this, &PianoRollVelocityDialog::radioButtonClicked);
    connect(ui->scaleRadioButton, &QRadioButton::clicked, this, &PianoRollVelocityDialog::radioButtonClicked);
    connect(ui->addRadioButton, &QRadioButton::clicked, this, &PianoRollVelocityDialog::radioButtonClicked);
}

PianoRollVelocityDialog::~PianoRollVelocityDialog()
{
    delete ui;
}

PianoRollVelocityDialog::Operation PianoRollVelocityDialog::operation() const
{
    if (ui->setRadioButton->isChecked()) {
        return Operation::Set;
    }

    if (ui->scaleRadioButton->isChecked()) {
        return Operation::Scale;
    }

    if (ui->addRadioButton->isChecked()) {
        return Operation::Add;
    }

    return Operation::Set;
}

int PianoRollVelocityDialog::value() const
{
    return ui->valueSpinBox->value();
}

double PianoRollVelocityDialog::doubleValue() const
{
    return ui->valueDoubleSpinBox->value();
}

void PianoRollVelocityDialog::setOperation(const Operation op)
{
    switch (op) {
        case Set:
            ui->setRadioButton->setChecked(true);
            ui->stackedWidget->setCurrentIndex(0);
            break;
        case Scale:
            ui->scaleRadioButton->setChecked(true);
            ui->stackedWidget->setCurrentIndex(1);
            break;
        case Add:
            ui->addRadioButton->setChecked(true);
            ui->stackedWidget->setCurrentIndex(0);
            break;
    }
}

void PianoRollVelocityDialog::setValue(const int val)
{
    ui->valueSpinBox->setValue(val);
}

void PianoRollVelocityDialog::radioButtonClicked(bool checked)
{
    ui->stackedWidget->setCurrentIndex(ui->scaleRadioButton->isChecked());

    if (ui->addRadioButton->isChecked()) {
        ui->valueSpinBox->setValue(0);
    }
}
