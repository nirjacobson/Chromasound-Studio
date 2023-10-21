#ifndef MOVECHANNELDOWNCOMMAND_H
#define MOVECHANNELDOWNCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class MoveChannelDownCommand : public QUndoCommand
{
public:
    MoveChannelDownCommand(MainWindow* window, const int index);

private:
    MainWindow* _mainWindow;
    int _index;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // MOVECHANNELDOWNCOMMAND_H
