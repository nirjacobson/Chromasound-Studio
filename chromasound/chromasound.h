#ifndef CHROMASOUND_H
#define CHROMASOUND_H

#include <stdio.h>
#include <cstdlib>

#include <QObject>

#include "Chromasound_Studio.h"
#include "project/project.h"
#include "project/channel/channel.h"

class Chromasound : public QObject
{
        Q_OBJECT

    public:
        virtual ~Chromasound() {};

        virtual quint32 position() = 0;
        virtual void setPosition(const float pos) = 0;

        virtual void play(const QByteArray& vgm, const Chromasound_Studio::Profile& profile, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection = false) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;

        virtual bool isPlaying() const = 0;
        virtual bool isPaused() const = 0;

        virtual void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity) = 0;
        virtual void keyOff(int key) = 0;
        virtual void pitchBend(float pitch, int pitchRange) = 0;
        virtual void sync() = 0;

    signals:
        void pcmUploadStarted();
        void pcmUploadFinished();
        void stopped();
};

#endif // CHROMASOUND_H
