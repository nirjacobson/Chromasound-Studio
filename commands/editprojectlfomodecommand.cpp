#include "editprojectlfomodecommand.h"
#include "mainwindow.h"

EditProjectLFOModeCommand::EditProjectLFOModeCommand(MainWindow* window, Project& project, const int newMode)
    : _mainWindow(window)
    , _project(project)
    , _oldMode(project.lfoMode())
    , _newMode(newMode)
{
    setText("edit LFO mode");
}

void EditProjectLFOModeCommand::undo()
{
    _project.setLFOMode(_oldMode);

    _mainWindow->doUpdate();
}

void EditProjectLFOModeCommand::redo()
{
    _project.setLFOMode(_newMode);

    _mainWindow->doUpdate();
}
