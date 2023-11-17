#include "addplaylistlfochangecommand.h"
#include "mainwindow.h"

AddPlaylistLFOChangeCommand::AddPlaylistLFOChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int mode)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _mode(mode)
{
    setText("add LFO change");
}

void AddPlaylistLFOChangeCommand::undo()
{
    _playlist.removeLFOChange(_change);

    _mainWindow->doUpdate();
}

void AddPlaylistLFOChangeCommand::redo()
{
    _change = _playlist.addLFOChange(_time, _mode);

    _mainWindow->doUpdate();
}
