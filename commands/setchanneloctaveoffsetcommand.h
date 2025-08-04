#ifndef SETCHANNELOCTAVEOFFSETCOMMAND_H
#define SETCHANNELOCTAVEOFFSETCOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class SetChannelOctaveOffsetCommand : public QUndoCommand
{
public:
    SetChannelOctaveOffsetCommand(MainWindow* window, Channel& channel, const int octaveOffset);

private:
    MainWindow* _mainWindow;
    Channel& _channel;
    int _originalOctaveOffset;
    int _newOctaveOffset;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETCHANNELOCTAVEOFFSETCOMMAND_H
