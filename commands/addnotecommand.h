#ifndef ADDNOTECOMMAND_H
#define ADDNOTECOMMAND_H

#include <QUndoCommand>
#include <QWidget>

#include "../project/track.h"
#include "../project/note.h"

class MainWindow;

class AddNoteCommand : public QUndoCommand
{
public:
    AddNoteCommand(MainWindow* window, Track& track, const float time, const Note& note);

private:
    MainWindow* _mainWindow;

    Track& _track;
    float _time;
    Note _note;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // ADDNOTECOMMAND_H
