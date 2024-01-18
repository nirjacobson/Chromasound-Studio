#include "fmlfowidget.h"
#include "ui_fmlfowidget.h"

FMLFOWidget::FMLFOWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FMLFOWidget)
{
    ui->setupUi(this);

    connect(ui->amsSlider, &QSlider::sliderMoved, this, &FMLFOWidget::amsSliderChanged);
    connect(ui->fmsSlider, &QSlider::sliderMoved, this, &FMLFOWidget::fmsSliderChanged);
}

FMLFOWidget::~FMLFOWidget()
{
    delete ui;
}

void FMLFOWidget::setApplication(Application* app)
{
    _app = app;
}

void FMLFOWidget::setSettings(LFOSettings* settings)
{
    _settings = settings;

    ui->amsSlider->setValue(_settings->ams());
    ui->fmsSlider->setValue(_settings->fms());
}

void FMLFOWidget::amsSliderChanged(const int value)
{
    LFOSettings newSettings(*_settings);
    newSettings.setAMS(value);
    _app->undoStack().push(new EditLFOSettingsCommand(_app->window(), *_settings, newSettings));
}

void FMLFOWidget::fmsSliderChanged(const int value)
{
    LFOSettings newSettings(*_settings);
    newSettings.setFMS(value);
    _app->undoStack().push(new EditLFOSettingsCommand(_app->window(), *_settings, newSettings));
}
