#ifndef EDITROMCHANNELSETTINGSCOMMAND_H
#define EDITROMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/romchannelsettings.h"

class MainWindow;

class EditROMChannelSettingsCommand : public QUndoCommand
{
public:
    EditROMChannelSettingsCommand(MainWindow* window, ROMChannelSettings& settings, const ROMChannelSettings& update);

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

#endif // EDITROMCHANNELSETTINGSCOMMAND_H
