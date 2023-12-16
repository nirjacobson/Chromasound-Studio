#ifndef SETFMCHANNELSETTINGSCOMMAND_H
#define SETFMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/fmchannelsettings.h"

class MainWindow;

class SetFMChannelSettingsCommand : public QUndoCommand
{
    public:
        SetFMChannelSettingsCommand(MainWindow* window, FMChannelSettings& settings, const FMChannelSettings& settingsAfter);

    private:
        MainWindow* _mainWindow;
        FMChannelSettings& _settings;
        FMChannelSettings _settingsBefore;
        FMChannelSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // SETFMCHANNELSETTINGSCOMMAND_H
