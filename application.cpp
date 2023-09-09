#include "application.h"

Application* Application::_instance = nullptr;

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _playMode(Pattern)
    , _ref(0)
    , _playing(false)
{

}

float Application::position() const
{
    float beatNS = nanosecondsPerBeat();

    if (!isPlaying()) {
        return _ref / beatNS;
    }

    if (_playMode == Pattern) {
        float patternLength = qCeil(_project.getFrontPattern().getLength()/_project.beatsPerBar()) * _project.beatsPerBar();

        return fmod( ((_ref + _timer.nsecsElapsed()) / beatNS), patternLength );
    }

    return (_ref + _timer.nsecsElapsed()) / beatNS;
}

void Application::pause()
{
    _ref += _timer.nsecsElapsed();
    _playing = false;
}

bool Application::isPlaying() const
{
    return _playing;
}

void Application::play()
{
    _playing = true;
    _timer.restart();
}

void Application::stop()
{
    _playing = false;
    _ref = 0;
}

Project& Application::project()
{
    return _project;
}

Application::PlayMode Application::playMode() const
{
    return _playMode;
}

void Application::setPlayMode(const PlayMode mode)
{
    _playMode = mode;

    if (_playing) {
        _ref = 0;
        _timer.restart();
    }
}

qint64 Application::nanosecondsPerBeat() const
{
    return 1e9 * 60 / _project.tempo();
}

