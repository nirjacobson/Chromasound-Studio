#ifndef EDITRHYTHMCHANNELSETTINGSCOMMAND_H
#define EDITRHYTHMCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/rhythmchannelsettings.h"

class MainWindow;


class EditRhythmChannelSettingsCommand : public QUndoCommand
{
public:
    EditRhythmChannelSettingsCommand(MainWindow* window, RhythmChannelSettings& settings, const RhythmChannelSettings& update);
private:
    MainWindow* _mainWindow;
    RhythmChannelSettings& _settings;
    RhythmChannelSettings _settingsBefore;
    RhythmChannelSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITRHYTHMCHANNELSETTINGSCOMMAND_H
