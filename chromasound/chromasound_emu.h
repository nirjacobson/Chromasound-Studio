#ifndef CHROMASOUND_EMU_H
#define CHROMASOUND_EMU_H

#include <QTimer>
#include <QSettings>

#include "chromasound.h"
#include "project/vgmstream.h"
#include "emu/gme.h"
#include "emu/Music_Emu.h"
#include "emu/Vgm_Emu.h"
#include "emu/Data_Reader.h"
#include "emu/producer.h"
#include "emu/audio_output.h"
#include "Chromasound_Studio.h"
#include "opl/opll.h"

class Chromasound_Emu : public Chromasound, public Producer<int16_t>
{
    public:
    Chromasound_Emu(const Project& project);
        ~Chromasound_Emu();

    private:
        static constexpr int FRAMES_PER_BUFFER = 4096;

        const Project& _project;

        class Player;

        Music_Emu* _emu;
        gme_type_t _type;
        AudioOutput<int16_t>* _output;

        Music_Emu::sample_t _buffers[FRAMES_PER_BUFFER * 2][2];
        int _buffer;
        int _bufferIdx;

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

        QMutex _loadLock;

        Player* _player;

        void setEqualizer();

        // Chromasound interface
    public:
        quint32 position();
        void setPosition(const float pos);
        void play(const QByteArray& vgm, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;
        void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);

        QList<VGMStream::Format> supportedFormats();

    public:
        void setOPLLPatchset(OPLL::Type type);

        // Producer interface
    public:
        int16_t* next(int size);
};

class Chromasound_Emu::Player : public QThread {
    public:
        enum Action {
            Idle,
            Load,
            Exit
        };

        Player(Chromasound_Emu& emu);

        void action(const Action action);
        Action action();

        void buffer(const int buffer);

    private:
        Chromasound_Emu& _emu;

        Action _action;

        int _buffer;

        QMutex _actionLock;


        // QThread interface
    protected:
        void run();
};

#endif // CHROMASOUND_EMU_H
