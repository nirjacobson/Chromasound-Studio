#ifndef SETCHANNELTYPECOMMAND_H
#define SETCHANNELTYPECOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class SetChannelTypeCommand : public QUndoCommand
{
public:
    SetChannelTypeCommand(MainWindow* window, Channel& channel, const Channel::Type type);

private:
    MainWindow* _mainWindow;
    Channel& _channel;
    Channel::Type _originalType;
    Channel::Type _newType;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETCHANNELTYPECOMMAND_H
