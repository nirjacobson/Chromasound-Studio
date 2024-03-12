#include "chromasound_dual.h"

Chromasound_Dual::Chromasound_Dual(Chromasound* chromasound1, Chromasound* chromasound2)
    : _chromasound1(chromasound1)
    , _chromasound2(chromasound2)
{

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

void Chromasound_Dual::play(const QByteArray& vgm, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
{
    _chromasound1->play(vgm, currentOffsetSamples, currentOffsetData, isSelection);
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

void Chromasound_Dual::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    _chromasound2->keyOn(project, channelType, settings, key, velocity);
}

void Chromasound_Dual::keyOff(int key)
{
    _chromasound2->keyOff(key);
}

QList<VGMStream::Format> Chromasound_Dual::supportedFormats()
{
    QList<VGMStream::Format> formats1 = _chromasound1->supportedFormats();
    QList<VGMStream::Format> formats2 = _chromasound2->supportedFormats();

    QSet<VGMStream::Format> formatsSet1(formats1.begin(), formats1.end());
    QSet<VGMStream::Format> formatsSet2(formats2.begin(), formats2.end());
    QSet<VGMStream::Format> intersection = formatsSet1.intersect(formatsSet2);

    QList<VGMStream::Format> intersectionList(intersection.begin(), intersection.end());

    return intersectionList;
}

void Chromasound_Dual::setOPLLPatchset(OPLL::Type type)
{
    _chromasound1->setOPLLPatchset(type);
    _chromasound2->setOPLLPatchset(type);
}
