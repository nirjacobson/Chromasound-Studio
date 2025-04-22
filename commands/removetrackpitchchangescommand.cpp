#include "removetrackpitchchangescommand.h"
#include "mainwindow.h"

RemoveTrackPitchChangesCommand::RemoveTrackPitchChangesCommand(MainWindow *window, Track &track, const QList<Track::PitchChange *> &items)
    : _mainWindow(window)
    , _track(track)
    , _items(items)
    , _performed(false)
{
    setText("remove pitch changes");
}

RemoveTrackPitchChangesCommand::~RemoveTrackPitchChangesCommand()
{
    if (_performed) {
        for (Track::PitchChange* pc : _items) {
            delete pc;
        }
    }
}

void RemoveTrackPitchChangesCommand::undo()
{
    _track.addPitchChanges(_items);

    _mainWindow->doUpdate();

    _performed = false;
}

void RemoveTrackPitchChangesCommand::redo()
{
    _track.removePitchChanges(_items);

    _mainWindow->doUpdate();

    _performed = true;
}
