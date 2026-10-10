#include "composerwidget.h"
#include "ui_composerwidget.h"

ComposerWidget::ComposerWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ComposerWidget)
{
    ui->setupUi(this);
}

ComposerWidget::~ComposerWidget()
{
    delete ui;
}
