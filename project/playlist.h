#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMap>
#include <QList>

#include "common/ganttwidget/ganttitem.h"
#include "common/ganttwidget/ganttmarker.h"
#include "ssg/ssgenvelopesettings.h"
#include "opl/usertone/oplsettings.h"

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

        class LFOChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                LFOChange(const float time, const int mode);

                float time() const;
                QString name() const;
                const QColor& color() const;

                int mode() const;
                void setMode(const int mode);

            private:
                LFOChange();

                float _time;
                int _mode;
        };

        class NoiseFrequencyChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                NoiseFrequencyChange(const float time, const int freq);

                float time() const;
                QString name() const;
                const QColor& color() const;

                int frequency() const;
                void setFrequency(const int freq);

            private:
                NoiseFrequencyChange();

                float _time;
                int _freq;
        };

        class EnvelopeFrequencyChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                EnvelopeFrequencyChange(const float time, const int freq);

                float time() const;
                QString name() const;
                const QColor& color() const;

                int frequency() const;
                void setFrequency(const int freq);

            private:
                EnvelopeFrequencyChange();

                float _time;
                int _freq;
        };

        class EnvelopeShapeChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                EnvelopeShapeChange(const float time, const SSGEnvelopeSettings& shape);

                float time() const;
                QString name() const;
                const QColor& color() const;

                SSGEnvelopeSettings& shape();
                const SSGEnvelopeSettings& shape() const;
                void setShape(const SSGEnvelopeSettings& shape);

            private:
                EnvelopeShapeChange();

                float _time;
                SSGEnvelopeSettings _shape;
        };

        class UserToneChange : public GanttMarker {
                friend class BSON;

            public:
                static QColor COLOR;
                UserToneChange(const float time, const OPLSettings& tone);

                float time() const;
                QString name() const;
                const QColor& color() const;

                OPLSettings& userTone();
                const OPLSettings& userTone() const;
                void setUserTone(const OPLSettings& tone);

            private:
                UserToneChange();

                float _time;
                OPLSettings _userTone;
        };

        Playlist(Project* const project);
        Playlist(Playlist&& o);
        ~Playlist();

        const QList<Item*>& items() const;
        void addItem(const float time, const int pattern);
        void addItems(const QList<Item*>& items);
        void removeItems(const QList<Item*>& items);
        void removeItem(const float time, const int pattern);

        float getLength() const;

        QMap<int, float> activePatternsAtTime(const float time) const;
        Playlist& operator=(Playlist&& o);

        bool doesLoop() const;
        float loopOffset() const;
        int loopOffsetSamples() const;
        void setLoopOffset(const float offset);

        const QList<LFOChange*>& lfoChanges() const;
        QList<LFOChange*>& lfoChanges();
        LFOChange* addLFOChange(const float time, const int mode);
        void removeLFOChange(const LFOChange* lc, const bool keep = false);

        const QList<NoiseFrequencyChange*>& noiseFrequencyChanges() const;
        QList<NoiseFrequencyChange*>& noiseFrequencyChanges();
        NoiseFrequencyChange* addNoiseFrequencyChange(const float time, const int freq);
        void removeNoiseFrequencyChange(const NoiseFrequencyChange* nfc, const bool keep = false);

        const QList<EnvelopeFrequencyChange*>& envelopeFrequencyChanges() const;
        QList<EnvelopeFrequencyChange*>& envelopeFrequencyChanges();
        EnvelopeFrequencyChange* addEnvelopeFrequencyChange(const float time, const int freq);
        void removeEnvelopeFrequencyChange(const EnvelopeFrequencyChange* efc, const bool keep = false);

        const QList<EnvelopeShapeChange*>& envelopeShapeChanges() const;
        QList<EnvelopeShapeChange*>& envelopeShapeChanges();
        EnvelopeShapeChange* addEnvelopeShapeChange(const float time, const SSGEnvelopeSettings& shape);
        void removeEnvelopeShapeChange(const EnvelopeShapeChange* esc, const bool keep = false);

        const QList<UserToneChange*>& userToneChanges() const;
        QList<UserToneChange*>& userToneChanges();
        UserToneChange* addUserToneChange(const float time, const OPLSettings& tone);
        void removeUserToneChange(const UserToneChange* utc, const bool keep = false);

        bool empty() const;
    private:
        Playlist();

        QList<Item*> _items;
        QList<LFOChange*> _lfoChanges;
        QList<NoiseFrequencyChange*> _noiseFreqChanges;
        QList<EnvelopeFrequencyChange*> _envFreqChanges;
        QList<EnvelopeShapeChange*> _envShapeChanges;
        QList<UserToneChange*> _userToneChanges;

        Project* _project;

        float _loopOffset;
};

#endif // PLAYLIST_H
