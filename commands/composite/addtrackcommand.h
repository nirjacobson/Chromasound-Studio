#ifndef ADDTRACKCOMMAND_H
#define ADDTRACKCOMMAND_H

#include <QUndoCommand>

#include "commands/addchannelcommand.h"
#include "commands/addtrackitemscommand.h"
#include "commands/setchannelnamecommand.h"

class MainWindow;

class AddTrackCommand : public QUndoCommand
{
    public:
        AddTrackCommand(MainWindow* window, const QList<Track::Item*>& items, const QString& name);
        ~AddTrackCommand();

    private:
        MainWindow* _mainWindow;
        QList<Track::Item*> _items;
        QString _name;

        AddChannelCommand* _addChannelCommand;
        SetChannelNameCommand* _setChannelNameCommand;
        AddTrackItemsCommand* _addTrackItemsCommand;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // ADDTRACKCOMMAND_H
