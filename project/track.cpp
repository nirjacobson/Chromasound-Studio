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

Track::Track(Track&& o)
{
    _items = o._items;
    _usePianoRoll = o._usePianoRoll;

    o.items().clear();
}

Track& Track::operator=(Track&& o)
{
    _items = o._items;
    _usePianoRoll = o._usePianoRoll;

    o.items().clear();

    return *this;
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

Track::Item* Track::addItem(const float time, const Note& note)
{
    Item* ret = nullptr;

    Item item(time, note);
    if (std::find_if(_items.begin(), _items.end(), [=](Item* const noteItem){ return *noteItem == item; }) == _items.end()) {
        ret = new Item(time, note);
        _items.append(ret);
    }

    return ret;
}

void Track::addItems(const QList<Item*>& items)
{
    _items.append(items);
}

float Track::removeItem(const float time, const int key)
{
    float tt;
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Item* const item){ return (item->note().key() == key) && (item->time() <= time) && (time < (item->time() + item->note().duration())); });
    if (it != _items.end()) {
        tt = (*it)->time();
    }
    _items.erase(it, _items.end());

    return tt;
}

float Track::removeItem(const float time)
{
    float tt;
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Item* const item){ return (item->time() <= time) && (time < (item->time() + item->note().duration())); });
    if (it != _items.end()) {
        tt = (*it)->time();
    }
    _items.erase(it, _items.end());

    return tt;
}

void Track::removeItems(const QList<Item*>& items)
{
    _items.erase(std::remove_if(_items.begin(), _items.end(), [&](const Track::Item* item){ return items.contains(item); }), _items.end());
}

void Track::usePianoRoll()
{
    _usePianoRoll = true;
}

bool Track::doesUsePianoRoll() const
{
    return _usePianoRoll;
}

void Track::useStepSequencer()
{
    _usePianoRoll = false;
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

Note& Track::Item::note()
{
    return _note;
}

bool Track::Item::operator==(const Item& item)
{
    return _time == item.time() && _note == item.note();
}

Track::Item::Item()
    : _note(0, 0)
{

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

int Track::Item::velocity() const
{
    return _note.velocity();
}

void Track::Item::setVelocity(const int velocity)
{
    _note.setVelocity(velocity);
}
