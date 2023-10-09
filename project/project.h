#ifndef PROJECT_H
#define PROJECT_H

#include <QList>

#include "pattern.h"
#include "channel/channel.h"
#include "ganttwidget/ganttitem.h"

class Project
{
        friend class BSON;

    public:
        enum PlayMode {
            PATTERN,
            SONG
        };

        class Playlist
        {
                friend class BSON;
                friend class Project;

            public:

                class Item : public GanttItem {
                        friend class BSON;
                        friend class Playlist;

                    public:
                        Item(Project* const project, const float time, const int pattern);

                        float time() const;

                        int pattern() const;

                        bool operator==(const Item& item);

                    private:
                        Item();

                        Project* _project;
                        float _time;
                        int _pattern;

                        // GanttItem interface
                    public:
                        long row() const;
                        float duration() const;
                        int velocity() const;
                        void setRow(const long row);
                        void setTime(const float time);
                        void setDuration(const float);
                        void setVelocity(const int);
                };

                Playlist(Project* const project);
                Playlist(Playlist&& o);
                ~Playlist();

                const QList<Item*>& items() const;
                void addItem(const float time, const int pattern);
                void removeItem(const float time, const int pattern);

                float getLength() const;

                QMap<int, float> activePatternsAtTime(const float time) const;
                Playlist& operator=(Playlist&& o);

            private:
                Playlist();

                QList<Item*> _items;
                Project* _project;
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
