#ifndef RESETFMCHANNELSETTINGSCOMMAND_H
#define RESETFMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/fmchannelsettings.h"

class MainWindow;

class ResetFMChannelSettingsCommand : public QUndoCommand
{
public:
    ResetFMChannelSettingsCommand(MainWindow* window, FMChannelSettings& settings);

private:
    MainWindow* _mainWindow;
    FMChannelSettings& _settings;
    FMChannelSettings _settingsBefore;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // RESETFMCHANNELSETTINGSCOMMAND_H
