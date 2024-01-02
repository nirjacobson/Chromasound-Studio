#include "noisewidget.h"
#include "ui_noisewidget.h"

NoiseWidget::NoiseWidget(QWidget *parent, Application* app)
    : QWidget(parent)
    , ui(new Ui::NoiseWidget)
    , _app(app)
    , _settings(nullptr)
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

    ui->typeComboBox->blockSignals(true);
    ui->typeComboBox->setCurrentIndex(settings->noiseType());
    ui->typeComboBox->blockSignals(false);

    ui->rateComboBox->blockSignals(true);
    ui->rateComboBox->setCurrentIndex(settings->shiftRate());
    ui->rateComboBox->blockSignals(false);
}

void NoiseWidget::doUpdate()
{
    if (_settings) setSettings(_settings);
}

void NoiseWidget::setApplication(Application* app)
{
    _app = app;
}

void NoiseWidget::typeChanged(int idx)
{
    NoiseChannelSettings newSettings(*_settings);

    switch (idx) {
        case 0:
            newSettings.setNoiseType(NoiseChannelSettings::NoiseType::Periodic);
            break;
        case 1:
            newSettings.setNoiseType(NoiseChannelSettings::NoiseType::White);
            break;
    }

    _app->undoStack().push(new EditNoiseChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

void NoiseWidget::shiftRateChanged(int idx)
{
    NoiseChannelSettings newSettings(*_settings);

    switch (idx) {
        case 0:
            newSettings.setShiftRate(NoiseChannelSettings::ShiftRate::FIVE_TWELVE);
            break;
        case 1:
            newSettings.setShiftRate(NoiseChannelSettings::ShiftRate::TEN_TWENTYFOUR);
            break;
        case 2:
            newSettings.setShiftRate(NoiseChannelSettings::ShiftRate::TWENTY_FORTYEIGHT);
            break;
    }

    _app->undoStack().push(new EditNoiseChannelSettingsCommand(_app->window(), *_settings, newSettings));
}

void NoiseWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
