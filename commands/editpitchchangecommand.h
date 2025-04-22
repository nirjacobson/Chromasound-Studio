#ifndef EDITPITCHCHANGECOMMAND_H
#define EDITPITCHCHANGECOMMAND_H

#include <QUndoCommand>

#include "project/track.h"

class MainWindow;

class EditPitchChangeCommand : public QUndoCommand
{
public:
    EditPitchChangeCommand(MainWindow* window, Track::PitchChange* change, const float toTime, const float toPitch, const QList<Track::PitchChange *> &group = QList<Track::PitchChange*>());

private:
    MainWindow* _mainWindow;

    bool _performed;

    float _toTime;
    float _toPitch;


    Track::PitchChange* _change;
    float _fromTime;
    float _fromPitch;

    QMap<Track::PitchChange*, EditPitchChangeCommand*> _groupCommands;
    QList<Track::PitchChange*> _group;

    // QUndoCommand interface
public:
    void undo();
    void redo();
    void undoNoUpdate();
    void redoNoUpdate();

    // QUndoCommand interface
public:
    int id() const;
    bool mergeWith(const QUndoCommand *other);
};

#endif // EDITPITCHCHANGECOMMAND_H
