#include "mainwindow.h"
#include "setpcmchannelsettingscommand.h"

SetPCMChannelSettingsCommand::SetPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& settingsAfter, const bool merge)
    : _mainWindow(window)
    , _merge(merge)
{
    setText("set channel settings");

    _settings.append(&settings);
    _settingsBefore.append(settings);
    _settingsAfter.append(settingsAfter);
}

void SetPCMChannelSettingsCommand::undo()
{
    for (int i = 0; i < _settings.size(); i++) {
        *_settings[i] = _settingsBefore[i];
    }

    _mainWindow->channelSettingsUpdated();
}

void SetPCMChannelSettingsCommand::redo()
{
    for (int i = 0; i < _settings.size(); i++) {
        *_settings[i] = _settingsAfter[i];
    }

    _mainWindow->channelSettingsUpdated();
}

int SetPCMChannelSettingsCommand::id() const
{
    return 'R';
}

bool SetPCMChannelSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const SetPCMChannelSettingsCommand* otherComm = dynamic_cast<const SetPCMChannelSettingsCommand*>(other);

    if (otherComm->_merge) {
        _settings.append(otherComm->_settings);
        _settingsBefore.append(otherComm->_settingsBefore);
        _settingsAfter.append(otherComm->_settingsAfter);

        return true;
    }

    return false;
}
