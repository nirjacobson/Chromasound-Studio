#include "project.h"

Project::Project()
    : _defaultPattern(0)
    , _tempo(120)
    , _beatsPerBar(4)
{
    _channels.append(Channel());
    _channels.append(Channel());
    _channels.append(Channel());
    _channels.append(Channel());

    _patterns.append(Pattern());
}

Project::~Project()
{
    for (PlaylistItem* item : _playlist)
        delete item;
}

int Project::channels() const
{
    return _channels.size();
}

Channel& Project::getChannel(const int idx)
{
    return _channels[idx];
}

Pattern& Project::getPattern(const int idx)
{
    if (idx >= _patterns.size()) {
        for (int i = _patterns.size(); i <= idx; i++) {
            _patterns.append(Pattern());
        }
    }

    return _patterns[idx];
}

const Pattern& Project::getPattern(const int idx) const
{
    return _patterns[idx];
}

int Project::defaultPattern() const
{
    return _defaultPattern;
}

void Project::setDefaultPattern(const int idx)
{
    _defaultPattern = idx;
}

Pattern& Project::getDefaultPattern()
{
    return getPattern(_defaultPattern);
}

const Pattern& Project::getDefaultPattern() const
{
    return getPattern(_defaultPattern);
}

QList<Project::PlaylistItem*>& Project::playlist()
{
    return _playlist;
}

void Project::addPlaylistItem(const float time, const int pattern)
{
    PlaylistItem item(this, time, pattern);
    if (std::find_if(_playlist.begin(), _playlist.end(), [=](PlaylistItem* const playlistItem){ return *playlistItem == item; }) == _playlist.end()) {
        _playlist.append(new PlaylistItem(this, time, pattern));
    }
}

void Project::removePlaylistItem(const float time, const int pattern)
{
    auto it = std::remove_if(_playlist.begin(), _playlist.end(), [=](PlaylistItem* const item){ return (item->pattern() == pattern) && (item->time() <= time) && ((item->time() + _patterns[item->pattern()].getLength()) >= time); });
    _playlist.erase(it, _playlist.end());
}

float Project::getLength() const
{
    float end = 0;
    for (const PlaylistItem* item : _playlist) {
        float thisEnd = item->time() + _patterns[item->pattern()].getLength();
        if (thisEnd > end) {
            end = thisEnd;
        }
    }

    return end;
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

const QList<Pattern>& Project::patterns() const
{
    return _patterns;
}

QMap<int, float> Project::activePatternsAtTime(const float time) const
{
    QMap<int, float> result;

    QList<PlaylistItem*>::ConstIterator it = _playlist.begin();
    while ((it = std::find_if(it, _playlist.end(), [=](const PlaylistItem* item){ return (item->time() <= time) && ((item->time() + _patterns[item->pattern()].getLength()) >= time); })) != _playlist.end()) {
        result.insert((*it)->pattern(), (*it)->time());
        it++;
    }

    return result;
}

Project::PlaylistItem::PlaylistItem(Project* const project, const float time, const int pattern)
    : _project(project)
    , _time(time)
    , _pattern(pattern)
{

}

float Project::PlaylistItem::time() const
{
    return _time;
}

int Project::PlaylistItem::pattern() const
{
    return _pattern;
}

bool Project::PlaylistItem::operator==(const PlaylistItem& item)
{
    return _time == item.time() && _pattern == item.pattern();
}

long Project::PlaylistItem::row() const
{
    return _pattern;
}

float Project::PlaylistItem::duration() const
{
    return _project->patterns()[_pattern].getLength();
}

int Project::PlaylistItem::velocity() const
{
    // Not supported
    return 0.0f;
}

void Project::PlaylistItem::setRow(const long row)
{
    _pattern = row;
}

void Project::PlaylistItem::setTime(const float time)
{
    _time = time;
}

void Project::PlaylistItem::setDuration(const float duration)
{
    // Not supported
}

void Project::PlaylistItem::setVelocity(const int velocity)
{
    // Not supported
}
