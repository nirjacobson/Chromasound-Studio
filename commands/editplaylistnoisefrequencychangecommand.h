#ifndef EDITPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
#define EDITPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class EditPlaylistNoiseFrequencyChangeCommand : public QUndoCommand
{
public:
    EditPlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist::NoiseFrequencyChange* change, const int newFreq);

private:
    MainWindow* _mainWindow;
    Playlist::NoiseFrequencyChange* _change;
    int _oldFreq;
    int _newFreq;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
