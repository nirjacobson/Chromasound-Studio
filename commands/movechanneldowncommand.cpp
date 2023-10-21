#include "movechanneldowncommand.h"

#include "mainwindow.h"

MoveChannelDownCommand::MoveChannelDownCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("move channel down");
}

void MoveChannelDownCommand::undo()
{
    _mainWindow->moveChannelUp(_index + 1);
}

void MoveChannelDownCommand::redo()
{
    _mainWindow->moveChannelDown(_index);
}
