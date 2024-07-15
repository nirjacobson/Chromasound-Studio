#include "setprojectspcmfilecommand.h"
#include "mainwindow.h"
#include "application.h"

SetProjectSPCMFileCommand::SetProjectSPCMFileCommand(MainWindow* window, Project& project, const QString& path)
    : _mainWindow(window)
    , _project(project)
    , _oldPath(project.spcmFile())
    , _newPath(path)
{

}

void SetProjectSPCMFileCommand::undo()
{
    _project.setSPCMFile(_oldPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}

void SetProjectSPCMFileCommand::redo()
{
    _project.setSPCMFile(_newPath);

    _mainWindow->app()->setupChromasound();

    _mainWindow->doUpdate();
}
