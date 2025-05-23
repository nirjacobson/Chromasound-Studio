#ifndef ADDTRACKITEMSCOMMAND_H
#define ADDTRACKITEMSCOMMAND_H

#include <QUndoCommand>

#include "../project/track.h"

class MainWindow;

class AddTrackItemsCommand : public QUndoCommand
{
    public:
        AddTrackItemsCommand(MainWindow* window, Track& track, const float time, const QList<Track::Item*>& items, bool ignoreLeadingSilence = true);
        AddTrackItemsCommand(MainWindow* window, Track& track, const float time, const QList<Track::Item*>& items, const QList<Track::PitchChange*>& pitchChanges, bool ignoreLeadingSilence = true);

    private:
        MainWindow* _mainWindow;

        Track& _track;
        float _time;
        QList<Track::Item*> _items;
        QList<Track::PitchChange*> _pitchChanges;
        bool _ignoreLeadingSilence;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // ADDTRACKITEMSCOMMAND_H
