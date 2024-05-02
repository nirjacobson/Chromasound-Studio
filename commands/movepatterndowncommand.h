#ifndef MOVEPATTERNDOWNCOMMAND_H
#define MOVEPATTERNDOWNCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class MovePatternDownCommand : public QUndoCommand
{
public:
    MovePatternDownCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // MOVEPATTERNDOWNCOMMAND_H
