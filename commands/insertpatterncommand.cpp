#include "insertpatterncommand.h"
#include "mainwindow.h"

InsertPatternCommand::InsertPatternCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("insert pattern");
}

void InsertPatternCommand::undo()
{
    _mainWindow->deletePattern(_index);
}

void InsertPatternCommand::redo()
{
    _mainWindow->insertPattern(_index);
}
