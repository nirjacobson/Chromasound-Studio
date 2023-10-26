#ifndef _BSON_H
#define _BSON_H

#include <QFile>

#include <libbson-1.0/bson/bson.h>

#include "project/channel/channel.h"
#include "project/note.h"
#include "project/track.h"
#include "project/pattern.h"
#include "project/project.h"
#include "project/playlist.h"

class BSON
{
    public:
        BSON();

        static QByteArray encode(const Project& project);
        static Project decode(const QString& file);

        static QByteArray encodePatch(const FMChannelSettings* settings);
        static FMChannelSettings* decodePatch(const QString& file);

        static void fromTrackItem(bson_t* dst, const Track::Item* const item);
        static Track::Item toTrackItem(bson_iter_t& b);

    private:
        static void fromChannel(bson_t* dst, const Channel& channel);
        static Channel toChannel(bson_iter_t& b);

        static void fromNote(bson_t* dst, const Note& note);
        static Note toNote(bson_iter_t& b);

        static void fromTrack(bson_t* dst, const Track& track);
        static Track toTrack(bson_iter_t& b);

        static void fromPattern(bson_t* dst, const Pattern& pattern);
        static Pattern toPattern(bson_iter_t& b);

        static void fromPlaylistItem(bson_t* dst, const Playlist::Item* const item);
        static Playlist::Item toPlaylistItem(bson_iter_t& b);

        static void fromPlaylist(bson_t* dst, const Playlist& playlist);
        static Playlist toPlaylist(bson_iter_t& b, Project* project);

        static void fromProject(bson_t* dst, const Project& project);
        static Project toProject(bson_iter_t& b);
};

#endif // _BSON_H
