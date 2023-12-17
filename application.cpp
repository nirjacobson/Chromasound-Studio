#include "application.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _undoStack(this)
    , _paused(false)
    , _mainWindow(nullptr)
{
    const char* fm_psg = std::getenv("FM_PSG");

    if (!fm_psg) {
        _fmPSG = new FM_PSG_Impl(_project);
    } else if (QString(fm_psg).toLower() == "dummy") {
        _fmPSG = new FM_PSG_Dummy(_project);
    } else if (QString(fm_psg).toLower() == "soft") {
        _fmPSG = new FM_PSG_Soft(_project);
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
        int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;

        if (_fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG)) {
            VGMStream vgmStream;

            if (_project.playMode() == Project::PlayMode::PATTERN) {
                QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), false, false, nullptr, -1, -1, position(), &currentOffsetData);
                _fmPSG->play(vgm, true, currentOffsetSamples, currentOffsetData);
            } else {
                int loopOffsetData;
                QByteArray vgm = vgmStream.compile(_project, false, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                if (_project.playlist().doesLoop()) {
                    _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData, currentOffsetSamples, currentOffsetData);
                } else {
                    _fmPSG->play(vgm, false, currentOffsetSamples, currentOffsetData);
                }
            }
        } else {
            VGMStream vgmStream(VGMStream::Format::STANDARD);

            if (_project.playMode() == Project::PlayMode::PATTERN) {
                QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), true, false, nullptr, -1, -1, position(), &currentOffsetData);
                _fmPSG->play(vgm, true, currentOffsetSamples, currentOffsetData);
            } else {
                int loopOffsetData;
                QByteArray vgm = vgmStream.compile(_project, true, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                if (_project.playlist().doesLoop()) {
                    _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData, currentOffsetSamples, currentOffsetData);
                } else {
                    _fmPSG->play(vgm, false, currentOffsetSamples, currentOffsetData);
                }
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
    int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

    if (_fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG)) {
        QByteArray vgm = VGMStream().compile(_project, pattern, false, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

        _fmPSG->setPosition(loopStart);
        _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
    } else {
        QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, pattern, true, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

        _fmPSG->setPosition(loopStart);
        _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
    }
}

void Application::play(const float loopStart, const float loopEnd)
{
    int loopOffsetData;
    int currentOffsetData;
    int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
    int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

    if (_fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG)) {
        QByteArray vgm = VGMStream().compile(_project, false, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

        _fmPSG->setPosition(loopStart);
        _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
    } else {
        QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, true, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);

        _fmPSG->setPosition(loopStart);
        _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
    }
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
    _mainWindow->showMaximized();
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

