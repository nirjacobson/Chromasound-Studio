#ifndef ADDPLAYLISTITEMCOMMAND_H
#define ADDPLAYLISTITEMCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistItemCommand : public QUndoCommand
{
public:
    AddPlaylistItemCommand(MainWindow* window, Playlist& playlist, const float time, const int pattern);

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

#endif // ADDPLAYLISTITEMCOMMAND_H
