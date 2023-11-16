#include "setpcmchannelcommand.h"

SetPCMChannelCommand::SetPCMChannelCommand(MainWindow* window, Channel& channel, PCMChannelSettings settingsAfter, const QString& name)
    : _mainWindow(window)
    , _channel(channel)
    , _settingsAfter(settingsAfter)
    , _setChannelTypeCommand(nullptr)
    , _setPCMChannelSettingsCommand(nullptr)
    , _setChannelNameCommand(nullptr)
    , _name(name)
{
    setText("set channel settings");
}

SetPCMChannelCommand::~SetPCMChannelCommand()
{
    if (_setChannelNameCommand) delete _setChannelNameCommand;
    if (_setPCMChannelSettingsCommand) delete _setPCMChannelSettingsCommand;
    if (_setChannelTypeCommand) delete _setChannelTypeCommand;
}

void SetPCMChannelCommand::undo()
{
    if (_setChannelNameCommand) {
        _setChannelNameCommand->undo();
        delete _setChannelNameCommand;
        _setChannelNameCommand = nullptr;
    }

    _setPCMChannelSettingsCommand->undo();
    delete _setPCMChannelSettingsCommand;
    _setPCMChannelSettingsCommand = nullptr;

    if (_setChannelTypeCommand) {
        _setChannelTypeCommand->undo();
        delete _setChannelTypeCommand;
        _setPCMChannelSettingsCommand = nullptr;
    }
}

void SetPCMChannelCommand::redo()
{
    if (_channel.type() != Channel::Type::PCM) {
        _setChannelTypeCommand = new SetChannelTypeCommand(_mainWindow, _channel, Channel::Type::PCM);
        _setChannelTypeCommand->redo();
    }

    _setPCMChannelSettingsCommand = new SetPCMChannelSettingsCommand(_mainWindow, dynamic_cast<PCMChannelSettings&>(_channel.settings()), _settingsAfter);
    _setPCMChannelSettingsCommand->redo();

    if (!_name.isEmpty()) {
        _setChannelNameCommand = new SetChannelNameCommand(_mainWindow, _channel, _name);
        _setChannelNameCommand->redo();
    }
}
