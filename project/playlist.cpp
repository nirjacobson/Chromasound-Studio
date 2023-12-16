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

    for (Playlist::LFOChange* change : _lfoChanges)
        delete change;
}

const QList<Playlist::Item*>& Playlist::items() const
{
    return _items;
}

void Playlist::addItem(const float time, const int pattern)
{
    Playlist::Item item(_project, time, pattern);
    if (std::find_if(_items.begin(), _items.end(), [=](Playlist::Item* const pitem) {
    return *pitem == item;
}) == _items.end()) {
        _items.append(new Playlist::Item(_project, time, pattern));
    }
}

void Playlist::addItems(const QList<Item*>& items)
{
    _items.append(items);
}

void Playlist::removeItems(const QList<Item*>& items)
{
    _items.erase(std::remove_if(_items.begin(), _items.end(), [&](const Playlist::Item* item) {
        return items.contains(item);
    }), _items.end());
}

void Playlist::removeItem(const float time, const int pattern)
{
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Playlist::Item* const item) {
        return (item->pattern() == pattern) && (item->time() <= time) && (time < (item->time() + _project->getPatternBarLength(item->pattern())));
    });
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
    while ((it = std::find_if(it, _items.end(), [=](const Playlist::Item* item) {
    return (item->time() <= time) && ((item->time() + _project->getPatternBarLength(item->pattern())) >= time);
    })) != _items.end()) {
        result.insert((*it)->pattern(), (*it)->time());
        it++;
    }

    return result;
}


Playlist& Playlist::operator=(Playlist&& o)
{
    _loopOffset = o._loopOffset;
    _items = o._items;
    _lfoChanges = o._lfoChanges;

    o._items.clear();
    o._lfoChanges.clear();

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

const QList<Playlist::LFOChange*>& Playlist::lfoChanges() const
{
    return _lfoChanges;
}

QList<Playlist::LFOChange*>& Playlist::lfoChanges()
{
    return _lfoChanges;
}

Playlist::LFOChange* Playlist::addLFOChange(const float time, const int mode)
{
    LFOChange* ret = nullptr;

    if (std::find_if(_lfoChanges.begin(), _lfoChanges.end(), [=](LFOChange* const lc) {
    return lc->time() == time;
    }) == _lfoChanges.end()) {
        ret = new LFOChange(time, mode);
        _lfoChanges.append(ret);
    }

    return ret;
}

void Playlist::removeLFOChange(const LFOChange* lc, const bool keep)
{
    if (_lfoChanges.removeAll(lc) > 0) {
        if (!keep) {
            delete lc;
        }
    }
}

Playlist::Playlist()
{

}

Playlist::LFOChange::LFOChange(const float time, const int mode)
    : _time(time)
    , _mode(mode)
{

}

float Playlist::LFOChange::time() const
{
    return _time;
}

QString Playlist::LFOChange::name() const
{
    switch (_mode) {
        case 0:
            return "LFO: Off";
        case 1:
            return "LFO: 3.98 Hz";
        case 2:
            return "LFO: 5.56 Hz";
        case 3:
            return "LFO: 6.02 Hz";
        case 4:
            return "LFO: 6.37 Hz";
        case 5:
            return "LFO: 6.88 Hz";
        case 6:
            return "LFO: 9.63 Hz";
        case 7:
            return "LFO: 48.1 Hz";
        case 8:
            return "LFO: 72.2 Hz";
    }

    return "LFO change";
}

int Playlist::LFOChange::mode() const
{
    return _mode;
}

void Playlist::LFOChange::setMode(const int mode)
{
    _mode = mode;
}

Playlist::LFOChange::LFOChange()
    : _time(0)
    , _mode(0)
{

}
