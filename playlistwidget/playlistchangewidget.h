#ifndef PLAYLISTCHANGEWIDGET_H
#define PLAYLISTCHANGEWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/playlist.h"
#include "commands/editplaylistlfochangecommand.h"
#include "commands/editplaylistnoisefrequencychangecommand.h"
#include "commands/editplaylistenvelopefrequencychangecommand.h"
#include "commands/editssgenvelopesettingscommand.h"
#include "commands/editfm2settingscommand.h"

namespace Ui {
class PlaylistChangeWidget;
}

class PlaylistChangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistChangeWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~PlaylistChangeWidget();

    void setApplication(Application* app);

    void setLFOChange(Playlist::LFOChange* change);
    void setNoiseFrequencyChange(Playlist::NoiseFrequencyChange* change);
    void setEnvelopeFrequencyChange(Playlist::EnvelopeFrequencyChange* change);
    void setEnvelopeShapeChange(Playlist::EnvelopeShapeChange* change);
    void setUserToneChange(Playlist::UserToneChange* change);

    void doUpdate();

signals:
    void doneButtonClicked();
    void removeButtonClicked();

private:
    Ui::PlaylistChangeWidget *ui;
    Application* _app;

    Playlist::LFOChange* _lfoChange;
    Playlist::NoiseFrequencyChange* _noiseFrequencyChange;
    Playlist::EnvelopeFrequencyChange* _envelopeFrequencyChange;
    Playlist::EnvelopeShapeChange* _envelopeShapeChange;
    Playlist::UserToneChange* _userToneChange;

private slots:
    void lfoChanged();
    void noiseFrequencyChanged();
    void envelopeFrequencyChanged();
    void envelopeShapeChanged();
    void userToneChanged();
};

#endif // PLAYLISTCHANGEWIDGET_H
