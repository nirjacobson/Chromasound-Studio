#ifndef PATTERN_H
#define PATTERN_H

#include <QMap>
#include <QtMath>
#include <QList>

#include "track.h"

class Pattern
{
        friend class BSON;

    public:
        Pattern();
        ~Pattern();

        Pattern(Pattern&& o);
        Pattern& operator=(Pattern&& o);

        bool hasTrack(const int idx) const;
        const Track& getTrack(const int idx) const;
        Track& getTrack(const int idx);
        Track& addTrack(const int idx);
        const QMap<int, Track*> tracks() const;

        void removeChannel(const int idx);

        float getLength() const;

        QList<int> activeTracksAtTime(const float time) const;

        void swapChannels(const int idxa, const int idxb);

    private:
        QMap<int, Track*> _tracks;
};

#endif // PATTERN_H
