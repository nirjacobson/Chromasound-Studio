#ifndef EDITOPNOPERATORSETTINGSCOMMAND_H
#define EDITOPNOPERATORSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/opnoperatorsettings.h"

class MainWindow;

class EditOPNOperatorSettingsCommand : public QUndoCommand
{
    public:
        EditOPNOperatorSettingsCommand(MainWindow* window, OPNOperatorSettings& settings, const OPNOperatorSettings& update);
    private:
        enum Type {
            NONE,
            AR,
            T1L,
            D1R,
            T2L,
            D2R,
            RR,
            RS
        };

        Type _type;

        MainWindow* _mainWindow;
        OPNOperatorSettings& _settings;
        OPNOperatorSettings _settingsBefore;
        OPNOperatorSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITOPNOPERATORSETTINGSCOMMAND_H
