#ifndef SETPROJECTDPCMFILECOMMAND_H
#define SETPROJECTDPCMFILECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectDPCMFileCommand : public QUndoCommand
{
public:
    SetProjectDPCMFileCommand(MainWindow* window, Project& project, const QString& path);

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

#endif // SETPROJECTDPCMFILECOMMAND_H
