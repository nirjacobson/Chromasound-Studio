#ifndef EDITPROJECTOPLLTYPECOMMAND_H
#define EDITPROJECTOPLLTYPECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"
#include "opl/opll.h"

class MainWindow;

class EditProjectOPLLTypeCommand : public QUndoCommand
{
public:
    EditProjectOPLLTypeCommand(MainWindow* window, Project& project, const OPLL::Type newType);

private:
    MainWindow* _mainWindow;
    Project& _project;
    OPLL::Type _oldType;
    OPLL::Type _newType;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPROJECTOPLLTYPECOMMAND_H
