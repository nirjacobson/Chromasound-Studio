#include "setchannelenabledcommand.h"
#include "mainwindow.h"

SetChannelEnabledCommand::SetChannelEnabledCommand(MainWindow* window, Channel& channel, const bool on, const bool merge)
    : _mainWindow(window)
    , _merge(merge)
{
    _channels.append(&channel);
    _onBefore.append(channel.enabled());
    _onAfter.append(on);
}

void SetChannelEnabledCommand::undo()
{
    for (int i = 0; i < _channels.size(); i++) {
        _channels[i]->setEnabled(_onBefore[i]);
    }

    _mainWindow->doUpdate();
}

void SetChannelEnabledCommand::redo()
{
    for (int i = 0; i < _channels.size(); i++) {
        _channels[i]->setEnabled(_onAfter[i]);
    }

    _mainWindow->doUpdate();
}

int SetChannelEnabledCommand::id() const
{
    return 'E';
}

bool SetChannelEnabledCommand::mergeWith(const QUndoCommand* other)
{
    const SetChannelEnabledCommand* otherComm = dynamic_cast<const SetChannelEnabledCommand*>(other);

    if (otherComm->_merge) {
        _channels.append(otherComm->_channels);
        _onBefore.append(otherComm->_onBefore);
        _onAfter.append(otherComm->_onAfter);

        return true;
    }

    return false;
}
