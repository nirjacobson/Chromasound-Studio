#include "projectinfoscreendialog.h"
#include "ui_projectinfoscreendialog.h"

ProjectInfoScreenDialog::ProjectInfoScreenDialog(QWidget *parent, const Project::Info& info)
    : QDialog(parent)
    , ui(new Ui::ProjectInfoScreenDialog)
{
    ui->setupUi(this);

    ui->titleLabel->setText(info.title());
    ui->gameLabel->setText(info.game());
    ui->authorLabel->setText(QString("by %1").arg(info.author()));
    ui->yearLabel->setText(QString::number(info.releaseDate().year()));
    ui->notesLabel->setText(info.notes());

    if (info.game().isEmpty()) {
        ui->gameLabel->setVisible(false);
    }

    if (info.notes().isEmpty()) {
        ui->line->setVisible(false);
        ui->notesLabel->setVisible(false);
    }
}

ProjectInfoScreenDialog::~ProjectInfoScreenDialog()
{
    delete ui;
}
