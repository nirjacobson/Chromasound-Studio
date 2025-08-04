#include "setchanneloctaveoffsetcommand.h"
#include "mainwindow.h"

SetChannelOctaveOffsetCommand::SetChannelOctaveOffsetCommand(MainWindow *window, Channel &channel, const int octaveOffset)
    : _mainWindow(window)
    , _channel(channel)
    , _originalOctaveOffset(channel.settings().octaveOffset())
    , _newOctaveOffset(octaveOffset)
{
    setText("set channel octave offset");
}

void SetChannelOctaveOffsetCommand::undo()
{
    _channel.settings().setOctaveOffset(_originalOctaveOffset);

    _mainWindow->doUpdate();
}

void SetChannelOctaveOffsetCommand::redo()
{
    _channel.settings().setOctaveOffset(_newOctaveOffset);

    _mainWindow->doUpdate();
}
