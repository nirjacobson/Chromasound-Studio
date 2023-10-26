#ifndef ADDPLAYLISTITEMSCOMMAND_H
#define ADDPLAYLISTITEMSCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistItemsCommand : public QUndoCommand
{
public:
    AddPlaylistItemsCommand(MainWindow* window, Playlist& playlist, const float time, const QList<Playlist::Item*>& items);
private:
    MainWindow* _mainWindow;

    Playlist& _playlist;
    float _time;
    QList<Playlist::Item*> _items;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDPLAYLISTITEMSCOMMAND_H
