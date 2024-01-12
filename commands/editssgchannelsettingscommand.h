#ifndef EDITSSGCHANNELSETTINGSCOMMAND_H
#define EDITSSGCHANNELSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/ssgchannelsettings.h"

class MainWindow;

class EditSSGChannelSettingsCommand : public QUndoCommand
{
public:
    EditSSGChannelSettingsCommand(MainWindow* window, SSGChannelSettings& settings, const SSGChannelSettings& update);
private:
    MainWindow* _mainWindow;
    SSGChannelSettings& _settings;
    SSGChannelSettings _settingsBefore;
    SSGChannelSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITSSGCHANNELSETTINGSCOMMAND_H
