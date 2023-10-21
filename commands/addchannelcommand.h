#ifndef ADDCHANNELCOMMAND_H
#define ADDCHANNELCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class AddChannelCommand : public QUndoCommand
{
public:
    AddChannelCommand(MainWindow* window);
private:
    MainWindow* _mainWindow;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDCHANNELCOMMAND_H
