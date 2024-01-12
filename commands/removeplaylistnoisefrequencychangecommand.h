#ifndef REMOVEPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
#define REMOVEPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistNoiseFrequencyChangeCommand : public QUndoCommand
{
public:
    RemovePlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist& playlist, Playlist::NoiseFrequencyChange* change);
    ~RemovePlaylistNoiseFrequencyChangeCommand();

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    Playlist::NoiseFrequencyChange* _change;

    bool _performed;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
