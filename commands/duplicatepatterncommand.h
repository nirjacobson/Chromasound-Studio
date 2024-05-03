#ifndef DUPLICATEPATTERNCOMMAND_H
#define DUPLICATEPATTERNCOMMAND_H

#include <QUndoCommand>

#include "project/pattern.h"
#include "project/playlist.h"

class MainWindow;

class DuplicatePatternCommand : public QUndoCommand
{
public:
    DuplicatePatternCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // DUPLICATEPATTERNCOMMAND_H
