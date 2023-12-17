#ifndef DUMMY_FM_PSG_H
#define DUMMY_FM_PSG_H

#include <QTimer>

#include "fm-psg.h"
#include "project/project.h"

class FM_PSG_Dummy : public FM_PSG
{
    public:
        FM_PSG_Dummy(const Project& project);

        void play(const QByteArray&, const int loopOffsetSamples, const int,  const int, const int, const float duration);
        void play(const QByteArray&, const bool loop, const int, const int);
        void play();
        void pause();
        void stop();
        float position();
        void setPosition(const float pos);
        bool isPlaying() const;

        void keyOn(const Project&, const Channel::Type, const ChannelSettings&, const int, const int);
        void keyOff(int);

        QList<VGMStream::Format> supportedFormats();
        bool requiresHeader() const;
    private:
        const Project& _project;
        QElapsedTimer _timer;
        qint64 _ref;
        bool _playing;
        int _loopOffsetSamples;
        float _duration;

        qint64 nanosecondsPerBeat() const;
};

#endif // DUMMY_FM_PSG_H
