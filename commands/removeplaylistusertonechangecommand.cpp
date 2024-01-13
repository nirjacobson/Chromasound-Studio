#include "removeplaylistusertonechangecommand.h"
#include "mainwindow.h"

RemovePlaylistUserToneChangeCommand::RemovePlaylistUserToneChangeCommand(MainWindow* window, Playlist& playlist, Playlist::UserToneChange* change)
    : _mainWindow(window)
    , _playlist(playlist)
    , _change(change)
    , _performed(false)
{
    setText("remove user tone change");
}

RemovePlaylistUserToneChangeCommand::~RemovePlaylistUserToneChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePlaylistUserToneChangeCommand::undo()
{
    _playlist.addUserToneChange(_change->time(), _change->userTone());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePlaylistUserToneChangeCommand::redo()
{
    _playlist.removeUserToneChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
