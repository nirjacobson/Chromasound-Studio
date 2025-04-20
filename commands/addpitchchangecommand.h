#ifndef ADDPITCHCHANGECOMMAND_H
#define ADDPITCHCHANGECOMMAND_H

#include <QUndoCommand>
#include <QWidget>

#include "project/track.h"

class MainWindow;

class AddPitchChangeCommand : public QUndoCommand
{
public:
    AddPitchChangeCommand(MainWindow* window, Track& track, const float time, const float pitch);

private:
    MainWindow* _mainWindow;

    Track& _track;
    float _time;
    float _pitch;
    Track::PitchChange* _change;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};
#endif // ADDPITCHCHANGECOMMAND_H
