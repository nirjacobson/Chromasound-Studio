#include "removenotecommand.h"
#include "mainwindow.h"

RemoveNoteCommand::RemoveNoteCommand(MainWindow* window, Track& track, const float time, const Note& note)
    : _mainWindow(window)
    , _track(track)
    , _time(time)
    , _note(note)
{
    setText("delete note");
}

void RemoveNoteCommand::undo()
{
    _track.addItem(_originalTime, _note);

    _mainWindow->doUpdate();
}

void RemoveNoteCommand::redo()
{
    _originalTime = _track.removeItem(_time, _note.key());

    _mainWindow->doUpdate();
}
