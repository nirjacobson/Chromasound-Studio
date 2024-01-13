#include "editplaylistenvelopefrequencychangecommand.h"
#include "mainwindow.h"

EditPlaylistEnvelopeFrequencyChangeCommand::EditPlaylistEnvelopeFrequencyChangeCommand(MainWindow* window, Playlist::EnvelopeFrequencyChange* change, const int newFreq)
    : _mainWindow(window)
    , _change(change)
    , _oldFreq(change->frequency())
    , _newFreq(newFreq)
{
    setText("edit envelope frequency change");
}

void EditPlaylistEnvelopeFrequencyChangeCommand::undo()
{
    _change->setFrequency(_oldFreq);

    _mainWindow->doUpdate();
}

void EditPlaylistEnvelopeFrequencyChangeCommand::redo()
{
    _change->setFrequency(_newFreq);

    _mainWindow->doUpdate();
}
