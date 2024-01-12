#include "removeplaylistenvelopefrequencychangecommand.h"
#include "mainwindow.h"

RemovePlaylistEnvelopeFrequencyChangeCommand::RemovePlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist& playlist, Playlist::EnvelopeFrequencyChange* change)
    : _mainWindow(window)
    , _playlist(playlist)
    , _change(change)
    , _performed(false)
{
    setText("remove envelope frequency change");
}

RemovePlaylistEnvelopeFrequencyChangeCommand::~RemovePlaylistEnvelopeFrequencyChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePlaylistEnvelopeFrequencyChangeCommand::undo()
{
    _playlist.addEnvelopeFrequencyChange(_change->time(), _change->frequency());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePlaylistEnvelopeFrequencyChangeCommand::redo()
{
    _playlist.removeEnvelopeFrequencyChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
