#include "deletechannelcommand.h"
#include "mainwindow.h"

DeleteChannelCommand::DeleteChannelCommand(MainWindow* window, const Channel& channel, const int index)
    : _mainWindow(window)
    , _index(index)
{
    setText("delete channel");
}

void DeleteChannelCommand::undo()
{
    _mainWindow->addChannel(_index, _deletedChannel);
}

void DeleteChannelCommand::redo()
{
    _deletedChannel = _mainWindow->deleteChannel(_index);
}
