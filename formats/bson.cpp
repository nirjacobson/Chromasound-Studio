#include "bson.h"

BSON::BSON()
{

}

QByteArray BSON::encode(const Project& project)
{
    bson_writer_t* writer;
    uint8_t* buf = NULL;
    size_t buflen = 0;
    bson_t* doc;

    writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_writer_begin(writer, &doc);
    fromProject(doc, project);
    bson_writer_end(writer);

    bson_writer_destroy(writer);

    QByteArray ret(reinterpret_cast<const char*>(buf), buflen);

    bson_free(buf);

    return ret;
}

Project BSON::decode(const QString& path)
{
    bson_reader_t* reader;
    const bson_t* b;
    bson_error_t error;

    if (!(reader = bson_reader_new_from_file(path.toStdString().c_str(), &error))) {
        throw  "error decoding file";
    }

    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    Project project = toProject(iter);
    if (!path.startsWith(".")) {
        project._path = path;
    }

    return project;
}

QByteArray BSON::encodePatch(const Settings* settings)
{
    bson_writer_t* writer;
    uint8_t* buf = NULL;
    size_t buflen = 0;
    bson_t* doc;

    writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_t sb = settings->toBSON();
    bson_writer_begin(writer, &doc);
    bson_copy_to_excluding_noinit(&sb, doc, "", NULL);
    bson_writer_end(writer);

    bson_writer_destroy(writer);

    QByteArray ret(reinterpret_cast<const char*>(buf), buflen);

    bson_free(buf);

    return ret;

}

FMChannelSettings* BSON::decodeOPNPatch(const QString& file)
{
    bson_reader_t* reader;
    const bson_t* b;
    bson_error_t error;

    if (!(reader = bson_reader_new_from_file(file.toStdString().c_str(), &error))) {
        throw  "error decoding file";
    }

    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    FMChannelSettings* settings = new FMChannelSettings;
    settings->fromBSON(iter);

    return settings;
}

OPLSettings* BSON::decodeOPLPatch(const QString& file)
{
    bson_reader_t* reader;
    const bson_t* b;
    bson_error_t error;

    if (!(reader = bson_reader_new_from_file(file.toStdString().c_str(), &error))) {
        throw  "error decoding file";
    }

    b = bson_reader_read(reader, NULL);

    bson_iter_t iter;
    bson_iter_init(&iter, b);

    OPLSettings* settings = new OPLSettings;
    settings->fromBSON(iter);

    return settings;
}

void BSON::fromChannel(bson_t* dst, const Channel& channel)
{
    BSON_APPEND_BOOL(dst, "enabled", channel._enabled);
    BSON_APPEND_UTF8(dst, "name", channel._name.toStdString().c_str());
    BSON_APPEND_UTF8(dst, "type", Channel::channelTypeToString(channel._type).toStdString().c_str());

    if (channel._settings) {
        bson_t settings = channel._settings->toBSON();
        BSON_APPEND_DOCUMENT(dst, "settings", &settings);
    }
}

