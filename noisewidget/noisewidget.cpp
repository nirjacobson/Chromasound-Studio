#include "noisewidget.h"
#include "ui_noisewidget.h"

NoiseWidget::NoiseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoiseWidget)
{
    ui->setupUi(this);

    connect(ui->typeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &NoiseWidget::typeChanged);
    connect(ui->rateComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &NoiseWidget::shiftRateChanged);
}

NoiseWidget::~NoiseWidget()
{
    delete ui;
}

void NoiseWidget::setSettings(NoiseChannelSettings* settings)
{
    _settings = settings;

    ui->typeComboBox->setCurrentIndex(settings->noiseType());
    ui->rateComboBox->setCurrentIndex(settings->shiftRate());
}

void NoiseWidget::typeChanged(int idx)
{
    switch (idx) {
        case 0:
            _settings->setNoiseType(NoiseChannelSettings::NoiseType::Periodic);
            break;
        case 1:
            _settings->setNoiseType(NoiseChannelSettings::NoiseType::White);
            break;
    }
}

void NoiseWidget::shiftRateChanged(int idx)
{
    switch (idx) {
        case 0:
            _settings->setShiftRate(NoiseChannelSettings::ShiftRate::FIVE_TWELVE);
            break;
        case 1:
            _settings->setShiftRate(NoiseChannelSettings::ShiftRate::TEN_TWENTYFOUR);
            break;
        case 2:
            _settings->setShiftRate(NoiseChannelSettings::ShiftRate::TWENTY_FORTYEIGHT);
            break;
    }
}
