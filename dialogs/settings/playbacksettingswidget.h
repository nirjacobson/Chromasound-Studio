#ifndef PLAYBACKSETTINGSWIDGET_H
#define PLAYBACKSETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

#include "Chromasound_Studio.h"

namespace Ui {
    class PlaybackSettingsWidget;
}

class PlaybackSettingsWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit PlaybackSettingsWidget(QWidget *parent = nullptr);
        ~PlaybackSettingsWidget();

        QString format() const;

    private:
        Ui::PlaybackSettingsWidget *ui;
};

#endif // PLAYBACKSETTINGSWIDGET_H
