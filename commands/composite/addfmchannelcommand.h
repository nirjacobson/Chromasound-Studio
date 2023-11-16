#ifndef ADDFMCHANNELCOMMAND_H
#define ADDFMCHANNELCOMMAND_H

#include <QUndoCommand>

#include "commands/addchannelcommand.h"
#include "commands/composite/setfmchannelcommand.h"

class AddFMChannelCommand : public QUndoCommand
{
public:
    AddFMChannelCommand(MainWindow* window, const FMChannelSettings& settingsAfter, const QString& name);

private:
    MainWindow* _mainWindow;
    FMChannelSettings _settingsAfter;
    QString _name;

    AddChannelCommand* _addChannelCommand;
    SetFMChannelCommand* _setFMChannelCommand;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDFMCHANNELCOMMAND_H
