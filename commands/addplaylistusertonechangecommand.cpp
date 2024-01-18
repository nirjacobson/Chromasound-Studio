#include "addplaylistusertonechangecommand.h"
#include "opl/usertone/oplsettings.h"
#include "mainwindow.h"

AddPlaylistUserToneChangeCommand::AddPlaylistUserToneChangeCommand(MainWindow* window, Playlist& playlist, const float time)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
{
    setText("add user tone change");
}

void AddPlaylistUserToneChangeCommand::undo()
{
    _playlist.removeUserToneChange(_change);

    _mainWindow->doUpdate();
}

void AddPlaylistUserToneChangeCommand::redo()
{
    _change = _playlist.addUserToneChange(_time, OPLSettings());

    _mainWindow->doUpdate();
}
