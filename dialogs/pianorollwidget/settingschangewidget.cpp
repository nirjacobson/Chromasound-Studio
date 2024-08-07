#include "settingschangewidget.h"
#include "ui_settingschangewidget.h"

SettingsChangeWidget::SettingsChangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsChangeWidget)
{
    ui->setupUi(this);

    connect(ui->fmWidget, &OPNWidget::keyPressed, this, &SettingsChangeWidget::keyOn);
    connect(ui->fmWidget, &OPNWidget::keyReleased, this, &SettingsChangeWidget::keyOff);

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
    ui->ssgWidget->setApplication(app);
    ui->melodyWidget->setApplication(app);
    ui->rhythmWidget->setApplication(app);
    ui->romWidget->setApplication(app);
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
    ui->ssgWidget->doUpdate();
    ui->melodyWidget->doUpdate();
    ui->rhythmWidget->doUpdate();
    ui->romWidget->doUpdate();
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
            try {
                SSGChannelSettings& scs = dynamic_cast<SSGChannelSettings&>(settings);
                ui->ssgWidget->setSettings(&scs);
                ui->stackedWidget->setCurrentIndex(3);
            } catch (std::bad_cast) {
                try {
                    MelodyChannelSettings& mcs = dynamic_cast<MelodyChannelSettings&>(settings);
                    ui->melodyWidget->setSettings(&mcs);
                    ui->stackedWidget->setCurrentIndex(4);
                } catch (std::bad_cast) {
                    try {
                        RhythmChannelSettings& rcs = dynamic_cast<RhythmChannelSettings&>(settings);
                        ui->rhythmWidget->setSettings(&rcs);
                        ui->stackedWidget->setCurrentIndex(5);
                    } catch (std::bad_cast) {
                        try {
                            ROMChannelSettings& rcs = dynamic_cast<ROMChannelSettings&>(settings);
                            ui->romWidget->setSettings(&rcs);
                            ui->stackedWidget->setCurrentIndex(6);
                        } catch (std::bad_cast) {
                            ui->stackedWidget->setCurrentIndex(0);
                        }
                    }
                }
            }
        }
    }

    ui->footerWidget->setSettings(settings);
}

void SettingsChangeWidget::setROMType(const Channel::Type type)
{
    ui->romWidget->setROMType(type);
}

