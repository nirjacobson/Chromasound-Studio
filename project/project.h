#ifndef PROJECT_H
#define PROJECT_H

#include <QList>

#include "pattern.h"
#include "channel.h"
#include "ganttwidget/ganttitem.h"

class Project
{
        friend class BSON;

    public:
        class Playlist
        {
                friend class BSON;
                friend class Project;

            public:

                class Item : public GanttItem {
                        friend class BSON;

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
                ~Playlist();

                void addItem(const float time, const int pattern);
                void removeItem(const float time, const int pattern);

                float getLength() const;

                QMap<int, float> activePatternsAtTime(const float time) const;

            private:
                Playlist();

                QList<Item*> _items;
                Project* _project;
        };

        Project();

        int channels() const;

        Channel& getChannel(const int idx);
        Pattern& getPattern(const int idx);
        const Pattern& getPattern(const int idx) const;
        int frontPattern() const;
        void setFrontPattern(const int idx);
        Pattern& getFrontPattern();
        const Pattern& getFrontPattern() const;

        Playlist& playlist();

        float getLength() const;

        int tempo() const;
        void setTempo(const int tempo);

        int beatsPerBar() const;
        void setBeatsPerBar(const int beats);

        const QList<Pattern>& patterns() const;

        void addChannel();

        Project& operator=(Project&& src);
        Project(Project&& o);

    private:
        QList<Channel> _channels;
        QList<Pattern> _patterns;
        int _frontPattern;

        Playlist _playlist;

        int _tempo;
        int _beatsPerBar;
};

#endif // PROJECT_H
