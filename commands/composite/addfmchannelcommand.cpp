#include "addfmchannelcommand.h"
#include "mainwindow.h"

AddFMChannelCommand::AddFMChannelCommand(MainWindow* window, const FMChannelSettings& settingsAfter, const QString& name)
    : _mainWindow(window)
    , _settingsAfter(settingsAfter)
    , _name(name)
    , _addChannelCommand(nullptr)
    , _setFMChannelCommand(nullptr)
{
    setText("add FM channel");
}

AddFMChannelCommand::~AddFMChannelCommand()
{
    if (_addChannelCommand) delete _addChannelCommand;
    if (_setFMChannelCommand) delete _setFMChannelCommand;
}

void AddFMChannelCommand::undo()
{
    _setFMChannelCommand->undo();
    delete _setFMChannelCommand;
    _setFMChannelCommand = nullptr;

    _addChannelCommand->undo();
    delete _addChannelCommand;
    _addChannelCommand = nullptr;
}

void AddFMChannelCommand::redo()
{
    _addChannelCommand = new AddChannelCommand(_mainWindow);
    _addChannelCommand->redo();

    _setFMChannelCommand = new SetFMChannelCommand(_mainWindow, _mainWindow->app()->project().getChannel(_mainWindow->app()->project().channels() - 1), _settingsAfter, _name);
    _setFMChannelCommand->redo();
}
