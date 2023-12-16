#ifndef SETCHANNELNAMECOMMAND_H
#define SETCHANNELNAMECOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class SetChannelNameCommand : public QUndoCommand
{
    public:
        SetChannelNameCommand(MainWindow* window, Channel& channel, const QString& name);

    private:
        MainWindow* _mainWindow;
        Channel& _channel;
        QString _nameBefore;
        QString _nameAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // SETCHANNELNAMECOMMAND_H
