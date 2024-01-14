#ifndef EDITPROJECTNOISEFREQUENCYCOMMAND_H
#define EDITPROJECTNOISEFREQUENCYCOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class EditProjectNoiseFrequencyCommand : public QUndoCommand
{
public:
    EditProjectNoiseFrequencyCommand(MainWindow* window, Project& project, const int newFreq);

private:
    MainWindow* _mainWindow;
    Project& _project;
    int _oldFreq;
    int _newFreq;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPROJECTNOISEFREQUENCYCOMMAND_H
