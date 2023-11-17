#ifndef ADDTRACKSETTINGSCHANGECOMMAND_H
#define ADDTRACKSETTINGSCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/track.h"
#include "project/channel/channelsettings.h"

class MainWindow;

class AddTrackSettingsChangeCommand : public QUndoCommand
{
public:
    AddTrackSettingsChangeCommand(MainWindow* window, Track& track, const float time, ChannelSettings* settings);

private:
    MainWindow* _mainWindow;
    Track& _track;
    float _time;
    ChannelSettings* _settings;

    Track::SettingsChange* _change;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDTRACKSETTINGSCHANGECOMMAND_H
