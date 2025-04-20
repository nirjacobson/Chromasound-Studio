#ifndef SETCHANNELPITCHRANGECOMMAND_H
#define SETCHANNELPITCHRANGECOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class SetChannelPitchRangeCommand : public QUndoCommand
{
public:
    SetChannelPitchRangeCommand(MainWindow* window, Channel& channel, const int pitchRange);

private:
    MainWindow* _mainWindow;
    Channel& _channel;
    int _originalPitchRange;
    int _newPitchRange;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETCHANNELPITCHRANGECOMMAND_H
