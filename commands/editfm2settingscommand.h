#ifndef EDITFM2SETTINGSCOMMAND_H
#define EDITFM2SETTINGSCOMMAND_H

#include <QUndoCommand>

#include "usertone/fm2settings.h"

class MainWindow;

class EditFM2SettingsCommand : public QUndoCommand
{
public:
    EditFM2SettingsCommand(MainWindow* window, FM2Settings& settings, const FM2Settings& update);

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
    FM2Settings& _settings;
    FM2Settings _settingsBefore;
    FM2Settings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // EDITFM2SETTINGSCOMMAND_H