Channel BSON::toChannel(bson_iter_t& b)
{
    Channel c;

    bson_iter_t enabled;
    bson_iter_t name;
    bson_iter_t type;
    bson_iter_t settings;
    bson_iter_t settingsInner;

    if (bson_iter_find_descendant(&b, "enabled", &enabled) && BSON_ITER_HOLDS_BOOL(&enabled)) {
        c._enabled = bson_iter_bool(&enabled);
    }
    if (bson_iter_find_descendant(&b, "name", &name) && BSON_ITER_HOLDS_UTF8(&name)) {
        c._name = bson_iter_utf8(&name, nullptr);
    }
    if (bson_iter_find_descendant(&b, "type", &type) && BSON_ITER_HOLDS_UTF8(&type)) {
        c._type = Channel::channelTypeFromString(bson_iter_utf8(&type, nullptr));
    }
    if (bson_iter_find_descendant(&b, "settings", &settings) && BSON_ITER_HOLDS_DOCUMENT(&settings) && bson_iter_recurse(&settings, &settingsInner)) {
        if (c._type == Channel::Type::TONE) {
            c._settings = new ToneChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::NOISE) {
            c._settings = new NoiseChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::FM) {
            c._settings = new FMChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::DPCM) {
            c._settings = new ROMChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::SPCM) {
            c._settings = new ROMChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::SSG) {
            c._settings = new SSGChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::MELODY) {
            c._settings = new MelodyChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
        if (c._type == Channel::Type::RHYTHM) {
            c._settings = new RhythmChannelSettings;
            c._settings->fromBSON(settingsInner);
        }
    }

    return c;
}

void BSON::fromNote(bson_t* dst, const Note& note)
{
    BSON_APPEND_INT32(dst, "key", note._key);
    BSON_APPEND_DOUBLE(dst, "duration", note._duration);
    BSON_APPEND_INT32(dst, "velocity", note._velocity);
}

Note BSON::toNote(bson_iter_t& b)
{
    Note n;

    bson_iter_t key;
    bson_iter_t duration;
    bson_iter_t velocity;

    if (bson_iter_find_descendant(&b, "key", &key) && BSON_ITER_HOLDS_INT32(&key)) {
        n._key = bson_iter_int32(&key);
    }
    if (bson_iter_find_descendant(&b, "duration", &duration) && BSON_ITER_HOLDS_DOUBLE(&duration)) {
        n._duration = bson_iter_double(&duration);
    }
    if (bson_iter_find_descendant(&b, "velocity", &velocity) && BSON_ITER_HOLDS_INT32(&velocity)) {
        n._velocity = bson_iter_int32(&velocity);
    }

    return n;
}

void BSON::fromTrackItem(bson_t* dst, const Track::Item* const item)
{
    bson_t note;
    bson_init(&note);
    fromNote(&note, item->note());

    BSON_APPEND_DOUBLE(dst, "time", item->time());
    BSON_APPEND_DOCUMENT(dst, "note", &note);
}

Track::Item BSON::toTrackItem(bson_iter_t& b)
{
    Track::Item i;

    bson_iter_t time;
    bson_iter_t note;
    bson_iter_t noteInner;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        i._time = bson_iter_double(&time);
    }
    if (bson_iter_find_descendant(&b, "note", &note) && BSON_ITER_HOLDS_DOCUMENT(&note) && bson_iter_recurse(&note, &noteInner)) {
        i._note = toNote(noteInner);
    }

    return i;
}

void BSON::fromTrack(bson_t* dst, const Track& track)
{
    bson_t items;
    bson_t settingsChanges;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "items", &items);
    for (const Track::Item* const item : track._items) {
        bson_t b_item;
        bson_init(&b_item);
        fromTrackItem(&b_item, item);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&items, key, &b_item);
    }
    bson_append_array_end(dst, &items);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "settingsChanges", &settingsChanges);
    for (const Track::SettingsChange* const sc : track._settingsChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromTrackSettingsChange(&b_change, sc);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&settingsChanges, key, &b_change);
    }
    bson_append_array_end(dst, &settingsChanges);

    BSON_APPEND_BOOL(dst, "usePianoRoll", track.doesUsePianoRoll());
}

Track BSON::toTrack(bson_iter_t& b)
{
    Track t;

    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    bson_iter_t changes;
    bson_iter_t change;

    bson_iter_t usePianoRoll;

    if (bson_iter_find_descendant(&b, "items", &items) && BSON_ITER_HOLDS_ARRAY(&items) && bson_iter_recurse(&items, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &item);
            t._items.append(new Track::Item(toTrackItem(item)));
        }
    }

    if (bson_iter_find_descendant(&b, "settingsChanges", &changes) && BSON_ITER_HOLDS_ARRAY(&changes) && bson_iter_recurse(&changes, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &change);
            t._settingsChanges.append(new Track::SettingsChange(toTrackSettingsChange(change)));
        }
    }

    if (bson_iter_find_descendant(&b, "usePianoRoll", &usePianoRoll) && BSON_ITER_HOLDS_BOOL(&usePianoRoll)) {
        t._usePianoRoll = bson_iter_bool(&usePianoRoll);
    }

    return t;
}

