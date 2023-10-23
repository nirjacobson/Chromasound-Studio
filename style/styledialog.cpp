#include "styledialog.h"
#include "ui_styledialog.h"

StyleDialog::StyleDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StyleDialog)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &StyleDialog::newTriggered);
    connect(ui->actionOpen, &QAction::triggered, this, &StyleDialog::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &StyleDialog::saveTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &StyleDialog::close);

    connect(ui->resetButton, &QPushButton::clicked, this, &StyleDialog::reset);
    connect(ui->applyButton, &QPushButton::clicked, this, &StyleDialog::apply);
}

StyleDialog::~StyleDialog()
{
    delete ui;
}

void StyleDialog::setApplication(Application* app)
{
    _app = app;
}


void StyleDialog::newTriggered()
{
    ui->styleTextEdit->clear();
}

void StyleDialog::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Cascading style sheets (*.css)");

    if (!path.isNull()) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            ui->styleTextEdit->setPlainText(stream.readAll());
            apply();
            file.close();
            QFileInfo info(file);
            ui->statusbar->showMessage(QString("Opened %1.%2.").arg(info.baseName(), info.completeSuffix()));
        }
    }
}

void StyleDialog::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "Cascading style sheets (*.css)");

    if (!path.isNull()) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << ui->styleTextEdit->toPlainText();
            file.close();
            QFileInfo info(file);
            ui->statusbar->showMessage(QString("Opened %1.%2.").arg(info.baseName(), info.completeSuffix()));
        }
    }
}

void StyleDialog::reset()
{
    _app->setStyleSheet("");
}

void StyleDialog::apply()
{
    _app->setStyleSheet(ui->styleTextEdit->toPlainText());
}

void StyleDialog::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
