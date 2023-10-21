#include "editplaylistcommand.h"
#include "mainwindow.h"

EditPlaylistCommand::EditPlaylistCommand(MainWindow* window, Playlist::Item* item, const float toTime)
    : _mainWindow(window)
    , _item(item)
    , _fromTime(item->time())
    , _toTime(toTime)
{
    setText("edit playlist");
}

void EditPlaylistCommand::undo()
{
    _item->setTime(_fromTime);

    _mainWindow->doUpdate();
}

void EditPlaylistCommand::redo()
{
    _item->setTime(_toTime);

    _mainWindow->doUpdate();
}

int EditPlaylistCommand::id() const
{
    return 'P';
}

bool EditPlaylistCommand::mergeWith(const QUndoCommand* other)
{
    const EditPlaylistCommand* epc;
    if ((epc = dynamic_cast<const EditPlaylistCommand*>(other))) {
        if (_item == epc->_item) {
            return true;
        }
    }

    return false;
}
