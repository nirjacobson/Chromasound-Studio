#include "editnotecommand.h"
#include "mainwindow.h"

EditNoteCommand::EditNoteCommand(MainWindow* window, Track::Item* item, const float toTime, const Note& note, const QList<Track::Item*>& group)
    : _mainWindow(window)
    , _group(group)
    , _toTime(toTime)
    , _note(note)
    , _item(item)
    , _fromTime(_item->time())
    , _fromNote(_item->note())
    , _performed(false)
{
    setText("edit note");
}

void EditNoteCommand::undo()
{
    undoNoUpdate();

    _mainWindow->doUpdate();
}

void EditNoteCommand::redo()
{
    redoNoUpdate();

    if (_performed) {
        _mainWindow->doUpdate();
    }

    _performed = true;
}

void EditNoteCommand::undoNoUpdate()
{
    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->undoNoUpdate();
    }

    _item->setTime(_fromTime);
    _item->note() = _fromNote;
}

void EditNoteCommand::redoNoUpdate()
{
    _item->setTime(_toTime);
    _item->note() = _note;

    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->redoNoUpdate();
    }
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
        } else {
            if (_group.contains(enc->_item)) {
                if (_groupCommands.contains(enc->_item)) {
                    _groupCommands[enc->_item]->mergeWith(other);
                } else {
                    EditNoteCommand* enc2 = new EditNoteCommand(
                        enc->_mainWindow,
                        enc->_item,
                        enc->_toTime,
                        enc->_note,
                        enc->_group);
                    enc2->_fromTime = enc->_fromTime;
                    enc2->_fromNote = enc->_fromNote;

                    _groupCommands.insert(enc->_item, enc2);
                }
                return true;
            }
        }
    }

    return false;
}
