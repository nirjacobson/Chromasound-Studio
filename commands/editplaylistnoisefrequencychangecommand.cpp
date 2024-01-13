#include "editplaylistnoisefrequencychangecommand.h"
#include "mainwindow.h"

EditPlaylistNoiseFrequencyChangeCommand::EditPlaylistNoiseFrequencyChangeCommand(MainWindow* window, Playlist::NoiseFrequencyChange* change, const int newFreq)
    : _mainWindow(window)
    , _change(change)
    , _oldFreq(change->frequency())
    , _newFreq(newFreq)
{
    setText("edit noise frequency change");
}

void EditPlaylistNoiseFrequencyChangeCommand::undo()
{
    _change->setFrequency(_oldFreq);

    _mainWindow->doUpdate();
}

void EditPlaylistNoiseFrequencyChangeCommand::redo()
{
    _change->setFrequency(_newFreq);

    _mainWindow->doUpdate();
}
