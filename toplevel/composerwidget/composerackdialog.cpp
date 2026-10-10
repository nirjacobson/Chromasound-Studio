#include "composerackdialog.h"
#include "ui_composerackdialog.h"

ComposerAckDialog::ComposerAckDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ComposerAckDialog)
{
    ui->setupUi(this);
}

ComposerAckDialog::~ComposerAckDialog()
{
    delete ui;
}
