#include "replacetrackitemscommand.h"

ReplaceTrackItemsCommand::ReplaceTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& items)
    : _mainWindow(window)
    , _track(track)
    , _items(items)
    , _addTrackItemsCommand(nullptr)
    , _removeTrackItemsCommand(nullptr)
{
    setText("load track");
}

ReplaceTrackItemsCommand::~ReplaceTrackItemsCommand()
{
    if (_addTrackItemsCommand) delete _addTrackItemsCommand;
    if (_removeTrackItemsCommand) delete _removeTrackItemsCommand;
}

void ReplaceTrackItemsCommand::undo()
{
    _addTrackItemsCommand->undo();
    delete _addTrackItemsCommand;
    _addTrackItemsCommand = nullptr;

    _removeTrackItemsCommand->undo();
    delete _removeTrackItemsCommand;
    _removeTrackItemsCommand = nullptr;
}

void ReplaceTrackItemsCommand::redo()
{
    _removeTrackItemsCommand = new RemoveTrackItemsCommand(_mainWindow, _track, _track.items());
    _removeTrackItemsCommand->redo();
    _addTrackItemsCommand = new AddTrackItemsCommand(_mainWindow, _track, 0, _items);
    _addTrackItemsCommand->redo();
}
