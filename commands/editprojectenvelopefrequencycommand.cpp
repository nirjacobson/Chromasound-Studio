#include "editprojectenvelopefrequencycommand.h"
#include "mainwindow.h"

EditProjectEnvelopeFrequencyCommand::EditProjectEnvelopeFrequencyCommand(MainWindow* window, Project& project, const int newFreq)
    : _mainWindow(window)
    , _project(project)
    , _oldFreq(project.ssgEnvelopeFrequency())
    , _newFreq(newFreq)
{
    setText("edit SSG envelope frequency");
}

void EditProjectEnvelopeFrequencyCommand::undo()
{
    _project.setSSGEnvelopeFrequency(_oldFreq);

    _mainWindow->doUpdate();
}

void EditProjectEnvelopeFrequencyCommand::redo()
{
    _project.setSSGEnvelopeFrequency(_newFreq);

    _mainWindow->doUpdate();
}
