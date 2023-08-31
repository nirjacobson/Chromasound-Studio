#ifndef TRACK_H
#define TRACK_H

#include <QMap>
#include <QList>

#include "note.h"
#include "ganttwidget/ganttitem.h"

class Track
{
    public:
        Track();
        ~Track();

        class Item : public GanttItem {
            public:
                Item(const float time, const Note& note);

                float time() const;
                const Note& note() const;

                bool operator==(const Track::Item& item);

            private:
                float _time;
                Note _note;

                // GanttItem interface
            public:
                long row() const;
                float duration() const;
                void setRow(const long row);
                void setTime(const float time);
                void setDuration(const float duration);
        };

        QList<Item*>& items();
        const QList<Item*>& items() const;

        float length() const;

        void addItem(const float time, const Note& note);
        void removeItem(const float time, const int key);

        void usePianoRoll();
        bool doesUsePianoRoll() const;

    private:
        QList<Item*> _items;
        bool _usePianoRoll;
};

#endif // TRACK_H
