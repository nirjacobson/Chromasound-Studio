#ifndef TRACK_H
#define TRACK_H

#include <QMap>
#include <QList>

#include "project/channel/channelsettings.h"
#include "note.h"
#include "common/ganttwidget/ganttitem.h"
#include "common/ganttwidget/ganttmarker.h"

class Track
{
        friend class BSON;
        friend class MIDI;

    public:
        Track();
        ~Track();
        Track(Track&& o);

        Track& operator=(Track&& o);

        class Item : public GanttItem {
                friend class BSON;
                friend class MIDI;

            public:
                Item(const float time, const Note& note);

                float time() const;
                const Note& note() const;
                Note& note();

                bool operator==(const Track::Item& item);

            private:
                Item();

                float _time;
                Note _note;

                // GanttItem interface
            public:
                long row() const;
                float duration() const;
                void setRow(const long row);
                void setTime(const float time);
                void setDuration(const float duration);
                int velocity() const;
                void setVelocity(const int velocity);
        };

        class SettingsChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                SettingsChange(const float time, ChannelSettings* settings);
                ~SettingsChange();

                // GanttMarker interface
                float time() const;
                QString name() const;
                const QColor& color() const;

                ChannelSettings& settings();

            private:
                SettingsChange();

                float _time;
                ChannelSettings* _settings;
        };

        class PitchChange {
            friend class BSON;
            public:
                PitchChange(const float time, const float pitch);

                void setTime(const float time);
                float time() const;
                float pitch() const;

            private:
                PitchChange();

                float _time;
                float _pitch;
        };

        QList<Item*>& items();
        const QList<Item*>& items() const;

        QList<SettingsChange*>& settingsChanges();
        QList<PitchChange*>& pitchChanges();
        const QList<PitchChange*>& pitchChanges() const;

        float length() const;

        Item* addItem(const float time, const Note& note);
        void addItems(const QList<Item*>& items);
        float removeItem(const float time, const int key);
        float removeItem(const float time);
        void removeItems(const QList<Item*>& items);

        SettingsChange* addSettingsChange(const float time, ChannelSettings* settings);
        void removeSettingsChange(const SettingsChange* sc, const bool keep = false);

        PitchChange* addPitchChange(const float time, const float pitch);
        void addPitchChanges(const QList<PitchChange*>& pitchChanges);
        void removePitchChange(const PitchChange* pc, const bool keep = false);

        void usePianoRoll();
        bool doesUsePianoRoll() const;

        void useStepSequencer();

        Track* copy() const;

        void clear();
        bool empty() const;

    private:
        QList<Item*> _items;
        QList<SettingsChange*> _settingsChanges;
        QList<PitchChange*> _pitchChanges;
        bool _usePianoRoll;
};

#endif // TRACK_H
