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
        Chromasound_Emu& emu = dynamic_cast<Chromasound_Emu&>(_mainWindow->app()->chromasound());
        emu.setOPLLPatchset(_oldType);
    } catch (std::bad_cast) {
        try {
            Chromasound_Dual& dual = dynamic_cast<Chromasound_Dual&>(_mainWindow->app()->chromasound());
            dual.setOPLLPatchset(_oldType);
        } catch (std::bad_cast) { }
    }

    _mainWindow->doUpdate();
}

void EditProjectOPLLTypeCommand::redo()
{
    _project.setOpllType(_newType);

    try {
        Chromasound_Emu& emu = dynamic_cast<Chromasound_Emu&>(_mainWindow->app()->chromasound());
        emu.setOPLLPatchset(_newType);
    } catch (std::bad_cast) {
        try {
            Chromasound_Dual& dual = dynamic_cast<Chromasound_Dual&>(_mainWindow->app()->chromasound());
            dual.setOPLLPatchset(_newType);
        } catch (std::bad_cast) { }
    }

    _mainWindow->doUpdate();
}
