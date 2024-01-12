#ifndef REMOVEPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
#define REMOVEPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistEnvelopeFrequencyChangeCommand : public QUndoCommand
{
public:
    RemovePlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist& playlist, Playlist::EnvelopeFrequencyChange* change);
    ~RemovePlaylistEnvelopeFrequencyChangeCommand();

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    Playlist::EnvelopeFrequencyChange* _change;

    bool _performed;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
