#include "application.h"

Application* Application::_instance = nullptr;

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
{

    const char* dummy_fm_psg = std::getenv("DUMMY_FM_PSG");
    bool dummy = dummy_fm_psg != nullptr;

    if (dummy) {
        _fmPSG = new FM_PSG_Dummy(_project);
    } else {
        _fmPSG = new FM_PSG_Impl(_project);
    }
}


void Application::pause()
{
    _fmPSG->pause();
}


void Application::play()
{
    _fmPSG->play();
}

void Application::stop()
{
    _fmPSG->stop();
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