void BSON::fromPattern(bson_t* dst, const Pattern& pattern)
{
    bson_t tracks;

    if (!pattern.name().isEmpty()) {
        BSON_APPEND_UTF8(dst, "name", pattern.name().toStdString().c_str());
    }

    BSON_APPEND_DOCUMENT_BEGIN(dst, "tracks", &tracks);
    for (auto it = pattern._tracks.begin(); it != pattern._tracks.end(); ++it) {
        bson_t b_track;
        bson_init(&b_track);
        fromTrack(&b_track, *it.value());

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(it.key(), &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&tracks, key, &b_track);
    }
    bson_append_document_end(dst, &tracks);
}

Pattern BSON::toPattern(bson_iter_t& b)
{
    Pattern p;

    bson_iter_t name;
    bson_iter_t tracks;
    bson_iter_t child;
    bson_iter_t track;

    bson_iter_t bb = b;

    if (bson_iter_find_descendant(&bb, "name", &name) && BSON_ITER_HOLDS_UTF8(&name)) {
        p._name = bson_iter_utf8(&name, nullptr);
    }

    if (bson_iter_find_descendant(&b, "tracks", &tracks) && BSON_ITER_HOLDS_DOCUMENT(&tracks) && bson_iter_recurse(&tracks, &child)) {
        while (bson_iter_next(&child)) {
            const char* key = bson_iter_key(&child);
            bson_iter_recurse(&child, &track);
            p._tracks[QString(key).toInt()] = new Track(toTrack(track));
        }
    }

    return p;
}

void BSON::fromPlaylistItem(bson_t* dst, const Playlist::Item* const item)
{
    BSON_APPEND_DOUBLE(dst, "time", item->time());
    BSON_APPEND_INT32(dst, "pattern", item->pattern());
}

Playlist::Item BSON::toPlaylistItem(bson_iter_t& b, Project* project)
{
    Playlist::Item i;

    bson_iter_t time;
    bson_iter_t pattern;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        i._time = bson_iter_double(&time);
    }
    if (bson_iter_find_descendant(&b, "pattern", &pattern) && BSON_ITER_HOLDS_INT32(&pattern)) {
        i._pattern = bson_iter_int32(&pattern);
    }

    i._project = project;

    return i;
}

void BSON::fromTrackSettingsChange(bson_t* dst, const Track::SettingsChange* const change)
{
    BSON_APPEND_DOUBLE(dst, "time", change->time());

    bson_t settings = change->_settings->toBSON();
    BSON_APPEND_DOCUMENT(dst, "settings", &settings);
}

Track::SettingsChange BSON::toTrackSettingsChange(bson_iter_t& b)
{
    Track::SettingsChange sc;

    bson_iter_t time;
    bson_iter_t settings;
    bson_iter_t settingsInner;
    bson_iter_t type;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        sc._time = bson_iter_double(&time);
    }
    if (bson_iter_find_descendant(&b, "settings", &settings) && BSON_ITER_HOLDS_DOCUMENT(&settings) && bson_iter_recurse(&settings, &settingsInner)) {
        if (bson_iter_find_descendant(&settingsInner, "_type", &type) && BSON_ITER_HOLDS_UTF8(&type)) {
            Channel::Type channelType = Channel::channelTypeFromString(bson_iter_utf8(&type, nullptr));

            if (bson_iter_recurse(&settings, &settingsInner)) {
                switch (channelType) {
                    case Channel::TONE:
                        sc._settings = new ToneChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::NOISE:
                        sc._settings = new NoiseChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::FM:
                        sc._settings = new FMChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::DPCM:
                        sc._settings = new ROMChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::SPCM:
                        sc._settings = new ROMChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::SSG:
                        sc._settings = new SSGChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::MELODY:
                        sc._settings = new MelodyChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                    case Channel::RHYTHM:
                        sc._settings = new RhythmChannelSettings;
                        sc._settings->fromBSON(settingsInner);
                        break;
                }
            }
        }
    }

    return sc;
}

