#include "movepatternupcommand.h"
#include "mainwindow.h"

MovePatternUpCommand::MovePatternUpCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("move pattern up");
}

void MovePatternUpCommand::undo()
{
    _mainWindow->movePatternDown(_index - 1);
}

void MovePatternUpCommand::redo()
{
    _mainWindow->movePatternUp(_index);
}
