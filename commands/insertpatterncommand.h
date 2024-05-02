#ifndef INSERTPATTERNCOMMAND_H
#define INSERTPATTERNCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class InsertPatternCommand : public QUndoCommand
{
public:
    InsertPatternCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // INSERTPATTERNCOMMAND_H
