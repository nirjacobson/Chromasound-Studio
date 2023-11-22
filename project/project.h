#ifndef PROJECT_H
#define PROJECT_H

#include <QList>
#include <QFile>
#include <QFileInfo>

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
        int getPatternBarLength(const int idx) const;
        int getPatternBarLength(const Pattern& pattern) const;
        PlayMode playMode() const;
        void setPlayMode(const PlayMode mode);

        Playlist& playlist();
        const Playlist& playlist() const;

        float getLength() const;

        int tempo() const;
        void setTempo(const int tempo);

        int beatsPerBar() const;
        void setBeatsPerBar(const int beats);

        int lfoMode() const;
        void setLFOMode(const int mode);

        const QList<Pattern*>& patterns() const;

        void addChannel();
        void addChannel(const int index, const Channel& channel);
        int indexOfChannel(const Channel& channel);

        bool hasPCM() const;
        quint32 pcmOffset(const QString& path) const;
        quint32 pcmSize() const;
        QByteArray pcm() const;

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

        int _lfoMode;

        void swapChannels(const int idxa, const int idxb);
};

#endif // PROJECT_H
