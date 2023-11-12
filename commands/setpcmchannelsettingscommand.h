#ifndef SETPCMCHANNELSETTINGSCOMMAND_H
#define SETPCMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/pcmchannelsettings.h"

class MainWindow;

class SetPCMChannelSettingsCommand : public QUndoCommand
{
public:
    SetPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& settingsAfter);

private:
    MainWindow* _mainWindow;
    PCMChannelSettings& _settings;
    PCMChannelSettings _settingsBefore;
    PCMChannelSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPCMCHANNELSETTINGSCOMMAND_H
