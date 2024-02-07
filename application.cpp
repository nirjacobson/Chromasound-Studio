#include "application.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _undoStack(this)
    , _paused(false)
    , _ignoreCSTime(false)
    , _mainWindow(nullptr)
{
    const char* chromasound = std::getenv("CHROMASOUND");

    if (!chromasound) {
        _chromasound = new Chromasound_Direct(_project);
    } else if (QString(chromasound).toLower() == "standin") {
        _chromasound = new Chromasound_Standin(_project);
    } else if (QString(chromasound).toLower() == "emu") {
        _chromasound = new Chromasound_Emu(_project);
    } else {
        _chromasound = new Chromasound_Direct(_project);
    }

    connect(_chromasound, &Chromasound::pcmUploadStarted, this, &Application::pcmUploadStarted);
    connect(_chromasound, &Chromasound::pcmUploadFinished, this, &Application::pcmUploadFinished);
}

Application::~Application()
{
    delete _chromasound;
}


void Application::pause()
{
    _chromasound->pause();
    _paused = true;
}

bool Application::paused() const
{
    return _paused;
}

void Application::play()
{
    _ignoreCSTime = false;

    if (_paused) {
        _chromasound->play();
    } else {
        QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
        QString format = settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString();
        VGMStream::Format vgmFormat =
            _chromasound->supportedFormats().contains(VGMStream::Format::CHROMASOUND) && format == Chromasound_Studio::Chromasound
            ? VGMStream::Format::CHROMASOUND
            : VGMStream::Format::STANDARD;

        if (vgmFormat == VGMStream::Format::CHROMASOUND) {
            if (_project.playMode() == Project::PlayMode::PATTERN) {
                QThread* thread = QThread::create([&]() {
                    int loopOffsetData;
                    int currentOffsetData;
                    int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                    VGMStream vgmStream;

                    emit compileStarted();
                    QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), false, nullptr, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _chromasound->play(vgm, currentOffsetSamples, currentOffsetData);
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
                    QByteArray vgm = vgmStream.compile(_project, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _chromasound->play(vgm, currentOffsetSamples, currentOffsetData);
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
                    QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), false, nullptr, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _chromasound->play(vgm, currentOffsetSamples, currentOffsetData);
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
                    QByteArray vgm = vgmStream.compile(_project, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                    emit compileFinished();

                    _chromasound->play(vgm, currentOffsetSamples, currentOffsetData);
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
    _ignoreCSTime = false;

    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
    QString format = settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString();
    VGMStream::Format vgmFormat =
        _chromasound->supportedFormats().contains(VGMStream::Format::CHROMASOUND) && format == Chromasound_Studio::Chromasound
        ? VGMStream::Format::CHROMASOUND
        : VGMStream::Format::STANDARD;

    if (vgmFormat == VGMStream::Format::CHROMASOUND) {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream().compile(_project, pattern, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _chromasound->setPosition(loopStart);
            _chromasound->play(vgm, currentOffsetSamples, currentOffsetData, true);
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
            QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, pattern, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _chromasound->setPosition(loopStart);
            _chromasound->play(vgm, currentOffsetSamples, currentOffsetData, true);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}

void Application::play(const float loopStart, const float loopEnd)
{
    _ignoreCSTime = false;

    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
    QString format = settings.value(Chromasound_Studio::Format, Chromasound_Studio::Chromasound).toString();
    VGMStream::Format vgmFormat =
        _chromasound->supportedFormats().contains(VGMStream::Format::CHROMASOUND) && format == Chromasound_Studio::Chromasound
        ? VGMStream::Format::CHROMASOUND
        : VGMStream::Format::STANDARD;

    if (vgmFormat == VGMStream::Format::CHROMASOUND) {
        QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
            int loopOffsetData;
            int currentOffsetData;
            int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
            int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

            emit compileStarted();
            QByteArray vgm = VGMStream().compile(_project, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _chromasound->setPosition(loopStart);
            _chromasound->play(vgm, currentOffsetSamples, currentOffsetData, true);
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
            QByteArray vgm = VGMStream(VGMStream::Format::STANDARD).compile(_project, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
            emit compileFinished();

            _chromasound->setPosition(loopStart);
            _chromasound->play(vgm, currentOffsetSamples, currentOffsetData, true);
        }, loopStart, loopEnd);

        connect(thread, &QThread::finished, this, [=]() {
            delete thread;
        });

        thread->start();
    }
}
void Application::stop()
{
    _chromasound->stop();
    _paused = false;
}

float Application::position() const
{
    if (_ignoreCSTime) {
        return 0;
    }

    return _chromasound->position() / 44100.0f / 60 * _project.tempo();
}

void Application::setPosition(const float pos)
{
    _chromasound->setPosition(pos);
    _mainWindow->doUpdate();
}

bool Application::isPlaying() const
{
    return _chromasound->isPlaying();
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
    _chromasound->keyOn(_project, channelType, settings, key, velocity);
}

void Application::keyOff(int key)
{
    _chromasound->keyOff(key);
}

QUndoStack& Application::undoStack()
{
    return _undoStack;
}

Chromasound& Application::chromasound()
{
    return *_chromasound;
}

void Application::ignoreCSTime(const bool ignore)
{
    _ignoreCSTime = ignore;
}

