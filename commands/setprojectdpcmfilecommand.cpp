#include "setprojectdpcmfilecommand.h"
#include "mainwindow.h"
#include "application.h"

SetProjectDPCMFileCommand::SetProjectDPCMFileCommand(MainWindow* window, Project& project, const QString& path)
    : _mainWindow(window)
    , _project(project)
    , _oldPath(project.dpcmFile())
    , _newPath(path)
{

}

void SetProjectDPCMFileCommand::undo()
{
    _project.setDPCMFile(_oldPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}

void SetProjectDPCMFileCommand::redo()
{
    _project.setDPCMFile(_newPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}
