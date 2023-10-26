#ifndef REMOVEPLAYLISTITEMSCOMMAND_H
#define REMOVEPLAYLISTITEMSCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class RemovePlaylistItemsCommand : public QUndoCommand
{
public:
    RemovePlaylistItemsCommand(MainWindow* window, Playlist& playlist, const QList<Playlist::Item*>& items);

private:
    MainWindow* _mainWindow;

    Playlist& _playlist;
    QList<Playlist::Item*> _items;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPLAYLISTITEMSCOMMAND_H
