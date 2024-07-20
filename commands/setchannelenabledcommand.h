#ifndef SETCHANNELENABLEDCOMMAND_H
#define SETCHANNELENABLEDCOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class SetChannelEnabledCommand : public QUndoCommand
{
public:
    SetChannelEnabledCommand(MainWindow* window, Channel& channel, const bool on, const bool merge = false);

private:
    MainWindow* _mainWindow;
    QList<Channel*> _channels;
    QList<bool> _onBefore;
    QList<bool> _onAfter;
    bool _merge;

    // QUndoCommand interface
public:
    void undo();
    void redo();

    // QUndoCommand interface
public:
    int id() const;
    bool mergeWith(const QUndoCommand* other);
};

#endif // SETCHANNELENABLEDCOMMAND_H
