#include "removeplaylistlfochangecommand.h"
#include "mainwindow.h"

RemovePlaylistLFOChangeCommand::RemovePlaylistLFOChangeCommand(MainWindow* window, Playlist& playlist, Playlist::LFOChange* change)
    : _mainWindow(window)
    , _playlist(playlist)
    , _change(change)
    , _performed(false)
{
    setText("remove LFO change");
}

RemovePlaylistLFOChangeCommand::~RemovePlaylistLFOChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePlaylistLFOChangeCommand::undo()
{
    _playlist.addLFOChange(_change->time(), _change->mode());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePlaylistLFOChangeCommand::redo()
{
    _playlist.removeLFOChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
