#ifndef ADDPCMCHANNELCOMMAND_H
#define ADDPCMCHANNELCOMMAND_H

#include <QUndoCommand>

#include "commands/addchannelcommand.h"
#include "commands/composite/setpcmchannelcommand.h"

class AddPCMChannelCommand : public QUndoCommand
{
    public:
        AddPCMChannelCommand(MainWindow* window, const ROMChannelSettings& settingsAfter, const QString& name);
        ~AddPCMChannelCommand();

    private:
        MainWindow* _mainWindow;
        ROMChannelSettings _settingsAfter;
        QString _name;

        AddChannelCommand* _addChannelCommand;
        SetPCMChannelCommand* _setPCMChannelCommand;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // ADDPCMCHANNELCOMMAND_H
