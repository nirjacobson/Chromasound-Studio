#include "removetrackitemscommand.h"
#include "mainwindow.h"

RemoveTrackItemsCommand::RemoveTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& items)
    : _mainWindow(window)
    , _track(track)
    , _items(items)
{
    setText("remove notes");
}

void RemoveTrackItemsCommand::undo()
{
    _track.addItems(_items);

    _mainWindow->doUpdate();
}

void RemoveTrackItemsCommand::redo()
{
    _track.removeItems(_items);

    _mainWindow->doUpdate();
}
