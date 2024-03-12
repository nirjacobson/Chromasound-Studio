#ifndef CHROMASOUND_DIRECT_H
#define CHROMASOUND_DIRECT_H

#include <QMap>

#include <QTimer>
#include <QMutex>

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
        void play(const QByteArray& vgm, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection = false);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;

        void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);

        QList<VGMStream::Format> supportedFormats();

    private:
        const Project& _project;
        int _gpioFd;

        VGMStream _vgmStream;
        QMap<int, VGMStream::StreamNoteItem*> _keys;

        QTimer _timer;
        QList<VGMStream::StreamItem*> _items;

        QMutex _mutex;

        VGMPlayer* _vgmPlayer;

        long _timeOffset;

        void reset();
};

#endif // CHROMASOUND_DIRECT_H
