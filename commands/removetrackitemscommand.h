#ifndef REMOVETRACKITEMSCOMMAND_H
#define REMOVETRACKITEMSCOMMAND_H

#include <QUndoCommand>

#include "../project/track.h"

class MainWindow;

class RemoveTrackItemsCommand : public QUndoCommand
{
    public:
        RemoveTrackItemsCommand(MainWindow* window, Track& track, const QList<Track::Item*>& notes, const QList<Track::PitchChange*>& pitchChanges);
        ~RemoveTrackItemsCommand();
    private:
        MainWindow* _mainWindow;

        Track& _track;
        QList<Track::Item*> _notes;
        QList<Track::PitchChange*> _pitchChanges;

        bool _performed;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
};

#endif // REMOVETRACKITEMSCOMMAND_H