void BSON::fromLFOChange(bson_t* dst, const Playlist::LFOChange* const change)
{
    BSON_APPEND_DOUBLE(dst, "time", change->time());
    BSON_APPEND_INT32(dst, "mode", change->mode());
}

Playlist::LFOChange BSON::toLFOChange(bson_iter_t& b)
{
    Playlist::LFOChange lfoChange;

    bson_iter_t time;
    bson_iter_t mode;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        lfoChange._time = bson_iter_double(&time);
    }

    if (bson_iter_find_descendant(&b, "mode", &mode) && BSON_ITER_HOLDS_INT(&mode)) {
        lfoChange._mode = bson_iter_int32(&mode);
    }

    return lfoChange;
}

void BSON::fromNoiseFrequencyChange(bson_t* dst, const Playlist::NoiseFrequencyChange* const change)
{
    BSON_APPEND_DOUBLE(dst, "time", change->time());
    BSON_APPEND_INT32(dst, "frequency", change->frequency());
}

Playlist::NoiseFrequencyChange BSON::toNoiseFrequencyChange(bson_iter_t& b)
{
    Playlist::NoiseFrequencyChange nfChange;

    bson_iter_t time;
    bson_iter_t freq;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        nfChange._time = bson_iter_double(&time);
    }

    if (bson_iter_find_descendant(&b, "frequency", &freq) && BSON_ITER_HOLDS_INT(&freq)) {
        nfChange._freq = bson_iter_int32(&freq);
    }

    return nfChange;
}

void BSON::fromEnvelopeFrequencyChange(bson_t* dst, const Playlist::EnvelopeFrequencyChange* const change)
{
    BSON_APPEND_DOUBLE(dst, "time", change->time());
    BSON_APPEND_INT32(dst, "frequency", change->frequency());
}

Playlist::EnvelopeFrequencyChange BSON::toEnvelopeFrequencyChange(bson_iter_t& b)
{
    Playlist::EnvelopeFrequencyChange efChange;

    bson_iter_t time;
    bson_iter_t freq;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        efChange._time = bson_iter_double(&time);
    }

    if (bson_iter_find_descendant(&b, "frequency", &freq) && BSON_ITER_HOLDS_INT(&freq)) {
        efChange._freq = bson_iter_int32(&freq);
    }

    return efChange;
}

void BSON::fromEnvelopeShapeChange(bson_t* dst, const Playlist::EnvelopeShapeChange* const change)
{
    bson_t shape = change->shape().toBSON();

    BSON_APPEND_DOUBLE(dst, "time", change->time());
    BSON_APPEND_DOCUMENT(dst, "shape", &shape);
}

Playlist::EnvelopeShapeChange BSON::toEnvelopeShapeChange(bson_iter_t& b)
{
    Playlist::EnvelopeShapeChange esChange;

    bson_iter_t time;
    bson_iter_t shape;
    bson_iter_t shapeInner;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        esChange._time = bson_iter_double(&time);
    }

    if (bson_iter_find_descendant(&b, "shape", &shape) && BSON_ITER_HOLDS_DOCUMENT(&shape) && bson_iter_recurse(&shape, &shapeInner)) {
        esChange._shape.fromBSON(shapeInner);
    }

    return esChange;
}

void BSON::fromUserToneChange(bson_t* dst, const Playlist::UserToneChange* const change)
{
    bson_t tone = change->_userTone.toBSON();

    BSON_APPEND_DOUBLE(dst, "time", change->time());
    BSON_APPEND_DOCUMENT(dst, "tone", &tone);
}

