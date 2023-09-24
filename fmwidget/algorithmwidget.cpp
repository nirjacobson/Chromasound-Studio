#include "algorithmwidget.h"
#include "ui_algorithmwidget.h"

AlgorithmWidget::AlgorithmWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlgorithmWidget)
    , _algorithm(0)
    , _fb(0)
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

void AlgorithmWidget::setAlgorithm(const int alg)
{
    _algorithm = alg;
    ui->prevButton->setEnabled(_algorithm != 0);
    ui->nextButton->setEnabled(_algorithm != 7);

    ui->displayWidget->setAlgorithm(_algorithm);
    ui->algorithmLabel->setText(QString::number(_algorithm));

    emit algorithmChanged(alg);
}

void AlgorithmWidget::setFeedback(const int fb)
{
    _fb = fb;

    ui->displayWidget->setFeedback(_fb);
    ui->fbValueLabel->setText(QString::number(_fb));
}

void AlgorithmWidget::prevAlgorithm()
{
    ui->nextButton->setEnabled(true);
    ui->prevButton->setEnabled(--_algorithm != 0);

    ui->displayWidget->setAlgorithm(_algorithm);
    ui->algorithmLabel->setText(QString::number(_algorithm));

    emit algorithmChanged(_algorithm);
}

void AlgorithmWidget::nextAlgorithm()
{
    ui->prevButton->setEnabled(true);
    ui->nextButton->setEnabled(++_algorithm != 7);

    ui->displayWidget->setAlgorithm(_algorithm);
    ui->algorithmLabel->setText(QString::number(_algorithm));

    emit algorithmChanged(_algorithm);
}

void AlgorithmWidget::feedbackDialChanged(const int feedback)
{
    _fb = feedback;

    ui->displayWidget->setFeedback(_fb);
    ui->fbValueLabel->setText(QString::number(_fb));

    emit feedbackChanged(_fb);
}
