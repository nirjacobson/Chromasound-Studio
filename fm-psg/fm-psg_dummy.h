#ifndef DUMMY_FM_PSG_H
#define DUMMY_FM_PSG_H

#include "fm-psg.h"
#include "project/project.h"

class FM_PSG_Dummy : public FM_PSG
{
    public:
        FM_PSG_Dummy(const Project& project);

        void play(const QByteArray&, const bool loop);
        void play();
        void pause();
        void stop();
        float position();
        bool isPlaying() const;

        void keyOn(const Channel::Type, const ChannelSettings&, const int, const int);
        void keyOff(int);

    private:
        const Project& _project;
        QElapsedTimer _timer;
        qint64 _ref;
        bool _playing;
        bool _loop;

        qint64 nanosecondsPerBeat() const;
};

#endif // DUMMY_FM_PSG_H