Playlist::UserToneChange BSON::toUserToneChange(bson_iter_t& b)
{
    Playlist::UserToneChange utChange;

    bson_iter_t time;
    bson_iter_t tone;
    bson_iter_t toneInner;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        utChange._time = bson_iter_double(&time);
    }

    if (bson_iter_find_descendant(&b, "tone", &tone) && BSON_ITER_HOLDS_DOCUMENT(&tone) && bson_iter_recurse(&tone, &toneInner)) {
        utChange._userTone.fromBSON(toneInner);
    }

    return utChange;
}

void BSON::fromPlaylist(bson_t* dst, const Playlist& playlist)
{
    BSON_APPEND_DOUBLE(dst, "loopOffset", playlist.loopOffset());

    bson_t items;
    bson_t lfoChanges;
    bson_t noiseFrequencyChanges;
    bson_t envelopeFrequencyChanges;
    bson_t envelopeShapeChanges;
    bson_t userToneChanges;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "items", &items);
    for (const Playlist::Item* const item : playlist._items) {
        bson_t b_item;
        bson_init(&b_item);
        fromPlaylistItem(&b_item, item);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&items, key, &b_item);
    }
    bson_append_array_end(dst, &items);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "lfoChanges", &lfoChanges);
    for (const Playlist::LFOChange* const lfoChange : playlist._lfoChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromLFOChange(&b_change, lfoChange);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&lfoChanges, key, &b_change);
    }
    bson_append_array_end(dst, &lfoChanges);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "noiseFrequencyChanges", &noiseFrequencyChanges);
    for (const Playlist::NoiseFrequencyChange* const nfChange : playlist._noiseFreqChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromNoiseFrequencyChange(&b_change, nfChange);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&noiseFrequencyChanges, key, &b_change);
    }
    bson_append_array_end(dst, &noiseFrequencyChanges);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "envelopeFrequencyChanges", &envelopeFrequencyChanges);
    for (const Playlist::EnvelopeFrequencyChange* const efChange : playlist._envFreqChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromEnvelopeFrequencyChange(&b_change, efChange);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&envelopeFrequencyChanges, key, &b_change);
    }
    bson_append_array_end(dst, &envelopeFrequencyChanges);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "envelopeShapeChanges", &envelopeShapeChanges);
    for (const Playlist::EnvelopeShapeChange* const esChange : playlist._envShapeChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromEnvelopeShapeChange(&b_change, esChange);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&envelopeShapeChanges, key, &b_change);
    }
    bson_append_array_end(dst, &envelopeShapeChanges);

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "userToneChanges", &userToneChanges);
    for (const Playlist::UserToneChange* const utChange : playlist._userToneChanges) {
        bson_t b_change;
        bson_init(&b_change);
        fromUserToneChange(&b_change, utChange);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&userToneChanges, key, &b_change);
    }
    bson_append_array_end(dst, &userToneChanges);
}

