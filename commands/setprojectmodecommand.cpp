#include "setprojectmodecommand.h"
#include "mainwindow.h"

SetProjectModeCommand::SetProjectModeCommand(MainWindow *window, const Project::PlayMode modeBefore, const Project::PlayMode modeAfter)
    : _mainWindow(window)
    , _modeBefore(modeBefore)
    , _modeAfter(modeAfter)
{
    setText("set play mode");
}

void SetProjectModeCommand::undo()
{
    _mainWindow->app()->project().setPlayMode(_modeBefore);
    _mainWindow->doUpdate();
}

void SetProjectModeCommand::redo()
{
    _mainWindow->app()->project().setPlayMode(_modeAfter);
    _mainWindow->doUpdate();
}
