#include "addplaylistenvelopefrequencychangecommand.h"
#include "mainwindow.h"

AddPlaylistEnvelopeFrequencyChangeCommand::AddPlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int freq)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _freq(freq)
{
    setText("add envelope frequency change");
}

void AddPlaylistEnvelopeFrequencyChangeCommand::undo()
{
    _playlist.removeEnvelopeFrequencyChange(_change);

    _mainWindow->doUpdate();
}

void AddPlaylistEnvelopeFrequencyChangeCommand::redo()
{
    _change = _playlist.addEnvelopeFrequencyChange(_time, _freq);

    _mainWindow->doUpdate();
}
