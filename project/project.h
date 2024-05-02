#ifndef PROJECT_H
#define PROJECT_H

#include <QList>
#include <QFile>
#include <QFileInfo>

#include "pattern.h"
#include "channel/channel.h"
#include "playlist.h"
#include "ssg/ssgenvelopesettings.h"
#include "opl/usertone/oplsettings.h"
#include "opl/opll.h"

class Project
{
        friend class BSON;

    public:
        enum PlayMode {
            PATTERN,
            SONG
        };

        class Info {
            public:
                const QString& title() const;
                const QString& game() const;
                const QString& author() const;
                const QDate& releaseDate() const;
                const QString& notes() const;

                void setTitle(const QString& title);
                void setGame(const QString& game);
                void setAuthor(const QString& author);
                void setReleaseDate(const QDate& date);
                void setNotes(const QString& notes);

            private:
                QString _title;
                QString _game;
                QString _author;
                QDate _releaseDate;
                QString _notes;
        };

        Project();
        ~Project();

        int channelCount() const;

        Channel& getChannel(const int idx);
        const Channel& getChannel(const int idx) const;
        void removeChannel(const int index);
        void moveChannelUp(const int idx);
        void moveChannelDown(const int idx);
        void movePatternUp(const int idx);
        void movePatternDown(const int idx);
        void insertPattern(const int idx, Pattern* pattern = nullptr);
        QList<Playlist::Item*> deletePattern(const int idx);

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

        const SSGEnvelopeSettings& ssgEnvelopeShape() const;
        SSGEnvelopeSettings& ssgEnvelopeShape();
        void setSSGEnvelopeSettings(const SSGEnvelopeSettings& settings);

        int ssgEnvelopeFrequency() const;
        void setSSGEnvelopeFrequency(const int freq);

        int ssgNoiseFrequency() const;
        void setSSGNoiseFrequency(const int freq);

        const QList<Pattern*>& patterns() const;
        const QList<Channel>& channels() const;

        void addChannel();
        void addChannel(const int index, const Channel& channel);
        int indexOfChannel(const Channel& channel);

        bool hasPCM() const;
        quint32 pcmOffset(const QString& path) const;
        QByteArray pcm() const;

        Project& operator=(Project&& src);
        Project(Project&& o);

        Info& info();
        const Info& info() const;

        OPLSettings& userTone();
        const OPLSettings& userTone() const;

        OPLL::Type opllType() const;
        void setOpllType(const OPLL::Type type);

        bool usesOPN() const;
        bool usesSSG() const;
        bool usesOPL() const;
        bool usesRhythm() const;

    private:
        QList<Channel> _channels;
        QList<Pattern*> _patterns;
        int _frontPattern;
        PlayMode _playMode;

        Playlist _playlist;
        Info _info;

        int _tempo;
        int _beatsPerBar;

        int _lfoMode;
        SSGEnvelopeSettings _ssgEnvelopeSettings;
        int _ssgEnvelopeFreq;
        int _ssgNoiseFreq;
        OPLSettings _userTone;
        OPLL::Type _opllType;

        void swapChannels(const int idxa, const int idxb);
        void swapPatterns(const int idxa, const int idxb);
};

#endif // PROJECT_H
