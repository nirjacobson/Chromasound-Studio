#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMap>
#include <QList>

#include "ganttwidget/ganttitem.h"

class Project;

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

    bool doesLoop() const;
    float loopOffset() const;
    int loopOffsetSamples() const;
    void setLoopOffset(const float offset);

private:
    Playlist();

    QList<Item*> _items;
    Project* _project;

    float _loopOffset;
};

#endif // PLAYLIST_H