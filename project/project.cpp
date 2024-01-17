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
    , _lfoMode(0)
    , _ssgEnvelopeFreq(0)
    , _ssgNoiseFreq(0)
    , _opllType(OPLL::Type::STANDARD)
{
    addChannel();
    addChannel();
    addChannel();
    addChannel();
    _channels.last().setType(Channel::Type::NOISE);
}

Project::~Project()
{
    for (Pattern* p : _patterns) {
        delete p;
    }
}

int Project::channelCount() const
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

int Project::getPatternBarLength(const int idx) const
{
    return getPatternBarLength(getPattern(idx));
}

int Project::getPatternBarLength(const Pattern& pattern) const
{
    return qCeil(pattern.getLength() / _beatsPerBar) * _beatsPerBar;
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

int Project::lfoMode() const
{
    return _lfoMode;
}

void Project::setLFOMode(const int mode)
{
    _lfoMode = mode;
}

const SSGEnvelopeSettings& Project::ssgEnvelopeShape() const
{
    return _ssgEnvelopeSettings;
}

SSGEnvelopeSettings& Project::ssgEnvelopeShape()
{
    return _ssgEnvelopeSettings;
}

void Project::setSSGEnvelopeSettings(const SSGEnvelopeSettings& settings)
{
    _ssgEnvelopeSettings = settings;
}

int Project::ssgEnvelopeFrequency() const
{
    return _ssgEnvelopeFreq;
}

void Project::setSSGEnvelopeFrequency(const int freq)
{
    _ssgEnvelopeFreq = freq;
}

int Project::ssgNoiseFrequency() const
{
    return _ssgNoiseFreq;
}

void Project::setSSGNoiseFrequency(const int freq)
{
    _ssgNoiseFreq = freq;
}

const QList<Pattern*>& Project::patterns() const
{
    return _patterns;
}

const QList<Channel>& Project::channels() const
{
    return _channels;
}

void Project::addChannel()
{
    _channels.append(Channel(QString("Channel %1").arg(_channels.size()+1)));
}

void Project::addChannel(const int index, const Channel& channel)
{
    _channels.insert(index, channel);
}

int Project::indexOfChannel(const Channel& channel)
{
    for (int i = 0; i < _channels.size(); i++) {
        if (&channel == &_channels[i]) {
            return i;
        }
    }

    return -1;
}

bool Project::hasPCM() const
{
    for (const Channel& channel : _channels) {
        if (channel.type() == Channel::Type::PCM) {
            return true;
        }
    }
    return false;
}

quint32 Project::pcmOffset(const QString& path) const
{
    QList<QString> paths;

    for (const Channel& channel : _channels) {
        if (channel.type() == Channel::Type::PCM) {
            paths.append(dynamic_cast<const PCMChannelSettings&>(channel.settings()).path());
        }
    }

    std::sort(paths.begin(), paths.end(), [](const QString& a, const QString& b) {
        return QFileInfo(QFile(a)).size() < QFileInfo(QFile(b)).size();
    });

    quint32 offset = 0;
    for (const QString& pcmPath : paths) {
        if (pcmPath == path) {
            return offset;
        }
        offset += QFileInfo(QFile(pcmPath)).size();
    }

    return offset;
}

QByteArray Project::pcm() const
{
    QByteArray result;

    QList<QString> paths;
    for (const Channel& channel : _channels) {
        if (channel.type() == Channel::Type::PCM) {
            paths.append(dynamic_cast<const PCMChannelSettings&>(channel.settings()).path());
        }
    }

    std::sort(paths.begin(), paths.end(), [](const QString& a, const QString& b) {
        return QFileInfo(QFile(a)).size() < QFileInfo(QFile(b)).size();
    });

    for (const QString& pcmPath : paths) {
        QFile pcmFile(pcmPath);
        pcmFile.open(QIODevice::ReadOnly);
        result.append(pcmFile.readAll());
        pcmFile.close();
    }

    return result;
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

    _lfoMode = src._lfoMode;
    _ssgEnvelopeSettings = src._ssgEnvelopeSettings;
    _ssgEnvelopeFreq = src._ssgEnvelopeFreq;
    _ssgNoiseFreq = src._ssgNoiseFreq;
    _userTone = src._userTone;

    _info = src._info;

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

    _lfoMode = o._lfoMode;
    _ssgEnvelopeSettings = o._ssgEnvelopeSettings;
    _ssgEnvelopeFreq = o._ssgEnvelopeFreq;
    _ssgNoiseFreq = o._ssgNoiseFreq;
    _userTone = o._userTone;

    _info = o._info;
}

Project::Info& Project::info()
{
    return _info;
}

const Project::Info& Project::info() const
{
    return _info;
}

FM2Settings& Project::userTone()
{
    return _userTone;
}

const FM2Settings& Project::userTone() const
{
    return _userTone;
}

OPLL::Type Project::opllType() const
{
    return _opllType;
}

void Project::setOpllType(const OPLL::Type type)
{
    _opllType = type;
}

bool Project::usesOPL() const
{
    for (int i = 0; i < _channels.size(); i++) {
        if (_channels[i].type() == Channel::Type::MELODY || _channels[i].type() == Channel::Type::RHYTHM) {
            return true;
        }
    }

    return false;
}

bool Project::usesRhythm() const
{
    for (int i = 0; i < _channels.size(); i++) {
        if (_channels[i].type() == Channel::Type::RHYTHM) {
            return true;
        }
    }

    return false;
}

void Project::swapChannels(const int idxa, const int idxb)
{
    for (Pattern* pat : _patterns) {
        pat->swapChannels(idxa, idxb);
    }

    _channels.swapItemsAt(idxa, idxb);
}

const QString& Project::Info::title() const
{
    return _title;
}

const QString& Project::Info::game() const
{
    return _game;
}

const QString& Project::Info::author() const
{
    return _author;
}

const QDate& Project::Info::releaseDate() const
{
    return _releaseDate;
}

const QString& Project::Info::notes() const
{
    return _notes;
}

void Project::Info::setTitle(const QString& title)
{
    _title = title;
}

void Project::Info::setGame(const QString& game)
{
    _game = game;
}

void Project::Info::setAuthor(const QString& author)
{
    _author = author;
}

void Project::Info::setReleaseDate(const QDate& date)
{
    _releaseDate = date;
}

void Project::Info::setNotes(const QString& notes)
{
    _notes = notes;
}
