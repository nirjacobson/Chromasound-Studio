#include "editplaylistlfochangecommand.h"
#include "mainwindow.h"

EditPlaylistLFOChangeCommand::EditPlaylistLFOChangeCommand(MainWindow* window, Playlist::LFOChange* change, const int newMode)
    : _mainWindow(window)
    , _change(change)
    , _oldMode(change->mode())
    , _newMode(newMode)
{
    setText("edit LFO change");
}

void EditPlaylistLFOChangeCommand::undo()
{
    _change->setMode(_oldMode);

    _mainWindow->doUpdate();
}

void EditPlaylistLFOChangeCommand::redo()
{
    _change->setMode(_newMode);

    _mainWindow->doUpdate();
}
