#include "removetrackitemscommand.h"
#include "mainwindow.h"

RemoveTrackItemsCommand::RemoveTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& notes, const QList<Track::PitchChange *> &pitchChanges)
    : _mainWindow(window)
    , _track(track)
    , _notes(notes)
    , _pitchChanges(pitchChanges)
    , _performed(false)
{
    setText("remove notes");
}

RemoveTrackItemsCommand::~RemoveTrackItemsCommand()
{
    if (_performed) {
        for (Track::Item* item : _notes) {
            delete item;
        }
        for (Track::PitchChange* pc : _pitchChanges) {
            delete pc;
        }
    }
}

void RemoveTrackItemsCommand::undo()
{
    _track.addPitchChanges(_pitchChanges);
    _track.addItems(_notes);

    _mainWindow->doUpdate();

    _performed = false;
}

void RemoveTrackItemsCommand::redo()
{
    _track.removeItems(_notes);
    _track.removePitchChanges(_pitchChanges);

    _mainWindow->doUpdate();

    _performed = true;
}
