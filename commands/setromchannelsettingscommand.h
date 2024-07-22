#ifndef SETROMCHANNELSETTINGSCOMMAND_H
#define SETROMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/romchannelsettings.h"

class MainWindow;

class SetROMChannelSettingsCommand : public QUndoCommand
{
    public:
        SetROMChannelSettingsCommand(MainWindow* window, ROMChannelSettings& settings, const ROMChannelSettings& settingsAfter, const bool merge = false);

    private:
        MainWindow* _mainWindow;
        QList<ROMChannelSettings*> _settings;
        QList<ROMChannelSettings> _settingsBefore;
        QList<ROMChannelSettings> _settingsAfter;
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

#endif // SETROMCHANNELSETTINGSCOMMAND_H
