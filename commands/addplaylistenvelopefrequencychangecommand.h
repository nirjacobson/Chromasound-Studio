#ifndef ADDPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
#define ADDPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistEnvelopeFrequencyChangeCommand : public QUndoCommand
{
    public:
        AddPlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int freq);

    private:
        MainWindow* _mainWindow;
        Playlist& _playlist;
        float _time;
        int _freq;

        Playlist::EnvelopeFrequencyChange* _change;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // ADDPLAYLISTENVELOPEFREQUENCYCHANGECOMMAND_H
