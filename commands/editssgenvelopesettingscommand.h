#ifndef EDITSSGENVELOPESETTINGSCOMMAND_H
#define EDITSSGENVELOPESETTINGSCOMMAND_H

#include <QUndoCommand>

#include "ssg/ssgenvelopesettings.h"

class MainWindow;

class EditSSGEnvelopeSettingsCommand : public QUndoCommand
{
public:
    EditSSGEnvelopeSettingsCommand(MainWindow* window, SSGEnvelopeSettings& settings, const SSGEnvelopeSettings& update);

private:
    MainWindow* _mainWindow;
    SSGEnvelopeSettings& _settings;
    SSGEnvelopeSettings _settingsBefore;
    SSGEnvelopeSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITSSGENVELOPESETTINGSCOMMAND_H
