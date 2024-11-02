#ifndef EDITPCMCHANNELSETTINGSCOMMAND_H
#define EDITPCMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/pcmchannelsettings.h"

class MainWindow;

class EditPCMChannelSettingsCommand : public QUndoCommand
{
public:
    EditPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& update);

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

#endif // EDITPCMCHANNELSETTINGSCOMMAND_H
