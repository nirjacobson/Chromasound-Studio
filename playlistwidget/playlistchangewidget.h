#ifndef PLAYLISTCHANGEWIDGET_H
#define PLAYLISTCHANGEWIDGET_H

#include <QWidget>

#include "project/playlist.h"

namespace Ui {
class PlaylistChangeWidget;
}

class PlaylistChangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistChangeWidget(QWidget *parent = nullptr);
    ~PlaylistChangeWidget();

    void setLFOChange(Playlist::LFOChange* change);
    void setNoiseFrequencyChange(Playlist::NoiseFrequencyChange* change);
    void setEnvelopeFrequencyChange(Playlist::EnvelopeFrequencyChange* change);
    void setEnvelopeShapeChange(Playlist::EnvelopeShapeChange* change);

signals:
    void doneButtonClicked();
    void removeButtonClicked();

private:
    Ui::PlaylistChangeWidget *ui;

    Playlist::LFOChange* _lfoChange;
    Playlist::NoiseFrequencyChange* _noiseFrequencyChange;
    Playlist::EnvelopeFrequencyChange* _envelopeFrequencyChange;
    Playlist::EnvelopeShapeChange* _envelopeShapeChange;

private slots:
    void lfoChanged();
    void noiseFrequencyChanged();
    void envelopeFrequencyChanged();
    void envelopeShapeChanged();
};

#endif // PLAYLISTCHANGEWIDGET_H
