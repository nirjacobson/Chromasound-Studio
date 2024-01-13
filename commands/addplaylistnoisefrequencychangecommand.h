#ifndef ADDPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
#define ADDPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistNoiseFrequencyChangeCommand : public QUndoCommand
{
    public:
        AddPlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int freq);

    private:
        MainWindow* _mainWindow;
        Playlist& _playlist;
        float _time;
        int _freq;

        Playlist::NoiseFrequencyChange* _change;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // ADDPLAYLISTNOISEFREQUENCYCHANGECOMMAND_H
