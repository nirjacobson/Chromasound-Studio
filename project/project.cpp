#include "project.h"

Project::Project()
    : _frontPattern(0)
    , _playlist(this)
    , _tempo(120)
    , _beatsPerBar(4)
{
    addChannel();
    addChannel();
    addChannel();
    addChannel();
}

Project::~Project()
{
    for (Pattern* p : _patterns) {
        delete p;
    }
}

int Project::channels() const
{
    return _channels.size();
}

Channel& Project::getChannel(const int idx)
{
    return _channels[idx];
}

void Project::removeChannel(const int index)
{
    for (Pattern* pat : _patterns) {
        pat->removeChannel(index);
    }

    _channels.removeAt(index);
}

Pattern& Project::getPattern(const int idx)
{
    if (idx >= _patterns.size()) {
        for (int i = _patterns.size(); i <= idx; i++) {
            _patterns.append(new Pattern);
        }
    }

    return *_patterns[idx];
}

const Pattern& Project::getPattern(const int idx) const
{
    return *_patterns[idx];
}

int Project::frontPattern() const
{
    return _frontPattern;
}

void Project::setFrontPattern(const int idx)
{
    _frontPattern = idx;
}

Pattern& Project::getFrontPattern()
{
    return getPattern(_frontPattern);
}

const Pattern& Project::getFrontPattern() const
{
    return getPattern(_frontPattern);
}

Project::Playlist& Project::playlist()
{
    return _playlist;
}

float Project::getLength() const
{
    return _playlist.getLength();
}

int Project::tempo() const
{
    return _tempo;
}

void Project::setTempo(const int tempo)
{
    _tempo = tempo;
}

int Project::beatsPerBar() const
{
    return _beatsPerBar;
}

void Project::setBeatsPerBar(const int beats)
{
    _beatsPerBar = beats;
}

const QList<Pattern*>& Project::patterns() const
{
    return _patterns;
}

void Project::addChannel()
{
    _channels.append(Channel());
}

Project& Project::operator=(Project&& src)
{
    _channels = src._channels;
    _patterns = src._patterns;
    _frontPattern = src._frontPattern;

    src._patterns.clear();

    _playlist._project = this;
    _playlist = std::move(src._playlist);

    _tempo = src._tempo;
    _beatsPerBar = src._beatsPerBar;

    return *this;
}

Project::Project(Project&& o)
{
    _channels = o._channels;
    _patterns = o._patterns;
    _frontPattern = o._frontPattern;

    o._patterns.clear();

    _playlist._project = this;
    _playlist = std::move(o._playlist);

    _tempo = o._tempo;
    _beatsPerBar = o._beatsPerBar;
}

Project::Playlist::Item::Item(Project* const project, const float time, const int pattern)
    : _project(project)
    , _time(time)
    , _pattern(pattern)
{

}

float Project::Playlist::Item::time() const
{
    return _time;
}

int Project::Playlist::Item::pattern() const
{
    return _pattern;
}

bool Project::Playlist::Item::operator==(const Playlist::Item& item)
{
    return _time == item.time() && _pattern == item.pattern();
}

Project::Playlist::Item::Item()
{

}

long Project::Playlist::Item::row() const
{
    return _pattern;
}

float Project::Playlist::Item::duration() const
{
    return _project->patterns()[_pattern]->getLength();
}

int Project::Playlist::Item::velocity() const
{
    // Not supported
    return 0.0f;
}

void Project::Playlist::Item::setRow(const long row)
{
    _pattern = row;
}

void Project::Playlist::Item::setTime(const float time)
{
    _time = time;
}

void Project::Playlist::Item::setDuration(const float)
{
    // Not supported
}

void Project::Playlist::Item::setVelocity(const int)
{
    // Not supported
}

Project::Playlist::Playlist(Project* const project)
{
    _project = project;
}

Project::Playlist::Playlist(Playlist&& o)
{
    _items = o._items;

    o._items.clear();

    for (Item* item : _items) {
        item->_project = _project;
    }
}

Project::Playlist::~Playlist()
{
    for (Playlist::Item* item : _items)
        delete item;
}

void Project::Playlist::addItem(const float time, const int pattern)
{
    Playlist::Item item(_project, time, pattern);
    if (std::find_if(_items.begin(), _items.end(), [=](Playlist::Item* const pitem){ return *pitem == item; }) == _items.end()) {
        _items.append(new Project::Playlist::Item(_project, time, pattern));
    }
}

void Project::Playlist::removeItem(const float time, const int pattern)
{
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Playlist::Item* const item){ return (item->pattern() == pattern) && (item->time() <= time) && ((item->time() + _project->patterns()[item->pattern()]->getLength()) >= time); });
    _items.erase(it, _items.end());
}

float Project::Playlist::getLength() const
{
    float end = 0;
    for (const Playlist::Item* item : _items) {
        float thisEnd = item->time() + _project->_patterns[item->pattern()]->getLength();
        if (thisEnd > end) {
            end = thisEnd;
        }
    }

    return end;
}

QMap<int, float> Project::Playlist::activePatternsAtTime(const float time) const
{
    QMap<int, float> result;

    QList<Playlist::Item*>::ConstIterator it = _items.begin();
    while ((it = std::find_if(it, _items.end(), [=](const Playlist::Item* item){ return (item->time() <= time) && ((item->time() + _project->_patterns[item->pattern()]->getLength()) >= time); })) != _items.end()) {
        result.insert((*it)->pattern(), (*it)->time());
        it++;
    }

    return result;
}


Project::Playlist& Project::Playlist::operator=(Playlist&& o)
{
    _items = o._items;

    o._items.clear();

    for (Item* item : _items) {
        item->_project = _project;
    }

    return *this;
}

Project::Playlist::Playlist()
{

}
