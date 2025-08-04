#include "setchannelpitchrangecommand.h"
#include "mainwindow.h"

SetChannelPitchRangeCommand::SetChannelPitchRangeCommand(MainWindow *window, Channel &channel, const int pitchRange)
    : _mainWindow(window)
    , _channel(channel)
    , _originalPitchRange(channel.settings().pitchRange())
    , _newPitchRange(pitchRange)
{
    setText("set channel pitch range");
}

void SetChannelPitchRangeCommand::undo()
{
    _channel.settings().setPitchRange(_originalPitchRange);

    _mainWindow->doUpdate();
}

void SetChannelPitchRangeCommand::redo()
{
    _channel.settings().setPitchRange(_newPitchRange);

    _mainWindow->doUpdate();
}
