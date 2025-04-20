#include "editpitchchangecommand.h"
#include "mainwindow.h"

EditPitchChangeCommand::EditPitchChangeCommand(MainWindow *window, Track::PitchChange *change, const float toPitch)
    : _mainWindow(window)
    , _change(change)
    , _fromPitch(change->pitch())
    , _toPitch(toPitch)
{
    setText("Edit pitch change");
}

void EditPitchChangeCommand::undo()
{
    *_change = Track::PitchChange(_change->time(), _fromPitch);

    _mainWindow->doUpdate();
}

void EditPitchChangeCommand::redo()
{
    *_change = Track::PitchChange(_change->time(), _toPitch);

    _mainWindow->doUpdate();
}
