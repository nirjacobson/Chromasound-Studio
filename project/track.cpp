#include "track.h"

QColor Track::SettingsChange::COLOR(255, 128, 128);

Track::Track()
    : _usePianoRoll(false)
{

}

Track::~Track()
{
    for (Item* item : _items)
        delete item;

    for (SettingsChange* sc : _settingsChanges)
        delete sc;

    for (PitchChange* pc : _pitchChanges)
        delete pc;
}

Track::Track(Track&& o)
{
    _items = o._items;
    _settingsChanges = o._settingsChanges;
    _pitchChanges = o._pitchChanges;
    _usePianoRoll = o._usePianoRoll;

    o.items().clear();
    o.settingsChanges().clear();
    o.pitchChanges().clear();
}

Track& Track::operator=(Track&& o)
{
    _items = o._items;
    _settingsChanges = o._settingsChanges;
    _pitchChanges = o._pitchChanges;
    _usePianoRoll = o._usePianoRoll;

    o.items().clear();
    o.settingsChanges().clear();
    o.pitchChanges().clear();

    return *this;
}

const QList<Track::PitchChange *> &Track::pitchChanges() const
{
    return _pitchChanges;
}

QList<Track::PitchChange*>& Track::pitchChanges()
{
    return _pitchChanges;
}

QList<Track::Item*>& Track::items()
{
    return _items;
}

const QList<Track::Item*>& Track::items() const
{
    return _items;
}

QList<Track::SettingsChange*>& Track::settingsChanges()
{
    return _settingsChanges;
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
    if (std::find_if(_items.begin(), _items.end(), [=](Item* const noteItem) {
    return *noteItem == item;
}) == _items.end()) {
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
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Item* const item) {
        return (item->note().key() == key) && (item->time() <= time) && (time < (item->time() + item->note().duration()));
    });
    if (it != _items.end()) {
        tt = (*it)->time();
    }
    _items.erase(it, _items.end());

    return tt;
}

float Track::removeItem(const float time)
{
    float tt;
    auto it = std::remove_if(_items.begin(), _items.end(), [=](Item* const item) {
        return (item->time() <= time) && (time < (item->time() + item->note().duration()));
    });
    if (it != _items.end()) {
        tt = (*it)->time();
    }
    _items.erase(it, _items.end());

    return tt;
}

void Track::removeItems(const QList<Item*>& items)
{
    _items.erase(std::remove_if(_items.begin(), _items.end(), [&](const Track::Item* item) {
        return items.contains(item);
    }), _items.end());
}

Track::SettingsChange* Track::addSettingsChange(const float time, ChannelSettings* settings)
{
    SettingsChange* ret = nullptr;

    if (std::find_if(_settingsChanges.begin(), _settingsChanges.end(), [=](SettingsChange* const sc) {
    return sc->time() == time;
    }) == _settingsChanges.end()) {
        ret = new SettingsChange(time, settings);
        _settingsChanges.append(ret);
    }

    return ret;
}

void Track::removeSettingsChange(const SettingsChange* sc, const bool keep)
{
    if (_settingsChanges.removeAll(sc) > 0) {
        if (!keep) {
            delete sc;
        }
    }
}

void Track::addPitchChanges(const QList<Track::PitchChange*>& pitchChanges)
{
    _pitchChanges.append(pitchChanges);
}

void Track::removePitchChange(const PitchChange *pc, const bool keep)
{
    if (_pitchChanges.removeAll(pc) > 0) {
        if (!keep) {
            delete pc;
        }
    }
}

void Track::removePitchChanges(const QList<PitchChange *> &changes)
{
    _pitchChanges.erase(std::remove_if(_pitchChanges.begin(), _pitchChanges.end(), [&](const Track::PitchChange* change) {
         return changes.contains(change);
     }), _pitchChanges.end());
}

Track::PitchChange* Track::addPitchChange(const float time, const float pitch)
{
    PitchChange* ret = nullptr;

    if (std::find_if(_pitchChanges.begin(), _pitchChanges.end(), [=](PitchChange* const pc) {
            return pc->time() == time;
        }) == _pitchChanges.end()) {
        ret = new PitchChange(time, pitch);
        _pitchChanges.append(ret);
    }

    return ret;
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

Track* Track::copy() const
{
    Track* track = new Track;
    track->_usePianoRoll = _usePianoRoll;

    for (Track::Item* item : _items) {
        track->_items.append(new Track::Item(*item));
    }
    for (Track::SettingsChange* sc : _settingsChanges) {
        track->_settingsChanges.append(new Track::SettingsChange(*sc));
    }

    return track;
}

void Track::clear()
{
    _items.clear();
    _settingsChanges.clear();
}

bool Track::empty() const
{
    return _items.empty() && _settingsChanges.empty();
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

Track::SettingsChange::SettingsChange(const float time, ChannelSettings* settings)
    : _time(time)
    , _settings(settings)
{

}

Track::SettingsChange::~SettingsChange()
{
    delete _settings;
}

float Track::SettingsChange::time() const
{
    return _time;
}

QString Track::SettingsChange::name() const
{
    return "Settings change";
}

const QColor& Track::SettingsChange::color() const
{
    return COLOR;
}

ChannelSettings& Track::SettingsChange::settings()
{
    return *_settings;
}

Track::SettingsChange::SettingsChange()
    : _time(0)
    , _settings(nullptr)
{

}


Track::PitchChange::PitchChange(const float time, const float pitch)
    : _time(time)
    , _pitch(pitch)
{

}

void Track::PitchChange::setTime(const float time)
{
    _time = time;
}

float Track::PitchChange::time() const
{
    return _time;
}

float Track::PitchChange::pitch() const
{
    return _pitch;
}

Track::PitchChange::PitchChange()
    : _time(0)
    , _pitch(0)
{

}
