#include "playlist.h"
#include "project.h"

Playlist::Item::Item(Project* const project, const float time, const int pattern)
    : _project(project)
    , _time(time)
    , _pattern(pattern)
{

}

float Playlist::Item::time() const
{
    return _time;
}

int Playlist::Item::pattern() const
{
    return _pattern;
}

bool Playlist::Item::operator==(const Playlist::Item& item)
{
    return _time == item.time() && _pattern == item.pattern();
}

Playlist::Item::Item()
{

}

long Playlist::Item::row() const
{
    return _pattern;
}

float Playlist::Item::duration() const
{
    return _project->getPatternBarLength(_pattern);
}

int Playlist::Item::velocity() const
{
    // Not supported
    return 0.0f;
}

void Playlist::Item::setRow(const long row)
{
    _pattern = row;
}

void Playlist::Item::setTime(const float time)
{
    _time = time;
}

void Playlist::Item::setDuration(const float)
{
    // Not supported
}

void Playlist::Item::setVelocity(const int)
{
    // Not supported
}

Playlist::Playlist(Project* const project)
    : _loopOffset(-1)
{
    _project = project;
}

Playlist::Playlist(Playlist&& o)
{
    _loopOffset = o._loopOffset;
    _items = o._items;

    o._items.clear();

    for (Item* item : _items) {
        item->_project = _project;
    }
}

Playlist::~Playlist()
{
    for (Playlist::Item* item : _items)
        delete item;
}

const QList<Playlist::Item*>& Playlist::items() const
{
    return _items;
}

void Playlist::addItem(const float time, const int pattern)
{
    Playlist::Item item(_project, time, pattern);
    if (std::find_if(_items.begin(), _items.end(), [=](Playlist::Item* const pitem){ return *pitem == item; }) == _items.end()) {
        _items.append(new Playlist::Item(_project, time, pattern));
    }
}

void Playlist::addItems(const QList<Item*>& items)
{
    _items.append(items);
}

void Playlist::removeItems(const QList<Item*>& items)
{
    _items.erase(std::remove_if(_items.begin(), _items.end(), [&](const Playlist::Item* item){ return items.contains(item); }), _items.end());
}

void Playlist::removeItem(const float time, const int pattern)
{
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Playlist::Item* const item){ return (item->pattern() == pattern) && (item->time() <= time) && ((item->time() + _project->getPatternBarLength(item->pattern())) >= time); });
    _items.erase(it, _items.end());
}

float Playlist::getLength() const
{
    float end = 0;
    for (const Playlist::Item* item : _items) {
        float thisEnd = item->time() + _project->getPatternBarLength(item->pattern());
        if (thisEnd > end) {
            end = thisEnd;
        }
    }

    return end;
}

QMap<int, float> Playlist::activePatternsAtTime(const float time) const
{
    QMap<int, float> result;

    QList<Playlist::Item*>::ConstIterator it = _items.begin();
    while ((it = std::find_if(it, _items.end(), [=](const Playlist::Item* item){ return (item->time() <= time) && ((item->time() + _project->getPatternBarLength(item->pattern())) >= time); })) != _items.end()) {
        result.insert((*it)->pattern(), (*it)->time());
        it++;
    }

    return result;
}


Playlist& Playlist::operator=(Playlist&& o)
{
    _loopOffset = o._loopOffset;
    _items = o._items;

    o._items.clear();

    for (Item* item : _items) {
        item->_project = _project;
    }

    return *this;
}

bool Playlist::doesLoop() const
{
    return _loopOffset >= 0;
}

float Playlist::loopOffset() const
{
    return _loopOffset;
}

int Playlist::loopOffsetSamples() const
{
    return _loopOffset / _project->tempo() * 60 * 44100;
}

void Playlist::setLoopOffset(const float offset)
{
    _loopOffset = offset;
}

Playlist::Playlist()
{

}
