#include "addtracksettingschangecommand.h"
#include "mainwindow.h"

AddTrackSettingsChangeCommand::AddTrackSettingsChangeCommand(MainWindow* window, Track& track, const float time, ChannelSettings* settings)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _settings(settings)
{

}

void AddTrackSettingsChangeCommand::undo()
{
    _track.removeSettingsChange(_change);

    _mainWindow->doUpdate();
}

void AddTrackSettingsChangeCommand::redo()
{
    _change = _track.addSettingsChange(_time, "Settings change", _settings);

    _mainWindow->doUpdate();
}
