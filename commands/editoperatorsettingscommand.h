#ifndef EDITOPERATORSETTINGSCOMMAND_H
#define EDITOPERATORSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/operatorsettings.h"

class MainWindow;

class EditOperatorSettingsCommand : public QUndoCommand
{
public:
    EditOperatorSettingsCommand(MainWindow* window, OperatorSettings& settings, const OperatorSettings& update);
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
    OperatorSettings& _settings;
    OperatorSettings _settingsBefore;
    OperatorSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // EDITOPERATORSETTINGSCOMMAND_H
