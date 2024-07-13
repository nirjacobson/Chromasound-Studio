#ifndef LOADMULTITRACKMIDICOMMAND_H
#define LOADMULTITRACKMIDICOMMAND_H

#include <QUndoCommand>

#include "commands/composite/addtrackcommand.h"

#include "formats/midi/midi.h"
#include "formats/midi/midifile.h"

class MainWindow;

class LoadMultiTrackMIDICommand : public QUndoCommand
{
public:
    LoadMultiTrackMIDICommand(MainWindow* window, const QString& path);
    ~LoadMultiTrackMIDICommand();

    // QUndoCommand interface
public:
    void undo();
    void redo();

private:
    MainWindow* _mainWindow;
    QString _midiFilePath;

    QList<AddTrackCommand*> _addTrackCommands;
};

#endif // LOADMULTITRACKMIDICOMMAND_H
