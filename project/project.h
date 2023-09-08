#ifndef PROJECT_H
#define PROJECT_H

#include <QList>

#include "pattern.h"
#include "channel.h"
#include "ganttwidget/ganttitem.h"

class Project
{
    public:

        class PlaylistItem : public GanttItem {
            public:
                PlaylistItem(Project* const project, const float time, const int pattern);

                float time() const;

                int pattern() const;

                bool operator==(const PlaylistItem& item);

            private:

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
                void setDuration(const float duration);
                void setVelocity(const int velocity);
        };

        Project();
        ~Project();

        int channels() const;

        Channel& getChannel(const int idx);
        Pattern& getPattern(const int idx);
        const Pattern& getPattern(const int idx) const;
        int defaultPattern() const;
        void setDefaultPattern(const int idx);
        Pattern& getDefaultPattern();
        const Pattern& getDefaultPattern() const;

        QList<PlaylistItem*>& playlist();
        void addPlaylistItem(const float time, const int pattern);
        void removePlaylistItem(const float time, const int pattern);

        float getLength() const;

        int tempo() const;
        void setTempo(const int tempo);

        int beatsPerBar() const;
        void setBeatsPerBar(const int beats);

        const QList<Pattern>& patterns() const;
        QMap<int, float> activePatternsAtTime(const float time) const;

    private:
        QList<Channel> _channels;
        QList<Pattern> _patterns;
        int _defaultPattern;

        QList<PlaylistItem*> _playlist;

        int _tempo;
        int _beatsPerBar;
};

#endif // PROJECT_H
