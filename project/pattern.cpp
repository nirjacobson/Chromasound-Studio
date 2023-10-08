#include "pattern.h"

Pattern::Pattern()
{

}

Pattern::~Pattern()
{
    for (auto it = _tracks.begin(); it != _tracks.end(); ++it) {
        delete it.value();
    }
}

Pattern::Pattern(Pattern&& o)
{
    _tracks = o._tracks;

    o._tracks.clear();
}

Pattern& Pattern::operator=(Pattern&& o)
{
    _tracks = o._tracks;

    o._tracks.clear();

    return *this;
}

bool Pattern::hasTrack(const int idx) const
{
    return _tracks.contains(idx);
}

const Track& Pattern::getTrack(const int idx) const
{
    return **_tracks.constFind(idx);
}

Track& Pattern::getTrack(const int idx)
{
    if (!_tracks[idx])
        _tracks[idx] = new Track;

    return *_tracks[idx];
}

Track& Pattern::addTrack(const int idx)
{
    _tracks[idx] = new Track;
    return *_tracks[idx];
}

const QMap<int, Track*> Pattern::tracks() const
{
    return _tracks;
}

void Pattern::removeChannel(const int idx)
{
    _tracks.remove(idx);

    QList<int> toDecrement;

    for (auto it = _tracks.begin(); it != _tracks.end(); ++it) {
        if (it.key() > idx) {
            toDecrement.append(it.key());
        }
    }

    std::sort(toDecrement.begin(), toDecrement.end());

    for (const int i : toDecrement) {
        _tracks[i-1] = _tracks[i];
        _tracks.remove(i);
    }
}

float Pattern::getLength() const
{
    float lastEnd = 0;
    for (auto it = _tracks.begin(); it != _tracks.end(); ++it) {
        if (it.value()->items().isEmpty()) {
            continue;
        }

        float end = it.value()->length();
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
        if (std::find_if(it.value()->items().begin(), it.value()->items().end(), [=](const Track::Item* item){
            return (item->time() <= time) && ((item->time() + item->duration()) >= time);
        }) != it.value()->items().end()) {
            result.append(it.key());
        }
    }

    return result;
}

void Pattern::swapChannels(const int idxa, const int idxb)
{
    bool hasA = hasTrack(idxa);
    bool hasB = hasTrack(idxb);

    Track* trackB = _tracks[idxb];

    if (hasA)
        _tracks[idxb] = _tracks[idxa];

    if (hasB)
        _tracks[idxa] = trackB;

    if (!hasB) {
        _tracks.remove(idxa);
    }

    if (!hasA) {
        _tracks.remove(idxb);
    }
}
