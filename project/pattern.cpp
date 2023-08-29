#include "pattern.h"

Pattern::Pattern()
{

}

bool Pattern::hasTrack(const int idx) const
{
    return _tracks.contains(idx);
}

const Track& Pattern::getTrack(const int idx) const
{
    return *_tracks.constFind(idx);
}

Track& Pattern::getTrack(const int idx)
{
    return _tracks[idx];
}

Track& Pattern::addTrack(const int idx)
{
    _tracks[idx] = Track();
    return _tracks[idx];
}

float Pattern::getLength() const
{
    float lastEnd = 0;
    QList<Track> tracks = _tracks.values();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        if (it->items().isEmpty()) {
            continue;
        }

        float end = 0;
        for (auto it2 = it->items().begin(); it2 != it->items().end(); ++it2) {
            float thisEnd = it2->time() + it2->note().duration();
            if (thisEnd > end) {
                end = thisEnd;
            }
        }
        if (end > lastEnd) {
            lastEnd = end;
        }
    }
    return lastEnd;
}
