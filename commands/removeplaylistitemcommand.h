#ifndef REMOVEPLAYLISTITEMCOMMAND_H
#define REMOVEPLAYLISTITEMCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistItemCommand : public QUndoCommand
{
public:
    RemovePlaylistItemCommand(MainWindow* window, Playlist& playlist, const float time, const int pattern);

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    float _time;
    int _pattern;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPLAYLISTITEMCOMMAND_H
