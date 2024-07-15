#ifndef SETPCMCHANNELCOMMAND_H
#define SETPCMCHANNELCOMMAND_H

#include <QUndoCommand>

#include "commands/setchanneltypecommand.h"
#include "commands/setchannelnamecommand.h"
#include "commands/setromchannelsettingscommand.h"

class SetPCMChannelCommand : public QUndoCommand
{
    public:
        SetPCMChannelCommand(MainWindow* window, Channel& channel, ROMChannelSettings settingsAfter, const QString& name = "");
        ~SetPCMChannelCommand();

    private:
        MainWindow* _mainWindow;
        Channel& _channel;
        ROMChannelSettings _settingsAfter;

        SetChannelTypeCommand* _setChannelTypeCommand;
        SetROMChannelSettingsCommand* _setROMChannelSettingsCommand;
        SetChannelNameCommand* _setChannelNameCommand;

        QString _name;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // SETPCMCHANNELCOMMAND_H
