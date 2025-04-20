#include "addtrackitemscommand.h"
#include "mainwindow.h"

AddTrackItemsCommand::AddTrackItemsCommand(MainWindow* window, Track& track, const float time, const QList<Track::Item*>& items, bool ignoreLeadingSilence)
    : AddTrackItemsCommand(window, track, time, items, QList<Track::PitchChange*>(), ignoreLeadingSilence)
{

}

AddTrackItemsCommand::AddTrackItemsCommand(MainWindow *window, Track &track, const float time, const QList<Track::Item *> &items, const QList<Track::PitchChange *> &pitchChanges, bool ignoreLeadingSilence)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _items(items)
    , _pitchChanges(pitchChanges)
    , _ignoreLeadingSilence(ignoreLeadingSilence)
{
    setText("paste notes");
}

void AddTrackItemsCommand::undo()
{
    _track.removeItems(_items);

    _mainWindow->doUpdate();
}

void AddTrackItemsCommand::redo()
{
    float earliestTime = 0;

    if (_ignoreLeadingSilence) {
        earliestTime = _items[0]->time();
        for (Track::Item* item : _items) {
            if (item->time() < earliestTime) {
                earliestTime = item->time();
            }
        }
    }

    for (Track::Item* item : _items) {
        item->setTime(item->time() - earliestTime + _time);
    }

    for (Track::PitchChange* pc : _pitchChanges) {
        pc->setTime(pc->time() - earliestTime + _time);
    }

    _track.addItems(_items);
    _track.addPitchChanges(_pitchChanges);

    _mainWindow->doUpdate();
}
