#ifndef EDITMELODYCHANNELSETTINGSCOMMAND_H
#define EDITMELODYCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/melodychannelsettings.h"

class MainWindow;

class EditMelodyChannelSettingsCommand : public QUndoCommand
{
    public:
        EditMelodyChannelSettingsCommand(MainWindow* window, MelodyChannelSettings& settings, const MelodyChannelSettings& update);
    private:
        MainWindow* _mainWindow;
        MelodyChannelSettings& _settings;
        MelodyChannelSettings _settingsBefore;
        MelodyChannelSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // EDITMELODYCHANNELSETTINGSCOMMAND_H
