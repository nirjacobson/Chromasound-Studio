#include "addtrackcommand.h"
#include "mainwindow.h"

AddTrackCommand::AddTrackCommand(MainWindow* window, const QList<Track::Item*>& items, const QString& name)
    : _mainWindow(window)
    , _items(items)
    , _name(name)
    , _addChannelCommand(nullptr)
    , _addTrackItemsCommand(nullptr)
    , _setChannelNameCommand(nullptr)
{
    setText("add track");
}

void AddTrackCommand::undo()
{
    _addTrackItemsCommand->undo();
    delete _addTrackItemsCommand;

    _setChannelNameCommand->undo();
    delete _setChannelNameCommand;

    _addChannelCommand->undo();
    delete _addChannelCommand;
}

void AddTrackCommand::redo()
{
    _addChannelCommand = new AddChannelCommand(_mainWindow);
    _addChannelCommand->redo();

    _setChannelNameCommand = new SetChannelNameCommand(_mainWindow, _mainWindow->app()->project().getChannel(_mainWindow->app()->project().channels() - 1), _name);
    _setChannelNameCommand->redo();

    Track& track = _mainWindow->app()->project().getFrontPattern().getTrack(_mainWindow->app()->project().channels() - 1);

    _addTrackItemsCommand = new AddTrackItemsCommand(_mainWindow,
                                                     track,
                                                     0,
                                                     _items);
    _addTrackItemsCommand->redo();

    track.usePianoRoll();
}
