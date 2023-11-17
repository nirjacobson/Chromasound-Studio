#include "setchannelnamecommand.h"
#include "mainwindow.h"

SetChannelNameCommand::SetChannelNameCommand(MainWindow* window, Channel& channel, const QString& name)
    : _mainWindow(window)
    , _channel(channel)
    , _nameBefore(channel.name())
    , _nameAfter(name)
{
    setText("set channel name");
}

void SetChannelNameCommand::undo()
{
    _channel.setName(_nameBefore);
    _mainWindow->doUpdate();
}

void SetChannelNameCommand::redo()
{
    _channel.setName(_nameAfter);
    _mainWindow->doUpdate();
}
