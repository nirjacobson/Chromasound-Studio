#include "fm-psg_dummy.h"

quint32 FM_PSG_Dummy::position()
{
    float sampleNS = 1e9 / 44100;

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

QList<VGMStream::Format> FM_PSG_Dummy::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::FM_PSG, VGMStream::Format::STANDARD});
}

qint64 FM_PSG_Dummy::nanosecondsPerBeat() const
{
    return 1e9 * 60 / _project.tempo();
}

bool FM_PSG_Dummy::requiresHeader() const
{
    return false;
}

FM_PSG_Dummy::FM_PSG_Dummy(const Project& project)
    : _project(project)
    , _ref(0)
    , _playing(false)
    , _loopOffsetSamples(-1)
    , _duration(-1)
{

}

void FM_PSG_Dummy::play(const QByteArray&, const int loopOffsetSamples, const int, const int, const int, const float duration)
{
    QTimer::singleShot(3000, [&]() {
        emit pcmUploadFinished();
    });
    emit pcmUploadStarted();

    _loopOffsetSamples = loopOffsetSamples;
    _duration = duration;
    _playing = true;
    _timer.restart();
}

void FM_PSG_Dummy::play(const QByteArray&, const bool loop, const int, const int)
{
    QTimer::singleShot(3000, [&]() {
        emit pcmUploadFinished();
    });
    emit pcmUploadStarted();

    _loopOffsetSamples = loop ? 0 : -1;
    _playing = true;
    _timer.restart();
}

void FM_PSG_Dummy::play()
{
    QTimer::singleShot(3000, [&]() {
        emit pcmUploadFinished();
    });
    emit pcmUploadStarted();

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