Playlist BSON::toPlaylist(bson_iter_t& b, Project* project)
{
    Playlist p(project);

    bson_iter_t loopOffset;
    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    bson_iter_t lfoChanges;
    bson_iter_t lfoChange;

    bson_iter_t noiseFreqChanges;
    bson_iter_t noiseFreqChange;

    bson_iter_t envFreqChanges;
    bson_iter_t envFreqChange;

    bson_iter_t envShapeChanges;
    bson_iter_t envShapeChange;

    bson_iter_t userToneChanges;
    bson_iter_t userToneChange;

    if (bson_iter_find_descendant(&b, "loopOffset", &loopOffset) && BSON_ITER_HOLDS_DOUBLE(&loopOffset)) {
        p._loopOffset = bson_iter_double(&loopOffset);
    }

    if (bson_iter_find_descendant(&b, "items", &items) && BSON_ITER_HOLDS_ARRAY(&items) && bson_iter_recurse(&items, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &item);
            p._items.append(new Playlist::Item(toPlaylistItem(item)));
        }
    }

    if (bson_iter_find_descendant(&b, "lfoChanges", &lfoChanges) && BSON_ITER_HOLDS_ARRAY(&lfoChanges) && bson_iter_recurse(&lfoChanges, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &lfoChange);
            p._lfoChanges.append(new Playlist::LFOChange(toLFOChange(lfoChange)));
        }
    }

    if (bson_iter_find_descendant(&b, "noiseFrequencyChanges", &noiseFreqChanges) && BSON_ITER_HOLDS_ARRAY(&noiseFreqChanges) && bson_iter_recurse(&noiseFreqChanges, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &noiseFreqChange);
            p._noiseFreqChanges.append(new Playlist::NoiseFrequencyChange(toNoiseFrequencyChange(noiseFreqChange)));
        }
    }

    if (bson_iter_find_descendant(&b, "envelopeFrequencyChanges", &envFreqChanges) && BSON_ITER_HOLDS_ARRAY(&envFreqChanges) && bson_iter_recurse(&envFreqChanges, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &envFreqChange);
            p._envFreqChanges.append(new Playlist::EnvelopeFrequencyChange(toEnvelopeFrequencyChange(envFreqChange)));
        }
    }

    if (bson_iter_find_descendant(&b, "envelopeShapeChanges", &envShapeChanges) && BSON_ITER_HOLDS_ARRAY(&envShapeChanges) && bson_iter_recurse(&envShapeChanges, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &envShapeChange);
            p._envShapeChanges.append(new Playlist::EnvelopeShapeChange(toEnvelopeShapeChange(envShapeChange)));
        }
    }

    if (bson_iter_find_descendant(&b, "userToneChanges", &userToneChanges) && BSON_ITER_HOLDS_ARRAY(&userToneChanges) && bson_iter_recurse(&userToneChanges, &child)) {
        while (bson_iter_next(&child)) {
            bson_iter_recurse(&child, &userToneChange);
            p._userToneChanges.append(new Playlist::UserToneChange(toUserToneChange(userToneChange)));
        }
    }

    return p;
}

void BSON::fromProject(bson_t* dst,const Project& project)
{
    // Channels

    bson_t channels;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "channels", &channels);
    for (const Channel& channel : project._channels) {
        bson_t b_channel;
        bson_init(&b_channel);
        fromChannel(&b_channel, channel);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&channels, key, &b_channel);
    }
    bson_append_array_end(dst, &channels);

    // Patterns

    bson_t patterns;

    i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "patterns", &patterns);
    for (const Pattern* pattern : project._patterns) {
        bson_t b_pattern;
        bson_init(&b_pattern);
        fromPattern(&b_pattern, *pattern);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&patterns, key, &b_pattern);
    }
    bson_append_array_end(dst, &patterns);

    BSON_APPEND_INT32(dst, "frontPattern", project._frontPattern);

    // Playlist

    bson_t b_playlist;
    bson_init(&b_playlist);
    fromPlaylist(&b_playlist, project._playlist);

    BSON_APPEND_DOCUMENT(dst, "playlist", &b_playlist);

    // Tempo & beats per bar

    BSON_APPEND_INT32(dst, "tempo", project._tempo);
    BSON_APPEND_INT32(dst, "beatsPerBar", project.beatsPerBar());

    // Play mode

    BSON_APPEND_UTF8(dst, "playMode", project._playMode == Project::PlayMode::PATTERN ? "PATTERN" : "SONG");

    // LFO

    BSON_APPEND_INT32(dst, "lfo", project._lfoMode);

    // SSG Noise Frequency

    BSON_APPEND_INT32(dst, "ssgNoiseFrequency", project._ssgNoiseFreq);

    // SSG Envelope frequency

    BSON_APPEND_INT32(dst, "ssgEnvelopeFrequency", project._ssgEnvelopeFreq);

    // SSG Envelope shape

    bson_t shape = project._ssgEnvelopeSettings.toBSON();
    BSON_APPEND_DOCUMENT(dst, "ssgEnvelopeShape", &shape);

    // User tone
    bson_t userTone = project._userTone.toBSON();
    BSON_APPEND_DOCUMENT(dst, "userTone", &userTone);

    // ROM
    BSON_APPEND_UTF8(dst, "spcmFile", project.spcmFile().toStdString().c_str());
    BSON_APPEND_UTF8(dst, "dpcmFile", project.dpcmFile().toStdString().c_str());

    // Info
    bson_t b_info;
    bson_init(&b_info);
    fromProjectInfo(&b_info, project._info);
    BSON_APPEND_BOOL(dst, "showInfo", project.showInfoOnOpen());

    BSON_APPEND_DOCUMENT(dst, "info", &b_info);
}

