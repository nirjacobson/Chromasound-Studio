#ifndef ADDPLAYLISTENVELOPESHAPECHANGECOMMAND_H
#define ADDPLAYLISTENVELOPESHAPECHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistEnvelopeShapeChangeCommand : public QUndoCommand
{
public:
    AddPlaylistEnvelopeShapeChangeCommand(MainWindow* window, Playlist& playlist, const float time);

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    float _time;

    Playlist::EnvelopeShapeChange* _change;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDPLAYLISTENVELOPESHAPECHANGECOMMAND_H
