#ifndef EDITOPLSETTINGSCOMMAND_H
#define EDITOPLSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "opl/usertone/oplsettings.h"

class MainWindow;

class EditOPLSettingsCommand : public QUndoCommand
{
    public:
        EditOPLSettingsCommand(MainWindow* window, OPLSettings& settings, const OPLSettings& update);

    private:
        enum Type {
            NONE,
            AR,
            DR,
            SL,
            RR,
            MULTI,
            KSL,
            TL,
            FB,
        };

        Type _type;
        int _operator;

        MainWindow* _mainWindow;
        OPLSettings& _settings;
        OPLSettings _settingsBefore;
        OPLSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITOPLSETTINGSCOMMAND_H
