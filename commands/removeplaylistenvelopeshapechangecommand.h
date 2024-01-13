#ifndef REMOVEPLAYLISTENVELOPESHAPECHANGECOMMAND_H
#define REMOVEPLAYLISTENVELOPESHAPECHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistEnvelopeShapeChangeCommand : public QUndoCommand
{
    public:
        RemovePlaylistEnvelopeShapeChangeCommand(MainWindow* window, Playlist& playlist, Playlist::EnvelopeShapeChange* change);
        ~RemovePlaylistEnvelopeShapeChangeCommand();

    private:
        MainWindow* _mainWindow;
        Playlist& _playlist;
        Playlist::EnvelopeShapeChange* _change;

        bool _performed;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // REMOVEPLAYLISTENVELOPESHAPECHANGECOMMAND_H
