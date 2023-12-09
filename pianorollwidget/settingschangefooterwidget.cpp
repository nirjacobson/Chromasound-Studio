#include "settingschangefooterwidget.h"
#include "ui_settingschangefooterwidget.h"

SettingsChangeFooterWidget::SettingsChangeFooterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsChangeFooterWidget),
    _settings(nullptr)
{
    ui->setupUi(this);

    connect(ui->volumeDial, &QDial::valueChanged, this, &SettingsChangeFooterWidget::volumeChanged);
    connect(ui->removeButton, &QPushButton::clicked, this, &SettingsChangeFooterWidget::removeClicked);
    connect(ui->doneButton, &QPushButton::clicked, this, &SettingsChangeFooterWidget::doneClicked);
}

SettingsChangeFooterWidget::~SettingsChangeFooterWidget()
{
    delete ui;
}

void SettingsChangeFooterWidget::setSettings(ChannelSettings& settings)
{
    _settings = &settings;
    ui->volumeDial->setValue(settings.volume());
}

void SettingsChangeFooterWidget::volumeChanged(int volume)
{
    ui->volumeValueLabel->setText(QString::number(volume));
    _settings->setVolume(volume);
}
