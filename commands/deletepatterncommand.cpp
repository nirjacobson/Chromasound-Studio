#include "deletepatterncommand.h"
#include "mainwindow.h"

DeletePatternCommand::DeletePatternCommand(MainWindow* window, const int index)
    : _mainWindow(window)
    , _index(index)
    , _pattern(nullptr)
{
    setText("delete pattern");
}

DeletePatternCommand::~DeletePatternCommand()
{
    if (_pattern) {
        delete _pattern;
        for (Playlist::Item* item : _items) {
            delete item;
        }
    }
}

void DeletePatternCommand::undo()
{
    _mainWindow->insertPattern(_index, _pattern, _items);
    _pattern = nullptr;
}

void DeletePatternCommand::redo()
{
    _pattern = _mainWindow->app()->project().getPattern(_index).copy();
    _items = _mainWindow->deletePattern(_index);
}
