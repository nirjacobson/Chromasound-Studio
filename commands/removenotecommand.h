#ifndef REMOVENOTECOMMAND_H
#define REMOVENOTECOMMAND_H

#include <QUndoCommand>

#include "../project/track.h"
#include "../project/note.h"

class MainWindow;

class RemoveNoteCommand : public QUndoCommand
{
public:
    RemoveNoteCommand(MainWindow* window, Track& track, const float time, const Note& note);

private:
    MainWindow* _mainWindow;

    Track& _track;
    float _time;
    float _originalTime;
    Note _note;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // REMOVENOTECOMMAND_H
