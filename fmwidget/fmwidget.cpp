#include "fmwidget.h"
#include "ui_fmwidget.h"

FMWidget::FMWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FMWidget)
{
    ui->setupUi(this);

    connect(ui->algorithmWidget, &AlgorithmWidget::algorithmChanged, this, &FMWidget::algorithmChanged);
    connect(ui->algorithmWidget, &AlgorithmWidget::feedbackChanged, this, &FMWidget::feedbackChanged);
}

FMWidget::~FMWidget()
{
    delete ui;
}

void FMWidget::setSettings(FMChannelSettings* settings)
{
    _settings = settings;

    ui->operatorWidget1->setSettings(&settings->operators()[0]);
    ui->operatorWidget2->setSettings(&settings->operators()[1]);
    ui->operatorWidget3->setSettings(&settings->operators()[2]);
    ui->operatorWidget4->setSettings(&settings->operators()[3]);

    ui->algorithmWidget->setAlgorithm(_settings->algorithm());
    ui->algorithmWidget->setFeedback(_settings->feedback());
}

void FMWidget::algorithmChanged(const int a)
{
    _settings->setAlgorithm(a);
}

void FMWidget::feedbackChanged(const int fb)
{
    _settings->setFeedback(fb);
}
