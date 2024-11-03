#ifndef SETPROJECTINFOCOMMAND_H
#define SETPROJECTINFOCOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectInfoCommand : public QUndoCommand
{
public:
    SetProjectInfoCommand(MainWindow* window, Project& project, const Project::Info& info, const bool showOnOpen);

private:
    MainWindow* _mainWindow;
    Project& _project;
    Project::Info _oldInfo;
    Project::Info _newInfo;
    bool _oldShowOnOpen;
    bool _newShowOnOpen;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPROJECTINFOCOMMAND_H
