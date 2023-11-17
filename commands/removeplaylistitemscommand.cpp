#include "removeplaylistitemscommand.h"
#include "mainwindow.h"

RemovePlaylistItemsCommand::RemovePlaylistItemsCommand(MainWindow* window, Playlist& playlist, const QList<Playlist::Item*>& items)
    : _mainWindow(window)
    , _playlist(playlist)
    , _items(items)
{
    setText("delete playlist items");
}

void RemovePlaylistItemsCommand::undo()
{
    _playlist.addItems(_items);

    _mainWindow->doUpdate();
}

void RemovePlaylistItemsCommand::redo()
{
    _playlist.removeItems(_items);

    _mainWindow->doUpdate();
}
