#include "setpcmchannelcommand.h"
#include "mainwindow.h"

SetPCMChannelCommand::SetPCMChannelCommand(MainWindow* window, Channel& channel, ROMChannelSettings settingsAfter, const QString& name)
    : _mainWindow(window)
    , _channel(channel)
    , _settingsAfter(settingsAfter)
    , _setChannelTypeCommand(nullptr)
    , _setROMChannelSettingsCommand(nullptr)
    , _setChannelNameCommand(nullptr)
    , _name(name)
{
    setText("set channel settings");
}

SetPCMChannelCommand::~SetPCMChannelCommand()
{
    if (_setChannelNameCommand) delete _setChannelNameCommand;
    if (_setROMChannelSettingsCommand) delete _setROMChannelSettingsCommand;
    if (_setChannelTypeCommand) delete _setChannelTypeCommand;
}

void SetPCMChannelCommand::undo()
{
    if (_setChannelNameCommand) {
        _setChannelNameCommand->undo();
        delete _setChannelNameCommand;
        _setChannelNameCommand = nullptr;
    }

    _setROMChannelSettingsCommand->undo();
    delete _setROMChannelSettingsCommand;
    _setROMChannelSettingsCommand = nullptr;

    if (_setChannelTypeCommand) {
        _setChannelTypeCommand->undo();
        delete _setChannelTypeCommand;
        _setROMChannelSettingsCommand = nullptr;
    }

    _mainWindow->doUpdate();
}

void SetPCMChannelCommand::redo()
{
    if (_channel.type() != Channel::Type::PCM) {
        _setChannelTypeCommand = new SetChannelTypeCommand(_mainWindow, _channel, Channel::Type::PCM);
        _setChannelTypeCommand->redo();
    }

    _setROMChannelSettingsCommand = new SetROMChannelSettingsCommand(_mainWindow, dynamic_cast<ROMChannelSettings&>(_channel.settings()), _settingsAfter);
    _setROMChannelSettingsCommand->redo();

    if (!_name.isEmpty()) {
        _setChannelNameCommand = new SetChannelNameCommand(_mainWindow, _channel, _name);
        _setChannelNameCommand->redo();
    }

    _mainWindow->doUpdate();
}
