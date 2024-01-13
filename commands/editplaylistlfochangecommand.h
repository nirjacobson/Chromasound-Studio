#ifndef EDITPLAYLISTLFOCHANGECOMMAND_H
#define EDITPLAYLISTLFOCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class EditPlaylistLFOChangeCommand : public QUndoCommand
{
public:
    EditPlaylistLFOChangeCommand(MainWindow* window, Playlist::LFOChange* change, const int newMode);

private:
    MainWindow* _mainWindow;
    Playlist::LFOChange* _change;
    int _oldMode;
    int _newMode;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPLAYLISTLFOCHANGECOMMAND_H
