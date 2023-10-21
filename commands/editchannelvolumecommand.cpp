#include "editchannelvolumecommand.h"
#include "mainwindow.h"

EditChannelVolumeCommand::EditChannelVolumeCommand(MainWindow* window, const int index, const int newVolume)
    : _mainWindow(window)
    , _index(index)
    , _newVolume(newVolume)
    , _originalVolume(_mainWindow->getChannelVolume(index))
{
    setText("edit volume");
}

void EditChannelVolumeCommand::undo()
{
    _mainWindow->setChannelVolume(_index, _originalVolume);
}

void EditChannelVolumeCommand::redo()
{
    _mainWindow->setChannelVolume(_index, _newVolume);
}

int EditChannelVolumeCommand::id() const
{
    return 'V';
}

bool EditChannelVolumeCommand::mergeWith(const QUndoCommand* other)
{
    const EditChannelVolumeCommand* ecvc;
    if ((ecvc = dynamic_cast<const EditChannelVolumeCommand*>(other))) {
        if (ecvc->_index == _index) {
            _newVolume = ecvc->_newVolume;
            return true;
        }
    }

    return false;
}
