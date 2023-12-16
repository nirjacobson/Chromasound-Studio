#ifndef REMOVETRACKSETTINGSCHANGECOMMAND_H
#define REMOVETRACKSETTINGSCHANGECOMMAND_H

#include <QUndoCommand>
#include "project/track.h"

class MainWindow;

class RemoveTrackSettingsChangeCommand : public QUndoCommand
{
    public:
        RemoveTrackSettingsChangeCommand(MainWindow* window, Track& track, Track::SettingsChange* change);
        ~RemoveTrackSettingsChangeCommand();

    private:
        MainWindow* _mainWindow;
        Track& _track;
        Track::SettingsChange* _change;

        bool _performed;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // REMOVETRACKSETTINGSCHANGECOMMAND_H
