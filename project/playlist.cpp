#include "playlist.h"
#include "project.h"

QColor Playlist::LFOChange::COLOR(255, 255, 64);
QColor Playlist::NoiseFrequencyChange::COLOR(224, 224, 224);
QColor Playlist::EnvelopeFrequencyChange::COLOR(64, 255, 54);
QColor Playlist::EnvelopeShapeChange::COLOR(128, 128, 255);

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

    _lfoChanges = o._lfoChanges;
    o._lfoChanges.clear();

    _noiseFreqChanges = o._noiseFreqChanges;
    o._noiseFreqChanges.clear();

    _envFreqChanges = o._envFreqChanges;
    o._envFreqChanges.clear();

    _envShapeChanges = o._envShapeChanges;
    o._envShapeChanges.clear();
}

Playlist::~Playlist()
{
    for (Playlist::Item* item : _items)
        delete item;

    for (Playlist::LFOChange* change : _lfoChanges)
        delete change;

    for (Playlist::NoiseFrequencyChange* change : _noiseFreqChanges)
        delete change;

    for (Playlist::EnvelopeFrequencyChange* change : _envFreqChanges)
        delete change;

    for (Playlist::EnvelopeShapeChange* change : _envShapeChanges)
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
    _noiseFreqChanges = o._noiseFreqChanges;
    _envFreqChanges = o._envFreqChanges;
    _envShapeChanges = o._envShapeChanges;

    o._items.clear();
    o._lfoChanges.clear();
    o._noiseFreqChanges.clear();
    o._envFreqChanges.clear();
    o._envShapeChanges.clear();

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

const QList<Playlist::NoiseFrequencyChange*>& Playlist::noiseFrequencyChanges() const
{
    return _noiseFreqChanges;
}

QList<Playlist::NoiseFrequencyChange*>& Playlist::noiseFrequencyChanges()
{
    return _noiseFreqChanges;
}

Playlist::NoiseFrequencyChange* Playlist::addNoiseFrequencyChange(const float time, const int freq)
{
    NoiseFrequencyChange* ret = nullptr;

    if (std::find_if(_noiseFreqChanges.begin(), _noiseFreqChanges.end(), [=](NoiseFrequencyChange* const nfc) {
            return nfc->time() == time;
        }) == _noiseFreqChanges.end()) {
        ret = new NoiseFrequencyChange(time, freq);
        _noiseFreqChanges.append(ret);
    }

    return ret;
}

void Playlist::removeNoiseFrequencyChange(const NoiseFrequencyChange* nfc, const bool keep)
{
    if (_noiseFreqChanges.removeAll(nfc) > 0) {
        if (!keep) {
            delete nfc;
        }
    }
}

const QList<Playlist::EnvelopeFrequencyChange*>& Playlist::envelopeFrequencyChanges() const
{
    return _envFreqChanges;
}

QList<Playlist::EnvelopeFrequencyChange*>& Playlist::envelopeFrequencyChanges()
{
    return _envFreqChanges;
}

Playlist::EnvelopeFrequencyChange* Playlist::addEnvelopeFrequencyChange(const float time, const int freq)
{
    EnvelopeFrequencyChange* ret = nullptr;

    if (std::find_if(_envFreqChanges.begin(), _envFreqChanges.end(), [=](EnvelopeFrequencyChange* const efc) {
            return efc->time() == time;
        }) == _envFreqChanges.end()) {
        ret = new EnvelopeFrequencyChange(time, freq);
        _envFreqChanges.append(ret);
    }

    return ret;
}

void Playlist::removeEnvelopeFrequencyChange(const EnvelopeFrequencyChange* efc, const bool keep)
{
    if (_envFreqChanges.removeAll(efc) > 0) {
        if (!keep) {
            delete efc;
        }
    }
}

const QList<Playlist::EnvelopeShapeChange*>& Playlist::envelopeShapeChanges() const
{
    return _envShapeChanges;
}

QList<Playlist::EnvelopeShapeChange*>& Playlist::envelopeShapeChanges()
{
    return _envShapeChanges;
}

Playlist::EnvelopeShapeChange* Playlist::addEnvelopeShapeChange(const float time, const SSGEnvelopeSettings& shape)
{
    EnvelopeShapeChange* ret = nullptr;

    if (std::find_if(_envShapeChanges.begin(), _envShapeChanges.end(), [=](EnvelopeShapeChange* const efc) {
            return efc->time() == time;
        }) == _envShapeChanges.end()) {
        ret = new EnvelopeShapeChange(time, shape);
        _envShapeChanges.append(ret);
    }

    return ret;
}

void Playlist::removeEnvelopeShapeChange(const EnvelopeShapeChange* esc, const bool keep)
{
    if (_envShapeChanges.removeAll(esc) > 0) {
        if (!keep) {
            delete esc;
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

const QColor& Playlist::LFOChange::color() const
{
        return COLOR;
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

Playlist::NoiseFrequencyChange::NoiseFrequencyChange(const float time, const int freq)
    : _time(time)
    , _freq(freq)
{

}

float Playlist::NoiseFrequencyChange::time() const
{
    return _time;
}

QString Playlist::NoiseFrequencyChange::name() const
{
    return "SSG Noise";
}

const QColor& Playlist::NoiseFrequencyChange::color() const
{
    return COLOR;
}

int Playlist::NoiseFrequencyChange::frequency() const
{
    return _freq;
}

void Playlist::NoiseFrequencyChange::setFrequency(const int freq)
{
    _freq = freq;
}

Playlist::NoiseFrequencyChange::NoiseFrequencyChange()
    : _time(0)
    , _freq(0)
{

}

Playlist::EnvelopeFrequencyChange::EnvelopeFrequencyChange(const float time, const int freq)
    : _time(time)
    , _freq(freq)
{

}

float Playlist::EnvelopeFrequencyChange::time() const
{
    return _time;
}

QString Playlist::EnvelopeFrequencyChange::name() const
{
    return "SSG Envelope (frequency)";
}

const QColor& Playlist::EnvelopeFrequencyChange::color() const
{
    return COLOR;
}

int Playlist::EnvelopeFrequencyChange::frequency() const
{
    return _freq;
}

void Playlist::EnvelopeFrequencyChange::setFrequency(const int freq)
{
    _freq = freq;
}

Playlist::EnvelopeFrequencyChange::EnvelopeFrequencyChange()
    : _time(0)
    , _freq(0)
{

}

Playlist::EnvelopeShapeChange::EnvelopeShapeChange(const float time, const SSGEnvelopeSettings& shape)
    : _time(time)
    , _shape(shape)
{

}

float Playlist::EnvelopeShapeChange::time() const
{
    return _time;
}

QString Playlist::EnvelopeShapeChange::name() const
{
    return "SSG Envelope (shape)";
}

const QColor& Playlist::EnvelopeShapeChange::color() const
{
    return COLOR;
}

const SSGEnvelopeSettings& Playlist::EnvelopeShapeChange::shape() const
{
    return _shape;
}

void Playlist::EnvelopeShapeChange::setShape(const SSGEnvelopeSettings& shape)
{
    _shape = shape;
}

Playlist::EnvelopeShapeChange::EnvelopeShapeChange()
    : _time(0)
{

}
