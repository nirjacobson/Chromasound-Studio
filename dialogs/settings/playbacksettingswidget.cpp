#include "playbacksettingswidget.h"
#include "ui_playbacksettingswidget.h"

PlaybackSettingsWidget::PlaybackSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaybackSettingsWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    if (settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString() == Chromasound_Studio::Chromasound) {
        ui->chromasoundRadioButton->setChecked(true);
    } else if (settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString() == Chromasound_Studio::Standard) {
        ui->standardRadioButton->setChecked(true);
    } else {
        ui->legacyRadioButton->setChecked(true);
    }
}

PlaybackSettingsWidget::~PlaybackSettingsWidget()
{
    delete ui;
}

QString PlaybackSettingsWidget::format() const
{
    if (ui->chromasoundRadioButton->isChecked()) {
        return Chromasound_Studio::Chromasound;
    } else if (ui->standardRadioButton->isChecked()) {
        return Chromasound_Studio::Standard;
    } else {
        return Chromasound_Studio::Legacy;
    }
}
