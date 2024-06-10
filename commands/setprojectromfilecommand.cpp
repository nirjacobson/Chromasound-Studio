#include "setprojectromfilecommand.h"
#include "mainwindow.h"
#include "application.h"

SetProjectROMFileCommand::SetProjectROMFileCommand(MainWindow* window, Project& project, const QString& path)
    : _mainWindow(window)
    , _project(project)
    , _oldPath(project.romFile())
    , _newPath(path)
{

}

void SetProjectROMFileCommand::undo()
{
    _project.setROMFile(_oldPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}

void SetProjectROMFileCommand::redo()
{
    _project.setROMFile(_newPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}
