#include "editnotecommand.h"
#include "mainwindow.h"

EditNoteCommand::EditNoteCommand(MainWindow* window, Track::Item* item, const float toTime, const Note& note)
    : _mainWindow(window)
    , _toTime(toTime)
    , _note(note)
    , _item(item)
    , _fromTime(_item->time())
    , _fromNote(_item->note())
    , _mergedDuration(false)
    , _mergedKey(false)
{
    setText("edit note");
}

void EditNoteCommand::undo()
{
    _item->setTime(_fromTime);
    _item->note() = _fromNote;

    _mainWindow->doUpdate();
}

void EditNoteCommand::redo()
{
    _item->setTime(_toTime);
    _item->note() = _note;

    _mainWindow->doUpdate();
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
                _mergedDuration = true;
                return true;
            }

            n = _note;
            n.setKey(enc->_note.key());
            if (enc->_note == n) {
                _toTime = enc->_toTime;
                _note = enc->_note;
                _mergedKey = true;
                return true;
            }
        }
    }

    return false;
}
