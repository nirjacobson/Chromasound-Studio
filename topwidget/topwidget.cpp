#include "topwidget.h"
#include "ui_topwidget.h"

TopWidget::TopWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::TopWidget),
    _isPlaying(false)
{
    ui->setupUi(this);

    if (app != nullptr) {
        setApplication(app);
    }

    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaStop));

    connect(ui->patRadioButton, &QRadioButton::clicked, this, &TopWidget::patModeSelected);
    connect(ui->songRadioButton, &QRadioButton::clicked, this, &TopWidget::songModeSelected);
    connect(ui->playButton, &QPushButton::clicked, this, &TopWidget::playPauseClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &TopWidget::stopClicked);
    connect(ui->patSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(patternChanged(int)));
    connect(ui->tempoSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(tempoChanged(int)));
    connect(ui->tempoSpinBox, SIGNAL(valueChanged(int)), this, SLOT(tempoDidChange(int)));
    connect(ui->beatsPerBarSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(beatsPerBarChanged(int)));
    connect(ui->beatsPerBarSpinBox, SIGNAL(valueChanged(int)), this, SLOT(beatsPerBarDidChange(int)));
}

TopWidget::~TopWidget()
{
    delete ui;
}

void TopWidget::setApplication(Application* app)
{
    _app = app;
    ui->seekWidget->setApplication(app);
    ui->positionDisplay->setApplication(app);
    ui->tempoSpinBox->setValue(app->project().tempo());
    ui->beatsPerBarSpinBox->setValue(app->project().beatsPerBar());
}

void TopWidget::setPattern(const int p)
{
    ui->patSpinBox->blockSignals(true);
    ui->patSpinBox->setValue(p+1);
    ui->patSpinBox->blockSignals(false);
}

void TopWidget::setTempo(const int t)
{
    ui->tempoSpinBox->blockSignals(true);
    ui->tempoSpinBox->setValue(t);
    ui->tempoSpinBox->blockSignals(false);
}

void TopWidget::setBeatsPerBar(const int b)
{
    ui->beatsPerBarSpinBox->blockSignals(true);
    ui->beatsPerBarSpinBox->setValue(b);
    ui->beatsPerBarSpinBox->blockSignals(false);
}

void TopWidget::playPauseClicked()
{
    _isPlaying = !_isPlaying;

    ui->playButton->setIcon(ui->playButton->style()->standardIcon(_isPlaying ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));

    if (_isPlaying) {
        emit play();
    } else {
        emit pause();
    }
}

void TopWidget::stopClicked()
{
    _isPlaying = false;
    ui->playButton->setIcon(ui->playButton->style()->standardIcon(QStyle::SP_MediaPlay));

    emit stop();
}

void TopWidget::tempoDidChange(int bpm)
{
    _app->project().setTempo(bpm);
}

void TopWidget::beatsPerBarDidChange(int beats)
{
    _app->project().setBeatsPerBar(beats);
}

void TopWidget::patModeSelected()
{
    _app->project().setPlayMode(Project::PlayMode::PATTERN);
}

void TopWidget::songModeSelected()
{
    _app->project().setPlayMode(Project::PlayMode::SONG);
}

