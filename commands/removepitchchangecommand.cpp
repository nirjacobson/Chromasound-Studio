#include "removepitchchangecommand.h"
#include "mainwindow.h"

RemovePitchChangeCommand::RemovePitchChangeCommand(MainWindow *window, Track &track, Track::PitchChange *change)
    : _mainWindow(window)
    , _track(track)
    , _change(change)
    , _performed(false)
{
    setText("remove pitch change");
}

RemovePitchChangeCommand::~RemovePitchChangeCommand()
{
    if (_performed) {
        delete _change;
    }
}

void RemovePitchChangeCommand::undo()
{
    _track.addPitchChange(_change->time(), _change->pitch());

    _mainWindow->doUpdate();

    _performed = false;
}

void RemovePitchChangeCommand::redo()
{
    _track.removePitchChange(_change, true);

    _mainWindow->doUpdate();

    _performed = true;
}
