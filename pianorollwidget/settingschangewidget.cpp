#include "settingschangewidget.h"
#include "ui_settingschangewidget.h"

SettingsChangeWidget::SettingsChangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsChangeWidget)
{
    ui->setupUi(this);

    connect(ui->fmWidget, &FMWidget::keyPressed, this, &SettingsChangeWidget::keyOn);
    connect(ui->fmWidget, &FMWidget::keyReleased, this, &SettingsChangeWidget::keyOff);

    connect(ui->footerWidget, &SettingsChangeFooterWidget::removeClicked, this, &SettingsChangeWidget::removeClicked);
    connect(ui->footerWidget, &SettingsChangeFooterWidget::doneClicked, this, &SettingsChangeWidget::doneClicked);
}

SettingsChangeWidget::~SettingsChangeWidget()
{
    delete ui;
}

void SettingsChangeWidget::setApplication(Application* app)
{
    ui->fmWidget->setApplication(app);
    ui->noiseWidget->setApplication(app);
}

void SettingsChangeWidget::pressKey(const int key)
{
    ui->fmWidget->pressKey(key);
}

void SettingsChangeWidget::releaseKey(const int key)
{
    ui->fmWidget->releaseKey(key);
}

void SettingsChangeWidget::doUpdate()
{
    ui->noiseWidget->doUpdate();
    ui->fmWidget->doUpdate();
}

void SettingsChangeWidget::setSettings(ChannelSettings& settings)
{
    try {
        FMChannelSettings& fmcs = dynamic_cast<FMChannelSettings&>(settings);
        ui->fmWidget->setSettings(&fmcs);
        ui->stackedWidget->setCurrentIndex(1);
    } catch (std::bad_cast) {
        try {
            NoiseChannelSettings& ncs = dynamic_cast<NoiseChannelSettings&>(settings);
            ui->noiseWidget->setSettings(&ncs);
            ui->stackedWidget->setCurrentIndex(2);
        } catch (std::bad_cast) {
            ui->stackedWidget->setCurrentIndex(0);
        }
    }

    ui->footerWidget->setSettings(settings);
}

