#ifndef SETPROJECTMODECOMMAND_H
#define SETPROJECTMODECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectModeCommand : public QUndoCommand
{
public:
    SetProjectModeCommand(MainWindow* window, const Project::PlayMode modeBefore, const Project::PlayMode modeAfter);

private:
    MainWindow* _mainWindow;
    Project::PlayMode _modeBefore;
    Project::PlayMode _modeAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPROJECTMODECOMMAND_H
