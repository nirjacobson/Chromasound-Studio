#ifndef EDITPROJECTLFOMODECOMMAND_H
#define EDITPROJECTLFOMODECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class EditProjectLFOModeCommand : public QUndoCommand
{
    public:
        EditProjectLFOModeCommand(MainWindow* window, Project& project, const int newMode);

    private:
        MainWindow* _mainWindow;
        Project& _project;
        int _oldMode;
        int _newMode;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // EDITPROJECTLFOMODECOMMAND_H
