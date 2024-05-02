#include "movepatterndowncommand.h"
#include "mainwindow.h"

MovePatternDownCommand::MovePatternDownCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("move pattern down");
}

void MovePatternDownCommand::undo()
{
    _mainWindow->movePatternUp(_index + 1);
}

void MovePatternDownCommand::redo()
{
    _mainWindow->movePatternDown(_index);
}
