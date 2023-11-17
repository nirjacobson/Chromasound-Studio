#include "addchannelcommand.h"
#include "mainwindow.h"

AddChannelCommand::AddChannelCommand(MainWindow* window)
    : _mainWindow(window)
{
    setText("add channel");
}

void AddChannelCommand::undo()
{
    _mainWindow->deleteChannel(_mainWindow->channels() - 1);
}

void AddChannelCommand::redo()
{
    _mainWindow->addChannel();
}
