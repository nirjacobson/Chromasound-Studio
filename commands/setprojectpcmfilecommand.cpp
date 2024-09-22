#include "setprojectpcmfilecommand.h"
#include "mainwindow.h"
#include "application.h"

SetProjectPCMFileCommand::SetProjectPCMFileCommand(MainWindow* window, Project& project, const QString& path)
    : _mainWindow(window)
    , _project(project)
    , _oldPath(project.pcmFile())
    , _newPath(path)
{

}

void SetProjectPCMFileCommand::undo()
{
    _project.setPCMFile(_oldPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}

void SetProjectPCMFileCommand::redo()
{
    _project.setPCMFile(_newPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}
