#ifndef SETFMCHANNELCOMMAND_H
#define SETFMCHANNELCOMMAND_H

#include <QUndoCommand>

#include "commands/setchanneltypecommand.h"
#include "commands/setfmchannelsettingscommand.h"
#include "commands/setchannelnamecommand.h"

class MainWindow;

class SetFMChannelCommand : public QUndoCommand
{
    public:
        SetFMChannelCommand(MainWindow* window, Channel& channel, const FMChannelSettings& settingsAfter, const QString& name = "");
        ~SetFMChannelCommand();

    private:
        MainWindow* _mainWindow;
        Channel& _channel;
        FMChannelSettings _settingsAfter;

        SetChannelTypeCommand* _setChannelTypeCommand;
        SetFMChannelSettingsCommand* _setFMChannelSettingsCommand;
        SetChannelNameCommand* _setChannelNameCommand;

        QString _name;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // SETFMCHANNELCOMMAND_H