Project BSON::toProject(bson_iter_t& b)
{
    Project p;
    p._channels.clear();

    // Channels

    bson_iter_t channels;
    bson_iter_t channel;
    bson_iter_t channelInner;
    if (bson_iter_find_descendant(&b, "channels", &channels) && BSON_ITER_HOLDS_ARRAY(&channels) && bson_iter_recurse(&channels, &channel)) {
        while (bson_iter_next(&channel)) {
            bson_iter_recurse(&channel, &channelInner);
            p._channels.append(toChannel(channelInner));
        }
    }

    // Patterns

    bson_iter_t patterns;
    bson_iter_t pattern;
    bson_iter_t patternInner;
    if (bson_iter_find_descendant(&b, "patterns", &patterns) && BSON_ITER_HOLDS_ARRAY(&patterns) && bson_iter_recurse(&patterns, &pattern)) {
        while (bson_iter_next(&pattern)) {
            bson_iter_recurse(&pattern, &patternInner);
            p._patterns.append(new Pattern(toPattern(patternInner)));
        }
    }

    bson_iter_t frontPattern;

    if (bson_iter_find_descendant(&b, "frontPattern", &frontPattern) && BSON_ITER_HOLDS_INT32(&frontPattern)) {
        p._frontPattern = bson_iter_int32(&frontPattern);
    }

    // Playlist

    bson_iter_t playlist;
    bson_iter_t playlistInner;
    if (bson_iter_find_descendant(&b, "playlist", &playlist) && BSON_ITER_HOLDS_DOCUMENT(&playlist) && bson_iter_recurse(&playlist, &playlistInner)) {
        p._playlist = toPlaylist(playlistInner, &p);
    }

    // Tempo & beats per bar

    bson_iter_t tempo;
    bson_iter_t beatsPerBar;

    if (bson_iter_find_descendant(&b, "tempo", &tempo) && BSON_ITER_HOLDS_INT32(&tempo)) {
        p._tempo = bson_iter_int32(&tempo);
    }
    if (bson_iter_find_descendant(&b, "beatsPerBar", &beatsPerBar) && BSON_ITER_HOLDS_INT32(&beatsPerBar)) {
        p._beatsPerBar = bson_iter_int32(&beatsPerBar);
    }

    // Play mode

    bson_iter_t playMode;

    if (bson_iter_find_descendant(&b, "playMode", &playMode) && BSON_ITER_HOLDS_UTF8(&playMode)) {
        p._playMode = (QString(bson_iter_utf8(&playMode, NULL)) == QString("PATTERN")) ? Project::PlayMode::PATTERN : Project::PlayMode::SONG;
    }

    // LFO

    bson_iter_t lfo;

    if (bson_iter_find_descendant(&b, "lfo", &lfo) && BSON_ITER_HOLDS_INT(&lfo)) {
        p._lfoMode = bson_iter_int32(&lfo);
    }

    // SSG noise frequency

    bson_iter_t nfreq;

    if (bson_iter_find_descendant(&b, "ssgNoiseFrequency", &nfreq) && BSON_ITER_HOLDS_INT(&nfreq)) {
        p._ssgNoiseFreq = bson_iter_int32(&nfreq);
    }

    // SSG envelope frequency

    bson_iter_t efreq;

    if (bson_iter_find_descendant(&b, "ssgEnvelopeFrequency", &efreq) && BSON_ITER_HOLDS_INT(&efreq)) {
        p._ssgEnvelopeFreq = bson_iter_int32(&efreq);
    }

    // SSG envelope shape

    bson_iter_t eshape;
    bson_iter_t eshapeInner;

    if (bson_iter_find_descendant(&b, "ssgEnvelopeShape", &eshape) && BSON_ITER_HOLDS_DOCUMENT(&eshape) && bson_iter_recurse(&eshape, &eshapeInner)) {
        p._ssgEnvelopeSettings.fromBSON(eshapeInner);
    }

    // User tone

    bson_iter_t userTone;
    bson_iter_t userToneInner;

    if (bson_iter_find_descendant(&b, "userTone", &userTone) && BSON_ITER_HOLDS_DOCUMENT(&userTone) && bson_iter_recurse(&userTone, &userToneInner)) {
        p._userTone.fromBSON(userToneInner);
    }

    // ROM

    bson_iter_t romFile;

    if (bson_iter_find_descendant(&b, "spcmFile", &romFile) && BSON_ITER_HOLDS_UTF8(&romFile)) {
        p._spcmFile = bson_iter_utf8(&romFile, NULL);
    }

    if (bson_iter_find_descendant(&b, "dpcmFile", &romFile) && BSON_ITER_HOLDS_UTF8(&romFile)) {
        p._dpcmFile = bson_iter_utf8(&romFile, NULL);
    }

    // Info

    bson_iter_t showInfo;

    if (bson_iter_find_descendant(&b, "showInfo", &showInfo) && BSON_ITER_HOLDS_BOOL(&showInfo)) {
        p._showInfoOnOpen = bson_iter_bool(&showInfo);
    }

    bson_iter_t info;

    if (bson_iter_find_descendant(&b, "info", &info) && BSON_ITER_HOLDS_DOCUMENT(&info) && bson_iter_recurse(&info, &info)) {
        p._info = toProjectInfo(info);
    }

    return p;
}

