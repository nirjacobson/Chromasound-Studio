#include "editprojectnoisefrequencycommand.h"
#include "mainwindow.h"

EditProjectNoiseFrequencyCommand::EditProjectNoiseFrequencyCommand(MainWindow* window, Project& project, const int newFreq)
    : _mainWindow(window)
    , _project(project)
    , _oldFreq(project.ssgNoiseFrequency())
    , _newFreq(newFreq)
{
    setText("edit SSG noise frequency");
}

void EditProjectNoiseFrequencyCommand::undo()
{
    _project.setSSGNoiseFrequency(_oldFreq);

    _mainWindow->doUpdate();
}

void EditProjectNoiseFrequencyCommand::redo()
{
    _project.setSSGNoiseFrequency(_newFreq);

    _mainWindow->doUpdate();
}
