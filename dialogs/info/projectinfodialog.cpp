#include "projectinfodialog.h"
#include "ui_projectinfodialog.h"

ProjectInfoDialog::ProjectInfoDialog(QWidget *parent, Application* app) :
    QDialog(parent),
    ui(new Ui::ProjectInfoWidget),
    _app(app)
{
    ui->setupUi(this);

    ui->titleLineEdit->setText(_app->project().info().title());
    ui->gameLineEdit->setText(_app->project().info().game());
    ui->authorLineEdit->setText(_app->project().info().author());
    ui->releaseDateEdit->setDate(_app->project().info().releaseDate());
    ui->notesTextEdit->setPlainText(_app->project().info().notes());
    ui->showOnOpenCheckBox->setChecked(_app->project().showInfoOnOpen());

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProjectInfoDialog::accepted);
}

ProjectInfoDialog::~ProjectInfoDialog()
{
    delete ui;
}

void ProjectInfoDialog::accepted()
{
    _app->project().info().setTitle(ui->titleLineEdit->text());
    _app->project().info().setGame(ui->gameLineEdit->text());
    _app->project().info().setAuthor(ui->authorLineEdit->text());
    _app->project().info().setReleaseDate(ui->releaseDateEdit->date());
    _app->project().info().setNotes(ui->notesTextEdit->toPlainText());
    _app->project().showInfoOnOpen(ui->showOnOpenCheckBox->isChecked());

    close();
}
