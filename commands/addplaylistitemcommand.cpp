#include "addplaylistitemcommand.h"
#include "mainwindow.h"

AddPlaylistItemCommand::AddPlaylistItemCommand(MainWindow* window, Playlist& playlist, const float time, const int pattern)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _pattern(pattern)
{
    setText("add playlist item");
}

void AddPlaylistItemCommand::undo()
{
    _playlist.removeItem(_time, _pattern);

    _mainWindow->doUpdate();
}

void AddPlaylistItemCommand::redo()
{
    _playlist.addItem(_time, _pattern);

    _mainWindow->doUpdate();
}
