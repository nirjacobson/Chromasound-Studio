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
    for (auto it = _tracks.begin(); it != _tracks.end(); ++it) {
        if (it->items().isEmpty()) {
            continue;
        }

        float end = it->length();
        if (end > lastEnd) {
            lastEnd = end;
        }
    }
    return lastEnd;
}

QList<int> Pattern::activeTracksAtTime(const float time) const
{
    QList<int> result;

    for (auto it = _tracks.begin(); it != _tracks.end(); ++it) {
        if (std::find_if(it->items().begin(), it->items().end(), [=](const Track::Item* item){
            return (item->time() <= time) && ((item->time() + item->duration()) >= time);
        }) != it->items().end()) {
            result.append(it.key());
        }
    }

    return result;
}
