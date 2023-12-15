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
        enum Format {
            FM_PSG,
            STANDARD
        };

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
                StreamNoteItem(const float time, const Channel::Type type, const Track* _track, const Note& note, const ChannelSettings* channelSettings);

                void setChannel(const int channel);
                void setOn(const bool on);
                void setChannelSettings(const ChannelSettings* settings);

                Channel::Type type() const;
                const Track* track() const;
                const Note& note() const;
                const ChannelSettings* channelSettings() const;
                int channel() const;
                bool on() const;
            private:
                Channel::Type _type;
                const Track* _track;
                int _channel;
                Note _note;
                const ChannelSettings* _channelSettings;
                bool _on;
        };

        class StreamLFOItem : public StreamItem {
            public:
                StreamLFOItem(const float time, const int mode);

                int mode() const;
            private:
                int _mode;
        };

        VGMStream(const Format format = Format::FM_PSG);
        ~VGMStream();

        void assignChannel(StreamNoteItem* noteItem, QList<StreamItem*>& items);
        void releaseChannel(const Channel::Type type, const int channel);

        void encode(const Project& project, QList<StreamItem*>& items, QByteArray& data);

        QByteArray compile(Project& project, const Pattern& pattern, bool header = false, int* loopOffsetData = nullptr, const float loopStart = -1, const float loopEnd = -1, const float currentOffset = 0, int* const currentOffsetData = nullptr);
        QByteArray compile(Project& project, const bool header = false, int* loopOffsetData = nullptr, const float loopStart = -1, const float loopEnd = -1, const float currentOffset = 0, int* const currentOffsetData = nullptr);

        void reset();

        QByteArray encodeStandardPCM(const Project& project, const Pattern& pattern, const float loopStart = -1, const float loopEnd = -1);
        QByteArray encodeStandardPCM(const Project& project, const float loopStart = -1, const float loopEnd = -1);

        QByteArray generateHeader(const Project& project, const QByteArray& data, const int totalSamples, const int loopOffsetData, const bool selectionLoop);

    private:
        static QList<float> frequencies;
        static QList<QList<int>> slotsByAlg;
        static QList<int> ym2612_frequencies;

        class StreamSettingsItem : public StreamItem {
            public:
                StreamSettingsItem(const float time, const int channel, const ChannelSettings* channelSettings, const int velocity = 100);

                const ChannelSettings* channelSettings() const;
                int channel() const;
                int velocity() const;
            private:
                int _channel;
                const ChannelSettings* _channelSettings;
                int _velocity;
        };

        class StreamEndItem : public StreamItem {
            public:
                StreamEndItem(const float time);
        };

        class PhysicalChannel {
            private:
                float _time;
                float _duration;
                bool _acquiredIndefinitely;
                bool _acquiredEver;

            public:
                PhysicalChannel();

                bool acquire(float time, float duration, bool& first);
                void release();
                virtual void reset();
        };
        class FMChannel : public PhysicalChannel {
            public:
                FMChannelSettings& settings();
                void reset() override;
            private:
                FMChannelSettings _settings;
        };
        class ToneChannel : public PhysicalChannel {

        };
        class NoiseChannel : public PhysicalChannel {
            public:
                NoiseChannelSettings& settings();
                void reset() override;
            private:
                NoiseChannelSettings _settings;
        };
        class PCMChannel : public PhysicalChannel {

        };

        static constexpr int FM_CHANNELS = 6;
        static constexpr int TONE_CHANNELS = 3;
        static constexpr int NOISE_CHANNELS = 1;
        static constexpr int PCM_CHANNELS = 4;
        static constexpr int MAX_PCM_ATTENUATION = 4;

        Format _format;

        FMChannel _fmChannels[FM_CHANNELS];
        ToneChannel _toneChannels[TONE_CHANNELS];
        NoiseChannel _noiseChannels[NOISE_CHANNELS];
        PCMChannel _pcmChannels[PCM_CHANNELS];

        QList<Track::SettingsChange*> _createdSCs;

        int acquireToneChannel(const float time, const float duration);
        int acquireNoiseChannel(const float time, const float duration, const NoiseChannelSettings* settings, QList<StreamItem*>& items);
        int acquireFMChannel(const float time, const float duration, const FMChannelSettings* settings, QList<StreamItem*>& items);
        int acquirePCMChannel(const float time, const float duration);

        void processProject(const Project& project, QList<StreamItem*>& items, const float loopStart = -1, const float loopEnd = -1);
        void processPattern(const float time, const Project& project, const Pattern& pattern, QList<StreamItem*>& items, const float loopStart = -1, const float loopEnd = -1);
        void processTrack(const float time, const Channel& channel, const Track* track, QList<StreamItem*>& items, const float loopStart = -1, const float loopEnd = -1);

        void assignChannelsAndExpand(QList<StreamItem*>& items, const int tempo);
        void applySettingsChanges(Project& project, const float time, const Pattern& pattern, QList<StreamItem*>& items);
        void applySettingsChanges2(Project& project, const float time, const Pattern& pattern, QList<StreamItem*>& items);
        void applySettingsChanges(Project& project, QList<StreamItem*>& items);
        void applySettingsChanges2(Project& project, QList<StreamItem*>& items);
        void addSettingsAtCurrentOffset(QList<StreamItem*>& items, const float currentTime);

        void sortItems(QList<StreamItem*>& items);

        void pad(QList<StreamItem*>& items, const float toDuration);

        int encode(const Project& project, const QList<StreamItem*>& items, QByteArray& data, const float loopTime = 0, int* const loopOffsetData = nullptr, const float currentTime = 0, int* const currentOffsetData = nullptr, const bool startAtLoop = false);

        int encodeDelay(const quint32 samples, QByteArray& data, const bool pcm = false);

        void encodeSettingsItem(const StreamSettingsItem* item, QByteArray& data);
        void encodeNoteItem(const Project& project, const StreamNoteItem* item, QByteArray& data);
        void encodeLFOItem(const StreamLFOItem* item, QByteArray& data);

};

#endif // VGMSTREAM_H
