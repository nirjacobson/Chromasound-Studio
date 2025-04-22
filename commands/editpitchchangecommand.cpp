#include "editpitchchangecommand.h"
#include "mainwindow.h"

EditPitchChangeCommand::EditPitchChangeCommand(MainWindow *window, Track::PitchChange *change, const float toTime, const float toPitch, const QList<Track::PitchChange *> &group)
    : _mainWindow(window)
    , _change(change)
    , _fromTime(change->time())
    , _fromPitch(change->pitch())
    , _toTime(toTime)
    , _toPitch(toPitch)
    , _group(group)
    , _performed(false)
{
    setText("edit pitch change");
}

void EditPitchChangeCommand::undo()
{
    undoNoUpdate();

    _mainWindow->doUpdate();
}

void EditPitchChangeCommand::redo()
{
    redoNoUpdate();

    if (_performed) {
        _mainWindow->doUpdate();
    }

    _performed = true;
}

void EditPitchChangeCommand::undoNoUpdate()
{
    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->undoNoUpdate();
    }

    *_change = Track::PitchChange(_fromTime, _fromPitch);
}

void EditPitchChangeCommand::redoNoUpdate()
{
    *_change = Track::PitchChange(_toTime, _toPitch);

    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->redoNoUpdate();
    }
}

int EditPitchChangeCommand::id() const
{
    return 'H';
}

bool EditPitchChangeCommand::mergeWith(const QUndoCommand *other)
{
    const EditPitchChangeCommand* epcc;
    if ((epcc = dynamic_cast<const EditPitchChangeCommand*>(other))) {
        if (_change == epcc->_change) {
            _toTime = epcc->_toTime;
            _toPitch = epcc->_toPitch;
            return true;
        } else {
            if (_group.contains(epcc->_change)) {
                if (_groupCommands.contains(epcc->_change)) {
                    _groupCommands[epcc->_change]->mergeWith(other);
                } else {
                    EditPitchChangeCommand* epcc2 = new EditPitchChangeCommand(
                        epcc->_mainWindow,
                        epcc->_change,
                        epcc->_toTime,
                        epcc->_toPitch,
                        epcc->_group);
                    epcc2->_fromTime = epcc->_fromTime;
                    epcc2->_fromPitch = epcc->_fromPitch;

                    _groupCommands.insert(epcc->_change, epcc2);
                }
                return true;
            }
        }
    }

    return false;
}
