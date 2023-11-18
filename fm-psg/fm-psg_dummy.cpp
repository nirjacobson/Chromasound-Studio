#include "fm-psg_dummy.h"

float FM_PSG_Dummy::position()
{
    float beatNS = nanosecondsPerBeat();

    float pos = (_ref + _timer.nsecsElapsed()) / beatNS;

    if (!_playing) {
        pos = _ref / beatNS;
    }

    if (_duration >= 0) {
        float loopPos = ((float)_loopOffsetSamples / 44100 / 60 * _project.tempo());
        return loopPos + fmod(pos - loopPos, _duration);
    } else if (_project.playMode() == Project::PlayMode::PATTERN) {
        if (_loopOffsetSamples >= 0) {
            float patternLength = _project.getPatternBarLength(_project.frontPattern());

            return fmod(pos, patternLength);
        }
    } else {
        if (_loopOffsetSamples >= 0) {
            float songLength = _project.getLength();
            float loopPos = _project.playlist().loopOffset();
            float loopLength = songLength - loopPos;

            if (pos >= loopPos) {
                return loopPos + fmod(pos - loopPos, loopLength);
            }
        }
    }

    return pos;
}

void FM_PSG_Dummy::setPosition(const float pos)
{
    _ref = pos * nanosecondsPerBeat();
}

bool FM_PSG_Dummy::isPlaying() const
{
    return _playing;
}

void FM_PSG_Dummy::keyOn(const Project&, const Channel::Type, const ChannelSettings&, const int, const int)
{

}

void FM_PSG_Dummy::keyOff(int)
{

}

qint64 FM_PSG_Dummy::nanosecondsPerBeat() const
{
    return 1e9 * 60 / _project.tempo();
}

FM_PSG_Dummy::FM_PSG_Dummy(Project& project)
    : _project(project)
    , _ref(0)
    , _playing(false)
    , _loopOffsetSamples(-1)
    , _duration(-1)
{

}

void FM_PSG_Dummy::play(const QByteArray&, const int loopOffsetSamples, const int, const int, const float duration)
{
    _loopOffsetSamples = loopOffsetSamples;
    _duration = duration;
    _playing = true;
    _timer.restart();
}

void FM_PSG_Dummy::play(const QByteArray&, const bool loop, const int)
{
    _loopOffsetSamples = loop ? 0 : -1;
    _playing = true;
    _timer.restart();
}

void FM_PSG_Dummy::play()
{
    _playing = true;
    _timer.restart();
}

void FM_PSG_Dummy::pause()
{
    _ref += _timer.nsecsElapsed();
    _playing = false;
}

void FM_PSG_Dummy::stop()
{
    _playing = false;
    _ref = 0;
    _duration = -1;
}
