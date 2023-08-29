#include "track.h"

Track::Track()
    : _usePianoRoll(false)
{

}

const QList<Track::Item>& Track::items() const
{
    return _items;
}

void Track::addItem(const float time, const Note& note)
{
    _items.append(Item(time, note));
}

void Track::removeItems(const float time)
{
    auto it = std::remove_if(_items.begin(), _items.end(), [=](const Track::Item& item){ return (item.time() <= time) && ((item.time() + item.note().duration()) >= time); });
    _items.erase(it, _items.end());
}

void Track::usePianoRoll()
{
    _usePianoRoll = true;
}

bool Track::doesUsePianoRoll() const
{
    return _usePianoRoll;
}

Track::Item::Item(const float time, const Note& note)
    : _time(time)
    , _note(note)
{

}

float Track::Item::time() const
{
    return _time;
}

const Note& Track::Item::note() const
{
    return _note;
}

bool Track::Item::operator==(const Item& item)
{
    return _time == item.time() && _note == item.note();
}
