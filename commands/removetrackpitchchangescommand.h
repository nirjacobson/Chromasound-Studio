#ifndef REMOVETRACKPITCHCHANGESCOMMAND_H
#define REMOVETRACKPITCHCHANGESCOMMAND_H

#include <QUndoCommand>

#include "../project/track.h"

class MainWindow;

class RemoveTrackPitchChangesCommand : public QUndoCommand
{
public:
    RemoveTrackPitchChangesCommand(MainWindow* window, Track& track, const QList<Track::PitchChange*>& items);
    ~RemoveTrackPitchChangesCommand();
private:
    MainWindow* _mainWindow;

    Track& _track;
    QList<Track::PitchChange*> _items;

    bool _performed;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};
#endif // REMOVETRACKPITCHCHANGESCOMMAND_H
