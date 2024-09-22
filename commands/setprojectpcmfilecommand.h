#ifndef SETPROJECTPCMFILECOMMAND_H
#define SETPROJECTPCMFILECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectPCMFileCommand : public QUndoCommand
{
public:
    SetProjectPCMFileCommand(MainWindow* window, Project& project, const QString& path);

private:
    MainWindow* _mainWindow;
    Project& _project;
    QString _oldPath;
    QString _newPath;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPROJECTPCMFILECOMMAND_H
