#include "removeplaylistnoisefrequencychangecommand.h"
#include "mainwindow.h"

RemovePlaylistNoiseFrequencyChangeCommand::RemovePlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist& playlist, Playlist::NoiseFrequencyChange* change)
    : _mainWindow(window)
    , _playlist(playlist)
    , _change(change)
    , _performed(false)
{
    setText("remove noise frequency change");
}

RemovePlaylistNoiseFrequencyChangeCommand::~RemovePlaylistNoiseFrequencyChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePlaylistNoiseFrequencyChangeCommand::undo()
{
    _playlist.addNoiseFrequencyChange(_change->time(), _change->frequency());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePlaylistNoiseFrequencyChangeCommand::redo()
{
    _playlist.removeNoiseFrequencyChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
