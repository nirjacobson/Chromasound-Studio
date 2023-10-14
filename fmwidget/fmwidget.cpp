#include "fmwidget.h"
#include "ui_fmwidget.h"

FMWidget::FMWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FMWidget)
{
    ui->setupUi(this);

    connect(ui->algorithmWidget, &AlgorithmWidget::algorithmChanged, this, &FMWidget::algorithmChanged);
    connect(ui->algorithmWidget, &AlgorithmWidget::feedbackChanged, this, &FMWidget::feedbackChanged);
    connect(ui->pianoWidget, &PianoWidget::keyPressed, this, &FMWidget::keyPressed);
    connect(ui->pianoWidget, &PianoWidget::keyReleased, this, &FMWidget::keyReleased);

    connect(ui->actionOpen, &QAction::triggered, this, &FMWidget::openTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &FMWidget::saveTriggered);
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

void FMWidget::pressKey(const int key)
{
    ui->pianoWidget->pressKey(key);
}

void FMWidget::releaseKey(const int key)
{
    ui->pianoWidget->releaseKey(key);
}

void FMWidget::algorithmChanged(const int a)
{
    _settings->setAlgorithm(a);
}

void FMWidget::feedbackChanged(const int fb)
{
    _settings->setFeedback(fb);
}


void FMWidget::openTriggered()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "", "YM2612 Patch (*.fm)");

    if (!path.isNull()) {
        FMChannelSettings* settings = BSON::decodePatch(path);
        *_settings = *settings;
        delete settings;

        setSettings(_settings);
    }
}

void FMWidget::saveTriggered()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save file"), "", "YM2612 Patch (*.fm)");

    if (!path.isNull()) {
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(BSON::encodePatch(_settings));
        file.close();
    }
}
