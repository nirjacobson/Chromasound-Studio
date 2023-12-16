#ifndef DELETECHANNELCOMMAND_H
#define DELETECHANNELCOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class DeleteChannelCommand : public QUndoCommand
{
    public:
        DeleteChannelCommand(MainWindow* window, const Channel& channel, const int index);
    private:
        MainWindow* _mainWindow;
        int _index;

        Channel _deletedChannel;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // DELETECHANNELCOMMAND_H
