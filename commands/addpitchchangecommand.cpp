#include "addpitchchangecommand.h"
#include "mainwindow.h"

AddPitchChangeCommand::AddPitchChangeCommand(MainWindow *window, Track &track, const float time, const float pitch)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _pitch(pitch)
    , _change(nullptr)
{
    setText("add pitch change");
}

void AddPitchChangeCommand::undo()
{
    _track.removePitchChange(_change);

    _mainWindow->doUpdate();
}

void AddPitchChangeCommand::redo()
{
    _change = _track.addPitchChange(_time, _pitch);

    _mainWindow->doUpdate();
}
