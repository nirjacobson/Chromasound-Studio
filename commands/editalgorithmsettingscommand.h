#ifndef EDITALGORITHMSETTINGSCOMMAND_H
#define EDITALGORITHMSETTINGSCOMMAND_H

#include <QUndoCommand>

#include "project/channel/algorithmsettings.h"

class MainWindow;

class EditAlgorithmSettingsCommand : public QUndoCommand
{
public:
    EditAlgorithmSettingsCommand(MainWindow* window, AlgorithmSettings& settings, const AlgorithmSettings& update);

private:
    MainWindow* _mainWindow;
    AlgorithmSettings& _settings;
    AlgorithmSettings _settingsBefore;
    AlgorithmSettings _settingsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();

    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // EDITALGORITHMSETTINGSCOMMAND_H
