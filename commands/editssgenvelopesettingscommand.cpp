#include "editssgenvelopesettingscommand.h"
#include "mainwindow.h"

EditSSGEnvelopeSettingsCommand::EditSSGEnvelopeSettingsCommand(MainWindow* window, SSGEnvelopeSettings& settings, const SSGEnvelopeSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
{
    setText("edit SSG envelope shape");
}

void EditSSGEnvelopeSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->doUpdate();
}

void EditSSGEnvelopeSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->doUpdate();
}
