#include "setprojecttempocommand.h"
#include "mainwindow.h"

SetProjectTempoCommand::SetProjectTempoCommand(MainWindow *window, const int tempoBefore, const int tempoAfter)
    : _mainWindow(window)
    , _tempoBefore(tempoBefore)
    , _tempoAfter(tempoAfter)
{
    setText("set tempo");
}

void SetProjectTempoCommand::undo()
{
    _mainWindow->app()->project().setTempo(_tempoBefore);
    _mainWindow->doUpdate();
}

void SetProjectTempoCommand::redo()
{
    _mainWindow->app()->project().setTempo(_tempoAfter);
    _mainWindow->doUpdate();
}
