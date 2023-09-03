#ifndef PATTERN_H
#define PATTERN_H

#include <QMap>
#include <QtMath>

#include "track.h"

class Pattern
{
    public:
        Pattern();

        bool hasTrack(const int idx) const;
        const Track& getTrack(const int idx) const;
        Track& getTrack(const int idx);
        Track& addTrack(const int idx);
        float getLength() const;

        QList<int> activeTracksAtTime(const float time) const;

    private:
        QMap<int, Track> _tracks;
};

#endif // PATTERN_H
