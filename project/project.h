#ifndef PROJECT_H
#define PROJECT_H

#include <QList>

#include "pattern.h"
#include "channel/channel.h"
#include "playlist.h"

class Project
{
        friend class BSON;

    public:
        enum PlayMode {
            PATTERN,
            SONG
        };

        Project();
        ~Project();

        int channels() const;

        Channel& getChannel(const int idx);
        const Channel& getChannel(const int idx) const;
        void removeChannel(const int index);
        void moveChannelUp(const int idx);
        void moveChannelDown(const int idx);

        Pattern& getPattern(const int idx);
        const Pattern& getPattern(const int idx) const;
        int frontPattern() const;
        void setFrontPattern(const int idx);
        Pattern& getFrontPattern();
        const Pattern& getFrontPattern() const;

        PlayMode playMode() const;
        void setPlayMode(const PlayMode mode);

        Playlist& playlist();
        const Playlist& playlist() const;

        float getLength() const;

        int tempo() const;
        void setTempo(const int tempo);

        int beatsPerBar() const;
        void setBeatsPerBar(const int beats);

        const QList<Pattern*>& patterns() const;

        void addChannel();

        Project& operator=(Project&& src);
        Project(Project&& o);

    private:
        QList<Channel> _channels;
        QList<Pattern*> _patterns;
        int _frontPattern;
        PlayMode _playMode;

        Playlist _playlist;

        int _tempo;
        int _beatsPerBar;

        void swapChannels(const int idxa, const int idxb);
};

#endif // PROJECT_H
