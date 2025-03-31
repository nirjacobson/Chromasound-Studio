#include "chromasound_standin.h"

quint32 Chromasound_Standin::position()
{
    float sampleNS = 1e9f / 44100;

    quint32 pos = (_ref + _timer.nsecsElapsed()) / sampleNS;

    if (!_playing) {
        pos = _ref / sampleNS;
    }

    if (_duration >= 0) {
        return _loopOffsetSamples + ((pos - _loopOffsetSamples) % (int)(_duration / _project.tempo() * 60 * 44100));
    } else if (_project.playMode() == Project::PlayMode::PATTERN) {
        if (_loopOffsetSamples >= 0) {
            quint32 patternLength = (float)_project.getPatternBarLength(_project.frontPattern()) / _project.tempo() * 60 * 44100;

            return pos % patternLength;
        }
    } else {
        if (_loopOffsetSamples >= 0) {
            quint32 songLength = _project.getLength() / _project.tempo() * 60 * 44100;
            quint32 loopPos = _project.playlist().loopOffset() / _project.tempo() * 60 * 44100;
            quint32 loopLength = songLength - loopPos;

            if (pos >= loopPos) {
                return loopPos + ((pos - loopPos) % loopLength);
            }
        }
    }

    return pos;
}

void Chromasound_Standin::setPosition(const float pos)
{
    _ref = pos * nanosecondsPerBeat();
}

bool Chromasound_Standin::isPlaying() const
{
    return _playing;
}

bool Chromasound_Standin::isPaused() const
{
    return _paused;
}

void Chromasound_Standin::keyOn(const Project&, const Channel::Type, const ChannelSettings&, const int, const int)
{

}

void Chromasound_Standin::keyOff(int)
{

}

void Chromasound_Standin::sync()
{

}

qint64 Chromasound_Standin::nanosecondsPerBeat() const
{
    return 1e9 * 60 / _project.tempo();
}

Chromasound_Standin::Chromasound_Standin(const Project& project)
    : _project(project)
    , _ref(0)
    , _playing(false)
    , _loopOffsetSamples(-1)
    , _duration(-1)
    , _paused(false)
{

}

void Chromasound_Standin::play(const QByteArray& vgm, const Chromasound_Studio::Profile&, const int, const int, const bool)
{
    QTimer::singleShot(3000, [&]() {
        emit pcmUploadFinished();
    });
    emit pcmUploadStarted();

    quint32 length = *(quint32*)&vgm.constData()[0x18];
    quint32 loopLength = *(quint32*)&vgm.constData()[0x20];
    quint32 introLength = length - loopLength;

    _loopOffsetSamples = introLength;
    _duration = (float)length / 44100 / 60 * _project.tempo();
    _playing = true;
    _paused = false;
    _timer.restart();
}

void Chromasound_Standin::play()
{
    QTimer::singleShot(3000, [&]() {
        emit pcmUploadFinished();
    });
    emit pcmUploadStarted();

    _playing = true;
    _paused = false;
    _timer.restart();
}

void Chromasound_Standin::pause()
{
    _ref += _timer.nsecsElapsed();
    _playing = false;
    _paused = true;
}

void Chromasound_Standin::stop()
{
    _playing = false;
    _paused = false;
    _ref = 0;
    _duration = -1;
}
