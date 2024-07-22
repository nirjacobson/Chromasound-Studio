#include "mainwindow.h"
#include "setromchannelsettingscommand.h"

SetROMChannelSettingsCommand::SetROMChannelSettingsCommand(MainWindow* window, ROMChannelSettings& settings, const ROMChannelSettings& settingsAfter, const bool merge)
    : _mainWindow(window)
    , _merge(merge)
{
    setText("set channel settings");

    _settings.append(&settings);
    _settingsBefore.append(settings);
    _settingsAfter.append(settingsAfter);
}

void SetROMChannelSettingsCommand::undo()
{
    for (int i = 0; i < _settings.size(); i++) {
        *_settings[i] = _settingsBefore[i];
    }

    _mainWindow->channelSettingsUpdated();
}

void SetROMChannelSettingsCommand::redo()
{
    for (int i = 0; i < _settings.size(); i++) {
        *_settings[i] = _settingsAfter[i];
    }

    _mainWindow->channelSettingsUpdated();
}

int SetROMChannelSettingsCommand::id() const
{
    return 'R';
}

bool SetROMChannelSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const SetROMChannelSettingsCommand* otherComm = dynamic_cast<const SetROMChannelSettingsCommand*>(other);

    if (otherComm->_merge) {
        _settings.append(otherComm->_settings);
        _settingsBefore.append(otherComm->_settingsBefore);
        _settingsAfter.append(otherComm->_settingsAfter);

        return true;
    }

    return false;
}
