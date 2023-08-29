#ifndef TRACK_H
#define TRACK_H

#include <QMap>
#include <QList>

#include "note.h"

class Track
{
    public:
        Track();

        class Item {
            public:
                Item(const float time, const Note& note);

                float time() const;
                const Note& note() const;

                bool operator==(const Track::Item& item);

            private:
                float _time;
                Note _note;
        };

        const QList<Item>& items() const;

        void addItem(const float time, const Note& note);
        void removeNote(const float time, const int key);
        void removeItems(const float time);

        void usePianoRoll();
        bool doesUsePianoRoll() const;

    private:
        QList<Item> _items;
        bool _usePianoRoll;
};

#endif // TRACK_H
