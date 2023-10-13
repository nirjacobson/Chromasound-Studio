#include "application.h"

Application* Application::_instance = nullptr;

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _paused(false)
{

    const char* dummy_fm_psg = std::getenv("DUMMY_FM_PSG");
    bool dummy = dummy_fm_psg != nullptr;

    if (dummy) {
        _fmPSG = new FM_PSG_Dummy(_project);
    } else {
        _fmPSG = new FM_PSG_Impl(_project);
    }
}

Application::~Application()
{
    delete _fmPSG;
}


void Application::pause()
{
    _fmPSG->pause();
    _paused = true;
}


void Application::play()
{
    if (_paused) {
        _fmPSG->play();
    } else {
        if (_project.playMode() == Project::PlayMode::PATTERN) {
            _fmPSG->play(VGMStream().compile(_project), true);
        } else {
            if (_project.playlist().doesLoop()) {
                int loopOffsetData;
                QByteArray vgm = VGMStream().compile(_project, false, &loopOffsetData);
                _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData);
            } else {
                _fmPSG->play(VGMStream().compile(_project), false);
            }
        }

    }
    _paused = false;
}

void Application::stop()
{
    _fmPSG->stop();
    _paused = false;
}

float Application::position() const
{
    return _fmPSG->position();
}

bool Application::isPlaying() const
{
    return _fmPSG->isPlaying();
}

Project& Application::project()
{
    return _project;
}

void Application::keyOn(const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    _fmPSG->keyOn(channelType, settings, key, velocity);
}

void Application::keyOff(int key)
{
    _fmPSG->keyOff(key);
}

