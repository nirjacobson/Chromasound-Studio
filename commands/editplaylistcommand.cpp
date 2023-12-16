#include "editplaylistcommand.h"
#include "mainwindow.h"

EditPlaylistCommand::EditPlaylistCommand(MainWindow* window, Playlist::Item* item, const float toTime, const QList<Playlist::Item*>& group)
    : _mainWindow(window)
    , _item(item)
    , _fromTime(item->time())
    , _toTime(toTime)
    , _group(group)
{
    setText("edit playlist");
}

void EditPlaylistCommand::undo()
{
    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->undo();
    }

    _item->setTime(_fromTime);

    _mainWindow->doUpdate();
}

void EditPlaylistCommand::redo()
{
    _item->setTime(_toTime);

    for (auto it = _groupCommands.begin(); it != _groupCommands.end(); ++it) {
        (*it)->redo();
    }

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
            _toTime = epc->_toTime;
            return true;
        } else {
            if (_group.contains(epc->_item)) {
                if (_groupCommands.contains(epc->_item)) {
                    _groupCommands[epc->_item]->mergeWith(other);
                } else {
                    EditPlaylistCommand* epc2 = new EditPlaylistCommand(
                        epc->_mainWindow,
                        epc->_item,
                        epc->_toTime,
                        epc->_group
                    );
                    epc2->_fromTime = epc->_fromTime;

                    _groupCommands.insert(epc->_item, epc2);
                }
                return true;
            }
        }
    }

    return false;
}
