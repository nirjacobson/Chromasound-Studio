#ifndef MOVECHANNELUPCOMMAND_H
#define MOVECHANNELUPCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class MoveChannelUpCommand : public QUndoCommand
{
public:
    MoveChannelUpCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // MOVECHANNELUPCOMMAND_H
