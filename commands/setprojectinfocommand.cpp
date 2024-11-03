#include "setprojectinfocommand.h"
#include "mainwindow.h"

SetProjectInfoCommand::SetProjectInfoCommand(MainWindow* window, Project& project, const Project::Info& info, const bool showOnOpen)
    : _mainWindow(window)
    , _project(project)
    , _oldInfo(project.info())
    , _newInfo(info)
    , _oldShowOnOpen(project.showInfoOnOpen())
    , _newShowOnOpen(showOnOpen)
{
    setText("set project info");
}

void SetProjectInfoCommand::undo()
{
    _project.info() = _oldInfo;
    _project.showInfoOnOpen(_oldShowOnOpen);

    _mainWindow->doUpdate();
}

void SetProjectInfoCommand::redo()
{
    _project.info() = _newInfo;
    _project.showInfoOnOpen(_newShowOnOpen);

    _mainWindow->doUpdate();
}
