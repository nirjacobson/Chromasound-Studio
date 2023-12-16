#ifndef REMOVEPLAYLISTLFOCHANGECOMMAND_H
#define REMOVEPLAYLISTLFOCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistLFOChangeCommand : public QUndoCommand
{
    public:
        RemovePlaylistLFOChangeCommand(MainWindow* window, Playlist& playlist, Playlist::LFOChange* change);
        ~RemovePlaylistLFOChangeCommand();

    private:
        MainWindow* _mainWindow;
        Playlist& _playlist;
        Playlist::LFOChange* _change;

        bool _performed;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // REMOVEPLAYLISTLFOCHANGECOMMAND_H
