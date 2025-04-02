#include "pianorollquantizationdivisiondialog.h"
#include "ui_pianorollquantizationdivisiondialog.h"

bool isPowerOf2(int n) {
    return (n & (n - 1)) == 0;
}

PianoRollQuantizationDivisionDialog::PianoRollQuantizationDivisionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PianoRollQuantizationDivisionDialog)
    , _denominator(16)
{
    ui->setupUi(this);
    connect(ui->denominatorSpinBox, &QSpinBox::valueChanged, this, &PianoRollQuantizationDivisionDialog::denominatorChanged);
}

PianoRollQuantizationDivisionDialog::~PianoRollQuantizationDivisionDialog()
{
    delete ui;
}

int PianoRollQuantizationDivisionDialog::denominator() const
{
    return _denominator;
}

void PianoRollQuantizationDivisionDialog::setDenominator(int d)
{
    ui->denominatorSpinBox->setValue(d);
}

void PianoRollQuantizationDivisionDialog::denominatorChanged(int d)
{
    if (isPowerOf2(d)) {
        _denominator = d;
        ui->buttonBox->buttons().first()->setDisabled(false);
    } else {
        ui->buttonBox->buttons().first()->setDisabled(true);
    }
}
