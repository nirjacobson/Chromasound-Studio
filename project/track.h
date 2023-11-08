#ifndef TRACK_H
#define TRACK_H

#include <QMap>
#include <QList>

#include "note.h"
#include "ganttwidget/ganttitem.h"

class Track
{
        friend class BSON;
        friend class MIDI;

    public:
        Track();
        ~Track();
        Track(Track&& o);

        Track& operator=(Track&& o);

        class Item : public GanttItem {
               friend class BSON;
                friend class MIDI;

            public:
                Item(const float time, const Note& note);

                float time() const;
                const Note& note() const;
                Note& note();

                bool operator==(const Track::Item& item);

            private:
                Item();

                float _time;
                Note _note;

                // GanttItem interface
            public:
                long row() const;
                float duration() const;
                void setRow(const long row);
                void setTime(const float time);
                void setDuration(const float duration);
                int velocity() const;
                void setVelocity(const int velocity);
        };

        QList<Item*>& items();
        const QList<Item*>& items() const;

        float length() const;

        Item* addItem(const float time, const Note& note);
        void addItems(const QList<Item*>& items);
        float removeItem(const float time, const int key);
        float removeItem(const float time);
        void removeItems(const QList<Item*>& items);

        void usePianoRoll();
        bool doesUsePianoRoll() const;

        void useStepSequencer();

    private:
        QList<Item*> _items;
        bool _usePianoRoll;
};

#endif // TRACK_H
