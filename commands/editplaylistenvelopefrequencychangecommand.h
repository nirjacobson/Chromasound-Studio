#ifndef EDITPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
#define EDITPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class EditPlaylistEnvelopeFrequencyChangeCommand : public QUndoCommand
{
public:
    EditPlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist::EnvelopeFrequencyChange* change, const int newFreq);

private:
    MainWindow* _mainWindow;
    Playlist::EnvelopeFrequencyChange* _change;
    int _oldFreq;
    int _newFreq;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
