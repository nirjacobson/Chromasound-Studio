#ifndef CHROMASOUND_STANDIN_H
#define CHROMASOUND_STANDIN_H

#include <QTimer>
#include <QElapsedTimer>

#include "chromasound.h"
#include "project/project.h"

class Chromasound_Standin : public Chromasound
{
    public:
        Chromasound_Standin(const Project& project);

        void play(const QByteArray&vgm, const Chromasound_Studio::Profile&, const int, const int, const bool = false);
        void play();
        void pause();
        void stop();
        quint32 position();
        void setPosition(const float pos);
        bool isPlaying() const;
        bool isPaused() const;

        void keyOn(const Project&, const Channel::Type, const ChannelSettings&, const int, const int);
        void keyOff(int);
        void pitchBend(float pitch, int pitchRange);
        void sync();

    private:
        const Project& _project;
        QElapsedTimer _timer;
        qint64 _ref;
        bool _playing;
        int _loopOffsetSamples;
        float _duration;
        bool _paused;

        qint64 nanosecondsPerBeat() const;
};

#endif // CHROMASOUND_STANDIN_H
