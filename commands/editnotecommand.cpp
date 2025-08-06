#include "editnotecommand.h"
#include "mainwindow.h"

EditNoteCommand::EditNoteCommand(MainWindow* window, Track::Item* item, const float toTime, const Note& note, const bool group)
    : _mainWindow(window)
    , _group(group)
    , _toTime(toTime)
    , _note(note)
    , _item(item)
    , _fromTime(_item->time())
    , _fromNote(_item->note())
{
    setText("edit note");
}

void EditNoteCommand::undo()
{
    undoNoUpdate();

    if (!_group) {
        _mainWindow->doUpdate();
    }
}

void EditNoteCommand::redo()
{
    redoNoUpdate();

    if (!_group) {
        _mainWindow->doUpdate();
    }
}

void EditNoteCommand::undoNoUpdate()
{
    _item->setTime(_fromTime);
    _item->note() = _fromNote;
}

void EditNoteCommand::redoNoUpdate()
{
    _item->setTime(_toTime);
    _item->note() = _note;
}

int EditNoteCommand::id() const
{
    return 'N';
}

bool EditNoteCommand::mergeWith(const QUndoCommand* other)
{
    const EditNoteCommand* enc;
    if ((enc = dynamic_cast<const EditNoteCommand*>(other))) {
        if (_item == enc->_item) {
            Note n = _note;
            n.setDuration(enc->_note.duration());
            if (enc->_note == n) {
                _toTime = enc->_toTime;
                _note = enc->_note;
                return true;
            }

            n = _note;
            n.setKey(enc->_note.key());
            if (enc->_note == n) {
                _toTime = enc->_toTime;
                _note = enc->_note;
                return true;
            }

            n = _note;
            n.setVelocity(enc->_note.velocity());
            if (enc->_note == n) {
                _toTime = enc->_toTime;
                _note = enc->_note;
                return true;
            }
        }
    }

    return false;
}
