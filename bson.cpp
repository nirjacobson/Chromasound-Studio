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

Project BSON::decode(const QString& file)
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

    return toProject(iter);
}

void BSON::fromChannel(bson_t* dst, const Channel& channel)
{
    BSON_APPEND_BOOL(dst, "enabled", channel._enabled);
    BSON_APPEND_UTF8(dst, "name", channel._name.toStdString().c_str());
}

Channel BSON::toChannel(bson_iter_t& b)
{
    Channel c;

    bson_iter_t enabled;
    bson_iter_t name;

    if (bson_iter_find_descendant(&b, "enabled", &enabled) && BSON_ITER_HOLDS_BOOL(&enabled)) {
        c._enabled = bson_iter_bool(&enabled);
    }
    if (bson_iter_find_descendant(&b, "name", &name) && BSON_ITER_HOLDS_UTF8(&name)) {
        c._name = bson_iter_utf8(&enabled, nullptr);
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
}

Track BSON::toTrack(bson_iter_t& b)
{
    Track t;

    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    if (bson_iter_find_descendant(&b, "items", &items) && BSON_ITER_HOLDS_DOCUMENT(&items) && bson_iter_recurse(&items, &child)) {
        do {
            bson_iter_recurse(&child, &item);
            t._items.append(new Track::Item(toTrackItem(item)));
        } while (bson_iter_next(&child)) ;
    }

    return t;
}

void BSON::fromPattern(bson_t* dst, const Pattern& pattern)
{
    bson_t tracks;

    BSON_APPEND_DOCUMENT_BEGIN(dst, "tracks", &tracks);
    for (auto it = pattern._tracks.begin(); it != pattern._tracks.end(); ++it) {
        bson_t b_track;
        bson_init(&b_track);
        fromTrack(&b_track, it.value());

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

    bson_iter_t tracks;
    bson_iter_t child;
    bson_iter_t track;

    if (bson_iter_find_descendant(&b, "tracks", &tracks) && BSON_ITER_HOLDS_DOCUMENT(&tracks) && bson_iter_recurse(&tracks, &child)) {
        do {
            const char* key = bson_iter_key(&child);
            bson_iter_recurse(&child, &track);
            p._tracks.insert(QString(key).toInt(), toTrack(track));
        } while (bson_iter_next(&child)) ;
    }

    return p;
}

void BSON::fromPlaylistItem(bson_t* dst, const Project::Playlist::Item* const item)
{
    BSON_APPEND_DOUBLE(dst, "time", item->time());
    BSON_APPEND_INT32(dst, "pattern", item->pattern());
}

Project::Playlist::Item BSON::toPlaylistItem(bson_iter_t& b)
{
    Project::Playlist::Item i;

    bson_iter_t time;
    bson_iter_t pattern;

    if (bson_iter_find_descendant(&b, "time", &time) && BSON_ITER_HOLDS_DOUBLE(&time)) {
        i._time = bson_iter_double(&time);
    }
    if (bson_iter_find_descendant(&b, "pattern", &pattern) && BSON_ITER_HOLDS_INT32(&pattern)) {
        i._pattern = bson_iter_int32(&pattern);
    }

    return i;
}

void BSON::fromPlaylist(bson_t* dst, const Project::Playlist& playlist)
{
    bson_t items;

    uint32_t i = 0;

    BSON_APPEND_ARRAY_BEGIN(dst, "items", &items);
    for (const Project::Playlist::Item* const item : playlist._items) {
        bson_t b_item;
        bson_init(&b_item);
        fromPlaylistItem(&b_item, item);

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i++, &key, keybuff, sizeof keybuff);

        BSON_APPEND_DOCUMENT(&items, key, &b_item);
    }
    bson_append_array_end(dst, &items);
}

Project::Playlist BSON::toPlaylist(bson_iter_t& b, Project* project)
{
    Project::Playlist p(project);

    bson_iter_t items;
    bson_iter_t child;
    bson_iter_t item;

    if (bson_iter_find_descendant(&b, "items", &items) && BSON_ITER_HOLDS_DOCUMENT(&items) && bson_iter_recurse(&items, &child)) {
        do {
            bson_iter_recurse(&child, &item);
            p._items.append(new Project::Playlist::Item(toPlaylistItem(item)));
        } while (bson_iter_next(&child)) ;
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
    for (const Pattern& pattern : project._patterns) {
        bson_t b_pattern;
        bson_init(&b_pattern);
        fromPattern(&b_pattern, pattern);

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
}

Project BSON::toProject(bson_iter_t& b)
{
    Project p;

    // Channels

    bson_iter_t channels;
    bson_iter_t channel;
    if (bson_iter_find_descendant(&b, "channels", &channels) && BSON_ITER_HOLDS_ARRAY(&channels) && bson_iter_recurse(&channels, &channel)) {
        do {
            p._channels.append(toChannel(channel));
        } while (bson_iter_next(&channel)) ;
    }

    // Patterns

    bson_iter_t patterns;
    bson_iter_t pattern;
    if (bson_iter_find_descendant(&b, "patterns", &patterns) && BSON_ITER_HOLDS_ARRAY(&patterns) && bson_iter_recurse(&patterns, &pattern)) {
        do {
            p._patterns.append(toPattern(pattern));
        } while (bson_iter_next(&channel)) ;
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

    return p;
}