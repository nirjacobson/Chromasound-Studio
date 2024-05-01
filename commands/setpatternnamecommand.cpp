#include "setpatternnamecommand.h"
#include "mainwindow.h"

SetPatternNameCommand::SetPatternNameCommand(MainWindow* window, Pattern& pattern, const QString& name)
    : _mainWindow(window)
    , _pattern(pattern)
    , _nameBefore(pattern.name())
    , _nameAfter(name)
{
    setText("set pattern name");
}

void SetPatternNameCommand::undo()
{
    _pattern.setName(_nameBefore);
    _mainWindow->doUpdate();
}

void SetPatternNameCommand::redo()
{
    _pattern.setName(_nameAfter);
    _mainWindow->doUpdate();
}
