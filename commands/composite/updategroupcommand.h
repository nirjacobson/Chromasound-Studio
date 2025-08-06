#ifndef UPDATEGROUPCOMMAND_H
#define UPDATEGROUPCOMMAND_H

#include <QUndoCommand>

class MainWindow;
class UpdateGroupCommand : public QUndoCommand
{
public:
    UpdateGroupCommand(MainWindow* window, QList<QUndoCommand *> commands);

private:
    MainWindow* _mainWindow;
    QList<QUndoCommand*> _commands;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // UPDATEGROUPCOMMAND_H
