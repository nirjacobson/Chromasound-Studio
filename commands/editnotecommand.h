#ifndef EDITNOTECOMMAND_H
#define EDITNOTECOMMAND_H

#include <QUndoCommand>

#include "project/track.h"

class MainWindow;

class EditNoteCommand : public QUndoCommand
{
    public:
    EditNoteCommand(MainWindow* window, Track::Item* item, const float toTime, const Note& note, const bool group);

    private:
        MainWindow* _mainWindow;

        float _toTime;
        Note _note;

        Track::Item* _item;
        float _fromTime;
        Note _fromNote;

        bool _group;
        // QUndoCommand interface
    public:
        void undo();
        void redo();
        void undoNoUpdate();
        void redoNoUpdate();
        int id() const;
        bool mergeWith(const QUndoCommand* other);
};

#endif // EDITNOTECOMMAND_H
