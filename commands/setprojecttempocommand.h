#ifndef SETPROJECTTEMPOCOMMAND_H
#define SETPROJECTTEMPOCOMMAND_H

#include <QUndoCommand>

class MainWindow;

class SetProjectTempoCommand : public QUndoCommand
{
public:
    SetProjectTempoCommand(MainWindow* window, const int tempoBefore, const int tempoAfter);
private:
    MainWindow* _mainWindow;
    int _tempoBefore;
    int _tempoAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPROJECTTEMPOCOMMAND_H
