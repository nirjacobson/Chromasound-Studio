#include "algorithmwidget.h"
#include "ui_algorithmwidget.h"

AlgorithmWidget::AlgorithmWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlgorithmWidget)
{
    ui->setupUi(this);

    ui->prevButton->setEnabled(false);

    QCommonStyle style;
    ui->prevButton->setIcon(style.standardIcon(QStyle::SP_ArrowLeft));
    ui->nextButton->setIcon(style.standardIcon(QStyle::SP_ArrowRight));

    connect(ui->prevButton, &QPushButton::clicked, this, &AlgorithmWidget::prevAlgorithm);
    connect(ui->nextButton, &QPushButton::clicked, this, &AlgorithmWidget::nextAlgorithm);
    connect(ui->fbDial, &QDial::valueChanged, this, &AlgorithmWidget::feedbackDialChanged);
}

AlgorithmWidget::~AlgorithmWidget()
{
    delete ui;
}

void AlgorithmWidget::setApplication(Application* app)
{
    _app = app;
}

void AlgorithmWidget::setSettings(AlgorithmSettings* settings)
{
    _settings = settings;

    ui->prevButton->setEnabled(_settings->algorithm() != 0);
    ui->nextButton->setEnabled(_settings->algorithm() != 7);

    ui->displayWidget->setAlgorithm(_settings->algorithm());
    ui->displayWidget->setFeedback(_settings->feedback());
    ui->algorithmLabel->setText(QString::number(_settings->algorithm()));

    ui->fbDial->blockSignals(true);
    ui->fbDial->setValue(_settings->feedback());
    ui->fbDial->blockSignals(false);
    ui->fbValueLabel->setText(QString::number(_settings->feedback()));
}

void AlgorithmWidget::prevAlgorithm()
{
    AlgorithmSettings newSettings(*_settings);
    newSettings.setAlgorithm(newSettings.algorithm() - 1);
    _app->undoStack().push(new EditAlgorithmSettingsCommand(_app->window(), *_settings, newSettings));
}

void AlgorithmWidget::nextAlgorithm()
{
    AlgorithmSettings newSettings(*_settings);
    newSettings.setAlgorithm(newSettings.algorithm() + 1);
    _app->undoStack().push(new EditAlgorithmSettingsCommand(_app->window(), *_settings, newSettings));
}

void AlgorithmWidget::feedbackDialChanged(const int feedback)
{
    AlgorithmSettings newSettings(*_settings);
    newSettings.setFeedback(feedback);
    _app->undoStack().push(new EditAlgorithmSettingsCommand(_app->window(), *_settings, newSettings));
}
