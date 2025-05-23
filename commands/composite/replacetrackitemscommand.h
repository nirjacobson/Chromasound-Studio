#ifndef REPLACETRACKITEMSCOMMAND_H
#define REPLACETRACKITEMSCOMMAND_H

#include <QUndoCommand>

#include "project/track.h"
#include "commands/removetrackitemscommand.h"
#include "commands/addtrackitemscommand.h"

class MainWindow;

class ReplaceTrackItemsCommand : public QUndoCommand
{
    public:
        ReplaceTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& newItems);
        ReplaceTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& items, const QList<Track::Item*>& newItems);
        ~ReplaceTrackItemsCommand();

    private:
        MainWindow* _mainWindow;
        Track& _track;
        QList<Track::Item*> _items;
        QList<Track::Item*> _newItems;

        RemoveTrackItemsCommand* _removeTrackItemsCommand;
        AddTrackItemsCommand* _addTrackItemsCommand;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // REPLACETRACKITEMSCOMMAND_H
