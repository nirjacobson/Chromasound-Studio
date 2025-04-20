#ifndef EDITPITCHCHANGECOMMAND_H
#define EDITPITCHCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/track.h"

class MainWindow;

class EditPitchChangeCommand : public QUndoCommand
{
public:
    EditPitchChangeCommand(MainWindow* window, Track::PitchChange* change, const float toPitch);

private:
    MainWindow* _mainWindow;

    float _toPitch;

    Track::PitchChange* _change;
    float _fromPitch;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // EDITPITCHCHANGECOMMAND_H
