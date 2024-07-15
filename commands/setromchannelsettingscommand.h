#ifndef SETROMCHANNELSETTINGSCOMMAND_H
#define SETROMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/romchannelsettings.h"

class MainWindow;

class SetROMChannelSettingsCommand : public QUndoCommand
{
    public:
        SetROMChannelSettingsCommand(MainWindow* window, ROMChannelSettings& settings, const ROMChannelSettings& settingsAfter);

    private:
        MainWindow* _mainWindow;
        ROMChannelSettings& _settings;
        ROMChannelSettings _settingsBefore;
        ROMChannelSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // SETROMCHANNELSETTINGSCOMMAND_H
