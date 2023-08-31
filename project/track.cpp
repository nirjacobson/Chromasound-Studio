#include "track.h"

Track::Track()
    : _usePianoRoll(false)
{

}

Track::~Track()
{
    for (Item* item : _items)
        delete item;
}

QList<Track::Item*>& Track::items()
{
    return _items;
}

const QList<Track::Item*>& Track::items() const
{
    return _items;
}

float Track::length() const
{
    float end = 0;
    for (auto it = _items.begin(); it != _items.end(); ++it) {
        float thisEnd = (*it)->time() + (*it)->note().duration();
        if (thisEnd > end) {
            end = thisEnd;
        }
    }

    return end;
}

void Track::addItem(const float time, const Note& note)
{
    Item item(time, note);
    if (std::find_if(_items.begin(), _items.end(), [=](Item* const noteItem){ return *noteItem == item; }) == _items.end()) {
        _items.append(new Item(time, note));
    }
}

void Track::removeItem(const float time, const int key)
{
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Item* const item){ return (item->note().key() == key) && (item->time() <= time) && ((item->time() + item->note().duration()) >= time); });
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

long Track::Item::row() const
{
    return _note.key();
}

float Track::Item::duration() const
{
    return _note.duration();
}

void Track::Item::setRow(const long row)
{
    _note.setKey(row);
}

void Track::Item::setTime(const float time)
{
    _time = time;
}

void Track::Item::setDuration(const float duration)
{
    _note.setDuration(duration);
}
