#include "projectinfodialog.h"
#include "ui_projectinfodialog.h"

ProjectInfoDialog::ProjectInfoDialog(QWidget *parent, Application* app) :
    QDialog(parent),
    ui(new Ui::ProjectInfoWidget),
    _app(app)
{
    ui->setupUi(this);

    doUpdate();

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProjectInfoDialog::accepted);
}

ProjectInfoDialog::~ProjectInfoDialog()
{
    delete ui;
}

void ProjectInfoDialog::doUpdate()
{
    ui->titleLineEdit->setText(_app->project().info().title());
    ui->gameLineEdit->setText(_app->project().info().game());
    ui->authorLineEdit->setText(_app->project().info().author());
    ui->releaseDateEdit->setDate(_app->project().info().releaseDate());
    ui->notesTextEdit->setPlainText(_app->project().info().notes());
    ui->showOnOpenCheckBox->setChecked(_app->project().showInfoOnOpen());
}

void ProjectInfoDialog::accepted()
{
    Project::Info info;
    info.setTitle(ui->titleLineEdit->text());
    info.setGame(ui->gameLineEdit->text());
    info.setAuthor(ui->authorLineEdit->text());
    info.setReleaseDate(ui->releaseDateEdit->date());
    info.setNotes(ui->notesTextEdit->toPlainText());

    _app->undoStack().push(new SetProjectInfoCommand(_app->window(), _app->project(), info, ui->showOnOpenCheckBox->isChecked()));

    close();
}
