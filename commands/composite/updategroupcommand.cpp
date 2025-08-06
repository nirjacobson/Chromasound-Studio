#include "updategroupcommand.h"
#include "mainwindow.h"

UpdateGroupCommand::UpdateGroupCommand(MainWindow *window, QList<QUndoCommand*> commands)
    : _mainWindow(window)
    , _commands(commands)
{

}

void UpdateGroupCommand::undo()
{
    for (QUndoCommand* command : _commands) {
        command->undo();
    }

    _mainWindow->doUpdate();
}

void UpdateGroupCommand::redo()
{
    for (QUndoCommand* command : _commands) {
        command->redo();
    }

    _mainWindow->doUpdate();
}
