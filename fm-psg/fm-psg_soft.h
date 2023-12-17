#ifndef FM_PSG_SOFT_H
#define FM_PSG_SOFT_H

#include <QTimer>

#include "fm-psg.h"
#include "project/vgmstream.h"
#include "soft/gme.h"
#include "soft/Music_Emu.h"
#include "soft/Data_Reader.h"
#include "soft/producer.h"
#include "soft/audio_output.h"


class FM_PSG_Soft : public FM_PSG, public Producer<int16_t>
{
    public:
        FM_PSG_Soft(const Project& project);
        ~FM_PSG_Soft();

        typedef struct {
            int treble;                /* -100 to +100 */
            int bass;                  /* -100 to +100 */
        } Equalizer;

    private:
        const Project& _project;

        Equalizer _eq;
        Music_Emu* _emu;
        gme_type_t _type;
        AudioOutput<int16_t>* _output;

        Music_Emu::sample_t _buffer[4096];

        long _position;
        long _positionOffset;
        bool _stopped;

        track_info_t _info;

        VGMStream _vgmStream;
        QMap<int, VGMStream::StreamNoteItem*> _keys;
        bool _startedInteractive;

        QTimer _timer;
        QList<VGMStream::StreamItem*> _items;

        QMutex _mutex;

        // FM_PSG interface
    public:
        float position();
        void setPosition(const float pos);
        void play(const QByteArray& vgm, const bool loop, const int currentOffsetSamples, const int currentOffsetData);
        void play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int currentOffsetSamples, const int currentOffsetData, const float duration);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;
        void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);

        QList<VGMStream::Format> supportedFormats();
        bool requiresHeader() const;

        // Producer interface
    public:
        int16_t* next(int size);
};

#endif // FM_PSG_SOFT_H
