#include "chromasound_dual.h"
#include "chromasound_emu.h"

Chromasound_Dual::Chromasound_Dual(Chromasound* chromasound1, Chromasound* chromasound2)
    : _chromasound1(chromasound1)
    , _chromasound2(chromasound2)
{

    connect(_chromasound1, &Chromasound::pcmUploadStarted, this, &Chromasound_Dual::pcmUploadStarted);
    connect(_chromasound1, &Chromasound::pcmUploadFinished, this, &Chromasound_Dual::pcmUploadFinished);
}

Chromasound_Dual::~Chromasound_Dual()
{
    delete _chromasound1;
    delete _chromasound2;
}

quint32 Chromasound_Dual::position()
{
    return _chromasound1->position();
}

void Chromasound_Dual::setPosition(const float pos)
{
    _chromasound1->setPosition(pos);
}

void Chromasound_Dual::play(const QByteArray& vgm, const Chromasound_Studio::Profile profile, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
{
    _chromasound1->play(vgm, profile, currentOffsetSamples, currentOffsetData, isSelection);
}

void Chromasound_Dual::play()
{
    _chromasound1->play();
}

void Chromasound_Dual::pause()
{
    _chromasound1->pause();
}

void Chromasound_Dual::stop()
{
    _chromasound1->stop();
    _chromasound2->stop();
}

bool Chromasound_Dual::isPlaying() const
{
    return _chromasound1->isPlaying();
}

bool Chromasound_Dual::isPaused() const
{
    return _chromasound1->isPaused();
}

void Chromasound_Dual::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    _chromasound2->keyOn(project, channelType, settings, key, velocity);
}

void Chromasound_Dual::keyOff(int key)
{
    _chromasound2->keyOff(key);
}

void Chromasound_Dual::setOPLLPatchset(OPLL::Type type)
{
    Chromasound_Emu* emu;
    if ((emu = dynamic_cast<Chromasound_Emu*>(_chromasound1))) {
        emu->setOPLLPatchset(type);
    }
    if ((emu = dynamic_cast<Chromasound_Emu*>(_chromasound2))) {
        emu->setOPLLPatchset(type);
    }
}
