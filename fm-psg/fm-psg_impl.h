#ifndef FM_PSG_IMPL_H
#define FM_PSG_IMPL_H

#include <QMap>
#include <QElapsedTimer>

#include "fm-psg.h"
#include "project/project.h"
#include "project/vgmstream.h"

class FM_PSG_Impl : public FM_PSG
{
    public:
        FM_PSG_Impl(const Project& project);
        ~FM_PSG_Impl();

        float position();
        void setPosition(const float pos);
        void play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int, const int currentOffsetData, const float);
        void play(const QByteArray& vgm, const bool loop, const int, const int currentOffsetData);
        void play();
        void pause();
        void stop();
        bool isPlaying() const;

        void keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity);
        void keyOff(int key);

        QList<VGMStream::Format> supportedFormats();
    private:
        const Project& _project;
        int _spi;

        VGMStream _vgmStream;
        QMap<int, VGMStream::StreamNoteItem*> _keys;

        VGMPlayer* _vgmPlayer;

        void reset();
};

#endif // FM_PSG_IMPL_H
