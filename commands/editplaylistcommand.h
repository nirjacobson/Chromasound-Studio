#ifndef EDITPLAYLISTCOMMAND_H
#define EDITPLAYLISTCOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class EditPlaylistCommand : public QUndoCommand
{
public:
    EditPlaylistCommand(MainWindow* window, Playlist::Item* item, const float toTime);
private:
    MainWindow* _mainWindow;
    Playlist::Item* _item;
    float _fromTime;
    float _toTime;

    // QUndoCommand interface
public:
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // EDITPLAYLISTCOMMAND_H
