#include "addpcmchannelcommand.h"
#include "mainwindow.h"

AddPCMChannelCommand::AddPCMChannelCommand(MainWindow* window, const PCMChannelSettings& settingsAfter, const QString& name)
    : _mainWindow(window)
    , _settingsAfter(settingsAfter)
    , _name(name)
    , _addChannelCommand(nullptr)
    , _setPCMChannelCommand(nullptr)
{
    setText("add PCM channel");
}

AddPCMChannelCommand::~AddPCMChannelCommand()
{
    if (_addChannelCommand) delete _addChannelCommand;
    if (_setPCMChannelCommand) delete _setPCMChannelCommand;
}

void AddPCMChannelCommand::undo()
{
    _setPCMChannelCommand->undo();
    delete _setPCMChannelCommand;
    _setPCMChannelCommand = nullptr;

    _addChannelCommand->undo();
    delete _addChannelCommand;
    _addChannelCommand = nullptr;
}

void AddPCMChannelCommand::redo()
{
    _addChannelCommand = new AddChannelCommand(_mainWindow);
    _addChannelCommand->redo();

    _setPCMChannelCommand = new SetPCMChannelCommand(_mainWindow, _mainWindow->app()->project().getChannel(_mainWindow->app()->project().channels() - 1), _settingsAfter, _name);
    _setPCMChannelCommand->redo();
}
