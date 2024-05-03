#include "duplicatepatterncommand.h"
#include "mainwindow.h"

DuplicatePatternCommand::DuplicatePatternCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
{

}

void DuplicatePatternCommand::undo()
{
    _mainWindow->deletePattern(_index + 1);
}

void DuplicatePatternCommand::redo()
{
    Pattern* pattern = _mainWindow->app()->project().getPattern(_index).copy();
    _mainWindow->insertPattern(_index + 1, pattern);
}
