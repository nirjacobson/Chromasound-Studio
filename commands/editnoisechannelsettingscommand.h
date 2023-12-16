#ifndef EDITNOISECHANNELSETTINGSCOMMAND_H
#define EDITNOISECHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/noisechannelsettings.h"

class MainWindow;

class EditNoiseChannelSettingsCommand : public QUndoCommand
{
    public:
        EditNoiseChannelSettingsCommand(MainWindow* window, NoiseChannelSettings& settings, const NoiseChannelSettings& update);

    private:
        MainWindow* _mainWindow;
        NoiseChannelSettings& _settings;
        NoiseChannelSettings _settingsBefore;
        NoiseChannelSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // EDITNOISECHANNELSETTINGSCOMMAND_H
