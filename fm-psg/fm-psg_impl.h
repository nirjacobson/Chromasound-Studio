#ifndef FM_PSG_IMPL_H
#define FM_PSG_IMPL_H

#include "fm-psg.h"
#include "project/project.h"

class FM_PSG_Impl : public FM_PSG
{
    public:
        FM_PSG_Impl(const Project& project);
        ~FM_PSG_Impl();

        float position();
        void play(const QByteArray& vgm, const bool loop);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;

    private:
        const Project& _project;
        int _spi;

        VGMPlayer* _vgmPlayer;

        void reset();
};

#endif // FM_PSG_IMPL_H
