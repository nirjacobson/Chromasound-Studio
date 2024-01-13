#ifndef EDITFM4OPERATORSETTINGSCOMMAND_H
#define EDITFM4OPERATORSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/fm4operatorsettings.h"

class MainWindow;

class EditFM4OperatorSettingsCommand : public QUndoCommand
{
    public:
    EditFM4OperatorSettingsCommand(MainWindow* window, FM4OperatorSettings& settings, const FM4OperatorSettings& update);
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
        FM4OperatorSettings& _settings;
        FM4OperatorSettings _settingsBefore;
        FM4OperatorSettings _settingsAfter;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITFM4OPERATORSETTINGSCOMMAND_H
