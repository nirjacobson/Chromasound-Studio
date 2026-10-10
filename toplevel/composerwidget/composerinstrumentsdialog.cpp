#include "composerinstrumentsdialog.h"
#include "ui_composerinstrumentsdialog.h"

ComposerInstrumentsDialog::ComposerInstrumentsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ComposerInstrumentsDialog)
{
    ui->setupUi(this);
}

ComposerInstrumentsDialog::~ComposerInstrumentsDialog()
{
    delete ui;
}
