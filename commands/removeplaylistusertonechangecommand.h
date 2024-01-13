#ifndef REMOVEPLAYLISTUSERTONECHANGECOMMAND_H
#define REMOVEPLAYLISTUSERTONECHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistUserToneChangeCommand : public QUndoCommand
{
public:
    RemovePlaylistUserToneChangeCommand(MainWindow* window, Playlist& playlist, Playlist::UserToneChange* change);
    ~RemovePlaylistUserToneChangeCommand();

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    Playlist::UserToneChange* _change;

    bool _performed;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPLAYLISTUSERTONECHANGECOMMAND_H
