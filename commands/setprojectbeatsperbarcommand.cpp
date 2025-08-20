#include "setprojectbeatsperbarcommand.h"
#include "mainwindow.h"

SetProjectBeatsPerBarCommand::SetProjectBeatsPerBarCommand(MainWindow *window, const int beatsBefore, const int beatsAfter)
    : _mainWindow(window)
    , _beatsBefore(beatsBefore)
    , _beatsAfter(beatsAfter)
{
    setText("set beats per bar");
}

void SetProjectBeatsPerBarCommand::undo()
{
    _mainWindow->app()->project().setBeatsPerBar(_beatsBefore);
    _mainWindow->doUpdate();
}

void SetProjectBeatsPerBarCommand::redo()
{
    _mainWindow->app()->project().setBeatsPerBar(_beatsAfter);
    _mainWindow->doUpdate();
}
