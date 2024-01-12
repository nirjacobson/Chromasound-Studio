#include "addplaylistenvelopeshapechangecommand.h"
#include "ssg/ssgenvelopesettings.h"
#include "mainwindow.h"

AddPlaylistEnvelopeShapeChangeCommand::AddPlaylistEnvelopeShapeChangeCommand(MainWindow* window, Playlist& playlist, const float time)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
{
    setText("add envelope shape change");
}

void AddPlaylistEnvelopeShapeChangeCommand::undo()
{
    _playlist.removeEnvelopeShapeChange(_change);

    _mainWindow->doUpdate();
}

void AddPlaylistEnvelopeShapeChangeCommand::redo()
{
    _change = _playlist.addEnvelopeShapeChange(_time, SSGEnvelopeSettings());

    _mainWindow->doUpdate();
}
