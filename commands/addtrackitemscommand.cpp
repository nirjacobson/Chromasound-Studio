#include "addtrackitemscommand.h"
#include "mainwindow.h"

AddTrackItemsCommand::AddTrackItemsCommand(MainWindow* window, Track& track, const float time, const QList<Track::Item*>& items, bool ignoreLeadingSilence)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _items(items)
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

    _track.addItems(_items);

    _mainWindow->doUpdate();
}
