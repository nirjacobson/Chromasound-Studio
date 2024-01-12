#include "removeplaylistenvelopeshapechangecommand.h"
#include "mainwindow.h"

RemovePlaylistEnvelopeShapeChangeCommand::RemovePlaylistEnvelopeShapeChangeCommand(MainWindow* window, Playlist& playlist, Playlist::EnvelopeShapeChange* change)
    : _mainWindow(window)
    , _playlist(playlist)
    , _change(change)
    , _performed(false)
{
    setText("remove envelope shape change");
}

RemovePlaylistEnvelopeShapeChangeCommand::~RemovePlaylistEnvelopeShapeChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePlaylistEnvelopeShapeChangeCommand::undo()
{
    _playlist.addEnvelopeShapeChange(_change->time(), _change->shape());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePlaylistEnvelopeShapeChangeCommand::redo()
{
    _playlist.removeEnvelopeShapeChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
