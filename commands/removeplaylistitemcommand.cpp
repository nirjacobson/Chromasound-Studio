#include "removeplaylistitemcommand.h"
#include "mainwindow.h"

RemovePlaylistItemCommand::RemovePlaylistItemCommand(MainWindow* window, Playlist& playlist, const float time, const int pattern)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _pattern(pattern)
{
    setText("delete playlist item");
}

void RemovePlaylistItemCommand::undo()
{
    _playlist.addItem(_time, _pattern);

    _mainWindow->doUpdate();
}

void RemovePlaylistItemCommand::redo()
{
    _playlist.removeItem(_time, _pattern);

    _mainWindow->doUpdate();
}
