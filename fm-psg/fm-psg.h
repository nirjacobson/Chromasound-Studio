#ifndef FM_PSG_H
#define FM_PSG_H

#include <stdio.h>
#include <cstdlib>

#include <QObject>

#include "vgmplayer.h"
#include "project/project.h"
#include "project/vgmstream.h"
#include "project/channel/channel.h"

class FM_PSG : public QObject
{
        Q_OBJECT

    public:
        virtual ~FM_PSG() {};

        virtual quint32 position() = 0;
        virtual void setPosition(const float pos) = 0;

        virtual void play(const QByteArray& vgm, const bool loop, const int currentOffsetSamples, const int currentOffsetData) = 0;
        virtual void play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int currentOffsetSamples, const int currentOffsetData, const float duration = -1) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;

        virtual bool isPlaying() const = 0;

        virtual void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity) = 0;
        virtual void keyOff(int key) = 0;

        virtual QList<VGMStream::Format> supportedFormats() = 0;
        virtual bool requiresHeader() const = 0;

    signals:
        void pcmUploadStarted();
        void pcmUploadFinished();
        void trackEnded();
};

#endif // FM_PSG_H
