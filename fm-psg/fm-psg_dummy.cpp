#include "fm-psg_dummy.h"

float FM_PSG_Dummy::position()
{
    float beatNS = nanosecondsPerBeat();

    if (!_playing) {
        return _ref / beatNS;
    }

    if (_loop) {
        float patternLength = qCeil(_project.getFrontPattern().getLength()/_project.beatsPerBar()) * _project.beatsPerBar();

        return fmod( ((_ref + _timer.nsecsElapsed()) / beatNS), patternLength );
    }

    return (_ref + _timer.nsecsElapsed()) / beatNS;
}

bool FM_PSG_Dummy::isPlaying() const
{
    return _playing;
}

void FM_PSG_Dummy::keyOn(const Channel::Type, const Settings&, const int, const int)
{

}

void FM_PSG_Dummy::keyOff(int)
{

}

qint64 FM_PSG_Dummy::nanosecondsPerBeat() const
{
    return 1e9 * 60 / _project.tempo();
}

FM_PSG_Dummy::FM_PSG_Dummy(const Project& project)
    : _project(project)
    , _ref(0)
    , _playing(false)
    , _loop(false)
{

}

void FM_PSG_Dummy::play(const QByteArray&, const bool loop)
{
    _loop = loop;
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
}
