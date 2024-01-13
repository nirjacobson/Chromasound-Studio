#ifndef ADDPLAYLISTUSERTONECHANGECOMMAND_H
#define ADDPLAYLISTUSERTONECHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistUserToneChangeCommand : public QUndoCommand
{
public:
    AddPlaylistUserToneChangeCommand(MainWindow* window, Playlist& playlist, const float time);

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    float _time;

    Playlist::UserToneChange* _change;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDPLAYLISTUSERTONECHANGECOMMAND_H
