#ifndef EDITPROJECTENVELOPEFREQUENCYCOMMAND_H
#define EDITPROJECTENVELOPEFREQUENCYCOMMAND_H

#include <QUndoCommand>

#include "project/project.h"

class MainWindow;

class EditProjectEnvelopeFrequencyCommand : public QUndoCommand
{
    public:
        EditProjectEnvelopeFrequencyCommand(MainWindow* window, Project& project, const int newFreq);

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

#endif // EDITPROJECTENVELOPEFREQUENCYCOMMAND_H
