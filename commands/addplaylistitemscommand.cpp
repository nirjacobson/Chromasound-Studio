#include "addplaylistitemscommand.h"
#include "mainwindow.h"

AddPlaylistItemsCommand::AddPlaylistItemsCommand(MainWindow* window, Playlist& playlist, const float time, const QList<Playlist::Item*>& items)
    : _mainWindow(window)
    , _playlist(playlist)
    , _time(time)
    , _items(items)
{

}

void AddPlaylistItemsCommand::undo()
{
    _playlist.removeItems(_items);

    _mainWindow->doUpdate();
}

void AddPlaylistItemsCommand::redo()
{
    float earliestTime = _items[0]->time();
    for (Playlist::Item* item : _items) {
        if (item->time() < earliestTime) {
            earliestTime = item->time();
        }
    }

    for (Playlist::Item* item : _items) {
        item->setTime(item->time() - earliestTime + _time);
    }

    _playlist.addItems(_items);

    _mainWindow->doUpdate();

}