void BSON::fromProjectInfo(bson_t* dst, const Project::Info& info)
{
    BSON_APPEND_UTF8(dst, "title", info.title().toStdString().c_str());
    BSON_APPEND_UTF8(dst, "game", info.game().toStdString().c_str());
    BSON_APPEND_UTF8(dst, "author", info.author().toStdString().c_str());
    BSON_APPEND_UTF8(dst, "releaseDate", info.releaseDate().toString("yyyy/MM/dd").toStdString().c_str());
    BSON_APPEND_UTF8(dst, "notes", info.notes().toStdString().c_str());
}

Project::Info BSON::toProjectInfo(bson_iter_t& b)
{
    Project::Info info;

    bson_iter_t title;
    bson_iter_t game;
    bson_iter_t author;
    bson_iter_t releaseDate;
    bson_iter_t notes;

    if (bson_iter_find_descendant(&b, "title", &title) && BSON_ITER_HOLDS_UTF8(&title)) {
        info.setTitle(bson_iter_utf8(&title, NULL));
    }

    if (bson_iter_find_descendant(&b, "game", &game) && BSON_ITER_HOLDS_UTF8(&game)) {
        info.setGame(bson_iter_utf8(&game, NULL));
    }

    if (bson_iter_find_descendant(&b, "author", &author) && BSON_ITER_HOLDS_UTF8(&author)) {
        info.setAuthor(bson_iter_utf8(&author, NULL));
    }

    if (bson_iter_find_descendant(&b, "releaseDate", &releaseDate) && BSON_ITER_HOLDS_UTF8(&releaseDate)) {
        info.setReleaseDate(QDate::fromString(bson_iter_utf8(&releaseDate, NULL), "yyyy/MM/dd"));
    }

    if (bson_iter_find_descendant(&b, "notes", &notes) && BSON_ITER_HOLDS_UTF8(&notes)) {
        info.setNotes(bson_iter_utf8(&notes, NULL));
    }

    return info;
}
