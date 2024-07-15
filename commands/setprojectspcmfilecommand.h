#ifndef SETPROJECTSPCMFILECOMMAND_H
#define SETPROJECTSPCMFILECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectSPCMFileCommand : public QUndoCommand
{
public:
    SetProjectSPCMFileCommand(MainWindow* window, Project& project, const QString& path);

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

#endif // SETPROJECTSPCMFILECOMMAND_H
