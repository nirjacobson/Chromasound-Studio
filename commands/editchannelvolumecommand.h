#ifndef EDITCHANNELVOLUMECOMMAND_H
#define EDITCHANNELVOLUMECOMMAND_H

#include <QUndoCommand>

#include "project/channel/channel.h"

class MainWindow;

class EditChannelVolumeCommand : public QUndoCommand
{
    public:
        EditChannelVolumeCommand(MainWindow* window, const int index, const int newVolume);

    private:
        MainWindow* _mainWindow;
        int _index;
        int _originalVolume;
        int _newVolume;

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITCHANNELVOLUMECOMMAND_H
