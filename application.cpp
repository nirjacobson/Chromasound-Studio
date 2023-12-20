#include "application.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _undoStack(this)
    , _paused(false)
    , _ignoreFMPSGTime(false)
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
        QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);
        QString format = settings.value(FM_PSG_Studio::Format, FM_PSG_Studio::FM_PSG).toString();
        VGMStream::Format vgmFormat =
            _fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG) && format == FM_PSG_Studio::FM_PSG
            ? VGMStream::Format::FM_PSG
            : VGMStream::Format::STANDARD;

        if (vgmFormat == VGMStream::Format::FM_PSG) {
            if (_project.playMode() == Project::PlayMode::PATTERN) {
                QThread* thread = QThread::create([&]() {
                    int loopOffsetData;
                    int currentOffsetData;
                    int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                    VGMStream vgmStream;

                    emit compileStarted();
                    QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), _fmPSG->requiresHeader(), false, nullptr, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _fmPSG->play(vgm, true, currentOffsetSamples, currentOffsetData);
                });

                connect(thread, &QThread::finished, this, [=]() {
                    delete thread;
                });

                thread->start();
            } else {
                QThread* thread = QThread::create([&]() {
                    int loopOffsetData;
                    int currentOffsetData;
                    int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                    VGMStream vgmStream;


                    emit compileStarted();
                    QByteArray vgm = vgmStream.compile(_project, _fmPSG->requiresHeader(), false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    if (_project.playlist().doesLoop()) {
                        _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData, currentOffsetSamples, currentOffsetData);
                    } else {
                        _fmPSG->play(vgm, false, currentOffsetSamples, currentOffsetData);
                    }
                });

                connect(thread, &QThread::finished, this, [=]() {
                    delete thread;
                });

                thread->start();
            }
        } else {
            if (_project.playMode() == Project::PlayMode::PATTERN) {
                QThread* thread = QThread::create([&]() {
                    int loopOffsetData;
                    int currentOffsetData;
                    int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                    VGMStream vgmStream(VGMStream::Format::STANDARD);

                    emit compileStarted();
                    QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), _fmPSG->requiresHeader(), false, nullptr, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _fmPSG->play(vgm, true, currentOffsetSamples, currentOffsetData);
                });

                connect(thread, &QThread::finished, this, [=]() {
                    delete thread;
                });

                thread->start();
            } else {
                QThread* thread = QThread::create([&]() {
                    int loopOffsetData;
                    int currentOffsetData;
                    int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                    VGMStream vgmStream(VGMStream::Format::STANDARD);

                    emit compileStarted();
                    QByteArray vgm = vgmStream.compile(_project, _fmPSG->requiresHeader(), false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    if (_project.playlist().doesLoop()) {
                        _fmPSG->play(vgm, _project.playlist().loopOffsetSamples(), loopOffsetData, currentOffsetSamples, currentOffsetData);
                    } else {
                        _fmPSG->play(vgm, false, currentOffsetSamples, currentOffsetData);
                    }
                });

                connect(thread, &QThread::finished, this, [=]() {
                    delete thread;
                });

                thread->start();
            }
        }

    }
    _paused = false;
}

void Application::play(const Pattern& pattern, const float loopStart, const float loopEnd)
{
    QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);
    QString format = settings.value(FM_PSG_Studio::Format, FM_PSG_Studio::FM_PSG).toString();
    VGMStream::Format vgmFormat =
        _fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG) && format == FM_PSG_Studio::FM_PSG
        ? VGMStream::Format::FM_PSG
        : VGMStream::Format::STANDARD;

    if (vgmFormat == VGMStream::Format::FM_PSG) {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream().compile(_project, pattern, _fmPSG->requiresHeader(), false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _fmPSG->setPosition(loopStart);
            _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    } else {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, pattern, _fmPSG->requiresHeader(), false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _fmPSG->setPosition(loopStart);
            _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}

void Application::play(const float loopStart, const float loopEnd)
{
    QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);
    QString format = settings.value(FM_PSG_Studio::Format, FM_PSG_Studio::FM_PSG).toString();
    VGMStream::Format vgmFormat =
        _fmPSG->supportedFormats().contains(VGMStream::Format::FM_PSG) && format == FM_PSG_Studio::FM_PSG
        ? VGMStream::Format::FM_PSG
        : VGMStream::Format::STANDARD;

    if (vgmFormat == VGMStream::Format::FM_PSG) {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream().compile(_project, _fmPSG->requiresHeader(), false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _fmPSG->setPosition(loopStart);
            _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    } else {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, _fmPSG->requiresHeader(), false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _fmPSG->setPosition(loopStart);
            _fmPSG->play(vgm, loopOffsetSamples, loopOffsetData, currentOffsetSamples, currentOffsetData, loopEnd - loopStart);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}
void Application::stop()
{
    _fmPSG->stop();
    _paused = false;
}

float Application::position() const
{
    if (_ignoreFMPSGTime) {
        return 0;
    }

    return _fmPSG->position() / 44100.0f / 60 * _project.tempo();
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

FM_PSG& Application::fmPSG()
{
    return *_fmPSG;
}

void Application::ignoreFMPSGTime(const bool ignore)
{
    _ignoreFMPSGTime = ignore;
}

