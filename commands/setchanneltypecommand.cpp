#include "mainwindow.h"
#include "setchanneltypecommand.h"

SetChannelTypeCommand::SetChannelTypeCommand(MainWindow* window, Channel& channel, const Channel::Type type)
    : _mainWindow(window)
    , _channel(channel)
    , _originalType(channel.type())
    , _newType(type)
{
    setText("set channel type");
}

void SetChannelTypeCommand::undo()
{
    _mainWindow->setChannelType(_channel, _originalType);
}

void SetChannelTypeCommand::redo()
{
    _mainWindow->setChannelType(_channel, _newType);
}
