#ifndef DUMMY_FM_PSG_H
#define DUMMY_FM_PSG_H

#include "fm-psg.h"
#include "project/project.h"

class FM_PSG_Dummy : public FM_PSG
{
    public:
        FM_PSG_Dummy(const Project& project);

        void play(const QByteArray&, const bool);
        void play();
        void pause();
        void stop();
        float position();
        bool isPlaying() const;

    private:
        const Project& _project;
        QElapsedTimer _timer;
        qint64 _ref;
        bool _playing;

        qint64 nanosecondsPerBeat() const;
};

#endif // DUMMY_FM_PSG_H
