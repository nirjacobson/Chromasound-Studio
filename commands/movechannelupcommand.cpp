#include "movechannelupcommand.h"
#include "mainwindow.h"

MoveChannelUpCommand::MoveChannelUpCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("move channel up");
}

void MoveChannelUpCommand::undo()
{
    _mainWindow->moveChannelDown(_index - 1);
}

void MoveChannelUpCommand::redo()
{
    _mainWindow->moveChannelUp(_index);
}
