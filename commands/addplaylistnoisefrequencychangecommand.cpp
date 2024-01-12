#include "addplaylistnoisefrequencychangecommand.h"
#include "mainwindow.h"

AddPlaylistNoiseFrequencyChangeCommand::AddPlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int freq)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _freq(freq)
{
    setText("add noise frequency change");
}

void AddPlaylistNoiseFrequencyChangeCommand::undo()
{
    _playlist.removeNoiseFrequencyChange(_change);

    _mainWindow->doUpdate();
}

void AddPlaylistNoiseFrequencyChangeCommand::redo()
{
    _change = _playlist.addNoiseFrequencyChange(_time, _freq);

    _mainWindow->doUpdate();
}
