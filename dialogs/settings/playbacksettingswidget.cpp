#include "playbacksettingswidget.h"
#include "ui_playbacksettingswidget.h"

PlaybackSettingsWidget::PlaybackSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaybackSettingsWidget)
{
    ui->setupUi(this);

    QSettings settings("settings.ini", QSettings::IniFormat);

    if (settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString() == Chromasound_Studio::Chromasound) {
        ui->chromasoundRadioButton->setChecked(true);
    } else {
        ui->standardRadioButton->setChecked(true);
    }
}

PlaybackSettingsWidget::~PlaybackSettingsWidget()
{
    delete ui;
}

QString PlaybackSettingsWidget::format() const
{
    return ui->chromasoundRadioButton->isChecked() ? Chromasound_Studio::Chromasound : Chromasound_Studio::Standard;
}
