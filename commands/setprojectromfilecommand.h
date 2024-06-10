#ifndef SETPROJECTROMFILECOMMAND_H
#define SETPROJECTROMFILECOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class SetProjectROMFileCommand : public QUndoCommand
{
public:
    SetProjectROMFileCommand(MainWindow* window, Project& project, const QString& path);

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

#endif // SETPROJECTROMFILECOMMAND_H
