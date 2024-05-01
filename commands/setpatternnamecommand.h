#ifndef SETPATTERNNAMECOMMAND_H
#define SETPATTERNNAMECOMMAND_H

#include <QUndoCommand>

#include "project/pattern.h"

class MainWindow;

class SetPatternNameCommand : public QUndoCommand
{
public:
    SetPatternNameCommand(MainWindow* window, Pattern& pattern, const QString& name);

private:
    MainWindow* _mainWindow;
    Pattern& _pattern;
    QString _nameBefore;
    QString _nameAfter;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // SETPATTERNNAMECOMMAND_H
