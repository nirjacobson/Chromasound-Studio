#ifndef VGMSTREAM_H
#define VGMSTREAM_H

#include <QByteArray>
#include <stdint.h>
#include "channel/fmchannelsettings.h"
#include "channel/noisechannelsettings.h"
#include "project.h"
#include "note.h"

class VGMStream
{
    public:
        VGMStream();

        QByteArray compile(const Project& project);
        QByteArray compile(const Project& project, const int pattern);

    private:
        static QList<float> frequencies;
        static QList<QList<int>> slotsByAlg;
        static QList<int> ym2612_frequencies;

        class StreamItem {
            private:
                float _time;
            public:
                virtual ~StreamItem();
                StreamItem(const float time);
                float time() const;
                void setTime(const float time);
        };

        class StreamNoteItem : public StreamItem {
            public:
                StreamNoteItem(const float time, const Channel::Type type, const Note& note, const Settings* channelSettings);

                void setChannel(const int channel);
                void setOn(const bool on);

                Channel::Type type() const;
                const Note& note() const;
                const Settings* channelSettings() const;
                int channel() const;
                bool on() const;
            private:
                Channel::Type _type;
                int _channel;
                Note _note;
                const Settings* _channelSettings;
                bool _on;
        };

        class StreamSettingsItem : public StreamItem {
            public:
                StreamSettingsItem(const float time, const int channel, const Settings* channelSettings);

                const Settings* channelSettings() const;
                int channel() const;
            private:
                int _channel;
                const Settings* _channelSettings;
        };

        class StreamEndItem : public StreamItem {
            public:
                StreamEndItem(const float time);
        };

        class PhysicalChannel {
            private:
                float _time;
                float _duration;

            public:
                bool acquire(float time, float duration);
        };
        class FMChannel : public PhysicalChannel {
            public:
                FMChannelSettings& settings();
            private:
                FMChannelSettings _settings;
        };
        class ToneChannel : public PhysicalChannel {

        };
        class NoiseChannel : public PhysicalChannel {
            public:
                NoiseChannelSettings& settings();
            private:
                NoiseChannelSettings _settings;
        };

        static constexpr int FM_CHANNELS = 6;
        static constexpr int TONE_CHANNELS = 3;
        static constexpr int NOISE_CHANNELS = 1;

        FMChannel _fmChannels[FM_CHANNELS];
        ToneChannel _toneChannels[TONE_CHANNELS];
        NoiseChannel _noiseChannels[NOISE_CHANNELS];

        int acquireToneChannel(const float time, const float duration);
        int acquireNoiseChannel(const float time, const float duration, const NoiseChannelSettings* settings, QList<StreamItem*>& items);
        int acquireFMChannel(const float time, const float duration, const FMChannelSettings* settings, QList<StreamItem*>& items);

        void processPattern(const float time, const Project& project, const Pattern& pattern, QList<StreamItem*>& items);
        void processTrack(const float time, const Channel& channel, const Track* track, QList<StreamItem*>& items);

        void generateItems(const Project& project, QList<StreamItem*>& items);
        void assignChannelsAndExpand(QList<StreamItem*>& items);

        void pad(QList<StreamItem*>& items, const float toDuration);

        void encode(const QList<StreamItem*> items, const int tempo, QByteArray& data);

        void encodeDelay(const int tempo, const float beats, QByteArray& data);
        void encodeSettingsItem(const StreamSettingsItem* item, QByteArray& data);
        void encodeNoteItem(const StreamNoteItem* item, QByteArray& data);
};

#endif // VGMSTREAM_H