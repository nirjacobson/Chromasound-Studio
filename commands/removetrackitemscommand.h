#ifndef REMOVETRACKITEMSCOMMAND_H
#define REMOVETRACKITEMSCOMMAND_H

#include <QUndoCommand>

#include "../project/track.h"

class MainWindow;

class RemoveTrackItemsCommand : public QUndoCommand
{
public:
    RemoveTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& items);
private:
    MainWindow* _mainWindow;

    Track& _track;
    QList<Track::Item*> _items;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVETRACKITEMSCOMMAND_H
