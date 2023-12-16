#ifndef EDITLFOSETTINGSCOMMAND_H
#define EDITLFOSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/lfosettings.h"

class MainWindow;

class EditLFOSettingsCommand : public QUndoCommand
{
    public:
        EditLFOSettingsCommand(MainWindow* window, LFOSettings& settings, const LFOSettings& update);

    private:
        MainWindow* _mainWindow;
        LFOSettings& _settings;
        LFOSettings _settingsBefore;
        LFOSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITLFOSETTINGSCOMMAND_H
