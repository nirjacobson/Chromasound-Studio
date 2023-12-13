#include "setfmchannelcommand.h"
#include "mainwindow.h"

SetFMChannelCommand::SetFMChannelCommand(MainWindow* window, Channel& channel, const FMChannelSettings& settingsAfter, const QString& name)
    : _mainWindow(window)
    , _channel(channel)
    , _settingsAfter(settingsAfter)
    , _setChannelTypeCommand(nullptr)
    , _setFMChannelSettingsCommand(nullptr)
    , _setChannelNameCommand(nullptr)
    , _name(name)
{
    setText("set channel settings");
}

SetFMChannelCommand::~SetFMChannelCommand()
{
    if (_setChannelNameCommand) delete _setChannelNameCommand;
    if (_setFMChannelSettingsCommand) delete _setFMChannelSettingsCommand;
    if (_setChannelTypeCommand) delete _setChannelTypeCommand;
}

void SetFMChannelCommand::undo()
{
    if (_setChannelNameCommand) {
        _setChannelNameCommand->undo();
        delete _setChannelNameCommand;
        _setChannelNameCommand = nullptr;
    }

    _setFMChannelSettingsCommand->undo();
    delete _setFMChannelSettingsCommand;
    _setFMChannelSettingsCommand = nullptr;

    if (_setChannelTypeCommand) {
        _setChannelTypeCommand->undo();
        delete _setChannelTypeCommand;
        _setChannelTypeCommand = nullptr;
    }

    _mainWindow->doUpdate();
}

void SetFMChannelCommand::redo()
{
    if (_channel.type() != Channel::Type::FM) {
        _setChannelTypeCommand = new SetChannelTypeCommand(_mainWindow, _channel, Channel::Type::FM);
        _setChannelTypeCommand->redo();
    }

    _setFMChannelSettingsCommand = new SetFMChannelSettingsCommand(_mainWindow, dynamic_cast<FMChannelSettings&>(_channel.settings()), _settingsAfter);
    _setFMChannelSettingsCommand->redo();

    if (!_name.isEmpty()) {
        _setChannelNameCommand = new SetChannelNameCommand(_mainWindow, _channel, _name);
        _setChannelNameCommand->redo();
    }

    _mainWindow->doUpdate();
}
