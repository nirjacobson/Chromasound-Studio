#ifndef CHROMASOUND_DIRECT_H
#define CHROMASOUND_DIRECT_H

#include <QMap>

#include <QTimer>

#include "chromasound.h"
#include "project/project.h"
#include "project/vgmstream.h"
#include "direct/vgmplayer.h"

class Chromasound_Direct : public Chromasound
{
    public:
        Chromasound_Direct(const Project& project);
        ~Chromasound_Direct();

        quint32 position();
        void setPosition(const float pos);
        void play(const QByteArray& vgm, const Chromasound_Studio::Profile& profile, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection = false);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;
        bool isPaused() const;

        void uploadPCM(const QByteArray& pcm);

        void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);
        void sync();

    private:
        const Project& _project;
        int _gpioFd;

        VGMStream* _vgmStream;
        QMap<int, VGMStream::StreamNoteItem*> _keys;

        QList<VGMStream::StreamItem*> _items;
        VGMPlayer* _vgmPlayer;

        long _timeOffset;

        Chromasound_Studio::Profile _profile;

        bool _startedInteractive;

        void reset();
};

#endif // CHROMASOUND_DIRECT_H
