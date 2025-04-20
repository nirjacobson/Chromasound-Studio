#ifndef REMOVEPITCHCHANGECOMMAND_H
#define REMOVEPITCHCHANGECOMMAND_H

#include <QUndoCommand>
#include "project/track.h"

class MainWindow;

class RemovePitchChangeCommand : public QUndoCommand
{
public:
    RemovePitchChangeCommand(MainWindow* window, Track& track, Track::PitchChange* change);
    ~RemovePitchChangeCommand();

private:
    MainWindow* _mainWindow;
    Track& _track;
    Track::PitchChange* _change;

    bool _performed;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVEPITCHCHANGECOMMAND_H
