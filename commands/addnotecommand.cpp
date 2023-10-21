#include "addnotecommand.h"

#include "mainwindow.h"

AddNoteCommand::AddNoteCommand(MainWindow* window, Track& track, const float time, const Note& note)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _note(note)
{
    setText("add note");
}

void AddNoteCommand::undo()
{
    _track.removeItem(_time, _note.key());

    _mainWindow->doUpdate();
}

void AddNoteCommand::redo()
{
    _track.addItem(_time, _note);

    _mainWindow->doUpdate();
}
