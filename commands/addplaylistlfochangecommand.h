#ifndef ADDPLAYLISTLFOCHANGECOMMAND_H
#define ADDPLAYLISTLFOCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/playlist.h"

class MainWindow;

class AddPlaylistLFOChangeCommand : public QUndoCommand
{
public:
    AddPlaylistLFOChangeCommand(MainWindow* window, Playlist& playlist, const float time, const int mode);

    void undo();
    void redo();

private:
    MainWindow* _mainWindow;
    Playlist& _playlist;
    float _time;
    int _mode;

    Playlist::LFOChange* _change;
};

#endif // ADDPLAYLISTLFOCHANGECOMMAND_H
