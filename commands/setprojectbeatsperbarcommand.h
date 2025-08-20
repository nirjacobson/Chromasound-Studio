#ifndef SETPROJECTBEATSPERBARCOMMAND_H
#define SETPROJECTBEATSPERBARCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class SetProjectBeatsPerBarCommand : public QUndoCommand
{
public:
    SetProjectBeatsPerBarCommand(MainWindow* window, const int beatsBefore, const int beatsAfter);

private:
    MainWindow* _mainWindow;
    int _beatsBefore;
    int _beatsAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPROJECTBEATSPERBARCOMMAND_H
