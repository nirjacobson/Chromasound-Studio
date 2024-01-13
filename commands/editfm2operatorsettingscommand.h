#ifndef EDITFM2OPERATORSETTINGSCOMMAND_H
#define EDITFM2OPERATORSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "usertone/fm2operatorsettings.h"

class MainWindow;

class EditFM2OperatorSettingsCommand : public QUndoCommand
{
public:
    EditFM2OperatorSettingsCommand(MainWindow* window, FM2OperatorSettings& settings, const FM2OperatorSettings& update);

private:
    enum Type {
        NONE,
        AR,
        DR,
        SL,
        RR,
        MULTI,
        KSL
    };

    Type _type;

    MainWindow* _mainWindow;
    FM2OperatorSettings& _settings;
    FM2OperatorSettings _settingsBefore;
    FM2OperatorSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // EDITFM2OPERATORSETTINGSCOMMAND_H
