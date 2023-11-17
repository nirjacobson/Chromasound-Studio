#include "removetracksettingschangecommand.h"
#include "mainwindow.h"

RemoveTrackSettingsChangeCommand::RemoveTrackSettingsChangeCommand(MainWindow* window, Track& track, Track::SettingsChange* change)
    : _mainWindow(window)
    , _track(track)
    , _change(change)
    , _performed(false)
{
    setText("remove track settings change");
}

RemoveTrackSettingsChangeCommand::~RemoveTrackSettingsChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemoveTrackSettingsChangeCommand::undo()
{
    _track.addSettingsChange(_change->time(), _change->name(), &_change->settings());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemoveTrackSettingsChangeCommand::redo()
{
    _track.removeSettingsChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
