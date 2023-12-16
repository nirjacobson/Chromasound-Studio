#ifndef EDITPLAYLISTCOMMAND_H
#define EDITPLAYLISTCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class EditPlaylistCommand : public QUndoCommand
{
    public:
        EditPlaylistCommand(MainWindow* window, Playlist::Item* item, const float toTime, const QList<Playlist::Item*>& group);
    private:
        MainWindow* _mainWindow;
        Playlist::Item* _item;
        float _fromTime;
        float _toTime;

        QMap<Playlist::Item*, EditPlaylistCommand*> _groupCommands;
        QList<Playlist::Item*> _group;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITPLAYLISTCOMMAND_H
