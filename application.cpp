#include "application.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _undoStack(this)
    , _paused(false)
    , _mainWindow(nullptr)
{
    const char* dummy_fm_psg = std::getenv("DUMMY_FM_PSG");
    bool dummy = dummy_fm_psg != nullptr;

    if (dummy) {
        _fmPSG = new FM_PSG_Dummy(_project);
    } else {
        _fmPSG = new FM_PSG_Impl(_project);
    }

    connect(_fmPSG, &FM_PSG::pcmUploadStarted, this, &Application::pcmUploadStarted);
    connect(_fmPSG, &FM_PSG::pcmUploadFinished, this, &Application::pcmUploadFinished);
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

bool Application::paused() const
{
    return _paused;
}


void Application::play()
{
    if (_paused) {
        _fmPSG->play();
    } else {
        int currentOffsetData;
        if (_project.playMode() == Project::PlayMode::PATTERN) {
            QByteArray vgm = VGMStream().compile(_project, _project.getFrontPattern(), false, nullptr, -1, -1, position(), &currentOffsetData);
            _fmPSG->play(vgm, true, currentOffsetData);
        } else {
            int loopOffsetData;
            QByteArray vgm = VGMStream().compile(_project, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
            if (_project.playlist().doesLoop()) {
                _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData, currentOffsetData);
            } else {
                _fmPSG->play(vgm, false, currentOffsetData);
            }
        }

    }
    _paused = false;
}

void Application::play(const Pattern& pattern, const float loopStart, const float loopEnd)
{
    int loopOffsetData;
    int currentOffsetData;
    int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
    QByteArray vgm = VGMStream().compile(_project, pattern, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

    _fmPSG->setPosition(loopStart);
    _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetData, loopEnd - loopStart);
}

void Application::play(const float loopStart, const float loopEnd)
{
    int loopOffsetData;
    int currentOffsetData;
    int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
    QByteArray vgm = VGMStream().compile(_project, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

    _fmPSG->setPosition(loopStart);
    _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetData, loopEnd - loopStart);
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

void Application::setPosition(const float pos)
{
    _fmPSG->setPosition(pos);
    _mainWindow->doUpdate();
}

bool Application::isPlaying() const
{
    return _fmPSG->isPlaying();
}

void Application::setWindow(MainWindow* window)
{
    _mainWindow = window;
}

void Application::showWindow()
{
    _mainWindow->show();
}

MainWindow* Application::window()
{
    return _mainWindow;
}

Project& Application::project()
{
    return _project;
}

void Application::keyOn(const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    _fmPSG->keyOn(_project, channelType, settings, key, velocity);
}

void Application::keyOff(int key)
{
    _fmPSG->keyOff(key);
}

QUndoStack& Application::undoStack()
{
    return _undoStack;
}

