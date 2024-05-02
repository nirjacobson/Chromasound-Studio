#ifndef DELETEPATTERNCOMMAND_H
#define DELETEPATTERNCOMMAND_H

#include <QUndoCommand>

#include "project/pattern.h"
#include "project/playlist.h"

class MainWindow;

class DeletePatternCommand : public QUndoCommand
{
public:
    DeletePatternCommand(MainWindow* window, const int index);
    ~DeletePatternCommand();

private:
    MainWindow* _mainWindow;
    int _index;
    Pattern* _pattern;
    QList<Playlist::Item*> _items;

    // QUndoCommand interface
public:
    void undo();
    void redo();
};

#endif // DELETEPATTERNCOMMAND_H
