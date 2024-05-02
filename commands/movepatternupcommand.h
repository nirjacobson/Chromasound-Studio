#ifndef MOVEPATTERNUPCOMMAND_H
#define MOVEPATTERNUPCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class MovePatternUpCommand : public QUndoCommand
{
public:
    MovePatternUpCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // MOVEPATTERNUPCOMMAND_H
