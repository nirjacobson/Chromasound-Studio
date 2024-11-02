#ifndef SETPCMCHANNELSETTINGSCOMMAND_H
#define SETPCMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/pcmchannelsettings.h"

class MainWindow;

class SetPCMChannelSettingsCommand : public QUndoCommand
{
    public:
        SetPCMChannelSettingsCommand(MainWindow* window, PCMChannelSettings& settings, const PCMChannelSettings& settingsAfter, const bool merge = false);

    private:
        MainWindow* _mainWindow;
        QList<PCMChannelSettings*> _settings;
        QList<PCMChannelSettings> _settingsBefore;
        QList<PCMChannelSettings> _settingsAfter;
        bool _merge;

        // QUndoCommand interface
    public:
        void undo();
        void redo();

        // QUndoCommand interface
    public:
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // SETPCMCHANNELSETTINGSCOMMAND_H
