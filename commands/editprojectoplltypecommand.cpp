#include "editprojectoplltypecommand.h"
#include "mainwindow.h"

EditProjectOPLLTypeCommand::EditProjectOPLLTypeCommand(MainWindow* window, Project& project, const OPLL::Type newType)
    : _mainWindow(window)
    , _project(project)
    , _oldType(project.opllType())
    , _newType(newType)
{
    setText("edit melody implementation");
}

void EditProjectOPLLTypeCommand::undo()
{
    _project.setOpllType(_oldType);

    try {
        FM_PSG_Soft& fmPsgSoft = dynamic_cast<FM_PSG_Soft&>(_mainWindow->app()->fmPSG());
        fmPsgSoft.setOPLLPatchset(_oldType);
    } catch (std::bad_cast&) { }

    _mainWindow->doUpdate();
}

void EditProjectOPLLTypeCommand::redo()
{
    _project.setOpllType(_newType);

    try {
        FM_PSG_Soft& fmPsgSoft = dynamic_cast<FM_PSG_Soft&>(_mainWindow->app()->fmPSG());
        fmPsgSoft.setOPLLPatchset(_newType);
    } catch (std::bad_cast&) { }

    _mainWindow->doUpdate();
}
