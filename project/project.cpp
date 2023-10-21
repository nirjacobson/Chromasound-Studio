#include "project.h"

Project::PlayMode Project::playMode() const
{
    return _playMode;
}

void Project::setPlayMode(const PlayMode mode)
{
    _playMode = mode;
}

Project::Project()
    : _frontPattern(0)
    , _playMode(PATTERN)
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

const Channel& Project::getChannel(const int idx) const
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

void Project::moveChannelUp(const int idx)
{
    if (idx != 0) {
        swapChannels(idx, idx-1);
    }
}

void Project::moveChannelDown(const int idx)
{
    if (idx != _channels.size()-1) {
        swapChannels(idx, idx+1);
    }
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

int Project::getPatternBarLength(const int idx)
{
    return qCeil(getPattern(idx).getLength() / _beatsPerBar) * _beatsPerBar;
}

Playlist& Project::playlist()
{
    return _playlist;
}

const Playlist& Project::playlist() const
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
    _channels.append(Channel(QString("Channel %1").arg(_channels.size()+1)));
}

void Project::addChannel(const int index, const Channel& channel)
{
    _channels.insert(index, channel);
}

Project& Project::operator=(Project&& src)
{
    _channels = src._channels;
    _patterns = src._patterns;
    _frontPattern = src._frontPattern;
    _playMode = src._playMode;

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
    _playMode = o._playMode;

    o._patterns.clear();

    _playlist._project = this;
    _playlist = std::move(o._playlist);

    _tempo = o._tempo;
    _beatsPerBar = o._beatsPerBar;
}

void Project::swapChannels(const int idxa, const int idxb)
{
    for (Pattern* pat : _patterns) {
        pat->swapChannels(idxa, idxb);
    }

    _channels.swapItemsAt(idxa, idxb);
}
