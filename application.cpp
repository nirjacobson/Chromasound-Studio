#include "application.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
    , _undoStack(this)
    , _paused(false)
    , _ignoreCSTime(false)
    , _mainWindow(nullptr)
    , _output(nullptr)
    , _chromasound(nullptr)
{
    setupChromasound();

    connect(_chromasound, &Chromasound::pcmUploadStarted, this, &Application::pcmUploadStarted);
    connect(_chromasound, &Chromasound::pcmUploadFinished, this, &Application::pcmUploadFinished);
}

Application::~Application()
{
    if (_output) {
        _output->stop();
        _output->destroy();
    }

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
#ifdef Q_OS_WIN
        QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
        QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
        bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
        bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
        bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
        Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
        Chromasound_Studio::Profile profile(isChromasound, pcmStrategy, discretePCM, usePCMSRAM);

        if (_project.usesOPL() && _project.hasPCM()) {
            Chromasound_Emu* emu;
            if ((emu = dynamic_cast<Chromasound_Emu*>(_chromasound))) {
                emu->setPCMData(_project.pcmFile().toStdString().c_str());
            }
        }


        if (_project.playMode() == Project::PlayMode::PATTERN) {
            QThread* thread = QThread::create([&]() {
                int currentOffsetData;
                int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                VGMStream vgmStream(profile);

                emit compileStarted();
                QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), false, nullptr, -1, -1, position(), &currentOffsetData);
                emit compileFinished();

                _chromasound->play(vgm, profile, currentOffsetSamples, currentOffsetData);
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
                VGMStream vgmStream(profile);


                emit compileStarted();
                QByteArray vgm = vgmStream.compile(_project, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                emit compileFinished();

                _chromasound->play(vgm, profile, currentOffsetSamples, currentOffsetData);
            });

            connect(thread, &QThread::finished, this, [=]() {
                delete thread;
            });

            thread->start();
        }
    }
    _paused = false;
}

void Application::play(const Pattern& pattern, const float loopStart, const float loopEnd)
{
    _ignoreCSTime = false;

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
    Chromasound_Studio::Profile profile(isChromasound, pcmStrategy, discretePCM, usePCMSRAM);

    if (_project.usesOPL() && _project.hasPCM()) {
        Chromasound_Emu* emu;
        if ((emu = dynamic_cast<Chromasound_Emu*>(_chromasound))) {
            emu->setPCMData(_project.pcmFile().toStdString().c_str());
        }
    }

    QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
        int loopOffsetData;
        int currentOffsetData;
        int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
        int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

        emit compileStarted();
        QByteArray vgm = VGMStream(profile).compile(_project, pattern, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
        emit compileFinished();

        if (_output) _output->stop();
        _chromasound->setPosition(loopStart);
        _chromasound->play(vgm, profile, currentOffsetSamples, currentOffsetData, true);
        if (_output) _output->start();
    }, loopStart, loopEnd);

    connect(thread, &QThread::finished, this, [=]() {
        delete thread;
    });

    thread->start();
}

void Application::play(const float loopStart, const float loopEnd)
{
    _ignoreCSTime = false;

#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, true).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, false).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, false).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::Random).toString());
    Chromasound_Studio::Profile profile(isChromasound, pcmStrategy, discretePCM, usePCMSRAM);

    if (_project.usesOPL() && _project.hasPCM()) {
        Chromasound_Emu* emu;
        if ((emu = dynamic_cast<Chromasound_Emu*>(_chromasound))) {
            emu->setPCMData(_project.pcmFile().toStdString().c_str());
        }
    }

    QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
        int loopOffsetData;
        int currentOffsetData;
        int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
        int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

        emit compileStarted();
        QByteArray vgm = VGMStream(profile).compile(_project, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
        emit compileFinished();

        if (_output) _output->stop();
        _chromasound->setPosition(loopStart);
        _chromasound->play(vgm, profile, currentOffsetSamples, currentOffsetData, true);
        if (_output) _output->start();
    }, loopStart, loopEnd);

    connect(thread, &QThread::finished, this, [=]() {
        delete thread;
    });

    thread->start();
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

void Application::setupChromasound()
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    int audioBufferSize = settings.value(Chromasound_Studio::AudioBufferSizeKey, 256).toInt();
    int readBufferSize = settings.value(Chromasound_Studio::ReadBufferSizeKey, 1).toInt();

    int numChromasounds = settings.value(Chromasound_Studio::NumberOfChromasoundsKey, 1).toInt();
    QString chromasound1Type = settings.value(Chromasound_Studio::Chromasound1Key, Chromasound_Studio::Emulator).toString();
    QString chromasound2Type = settings.value(Chromasound_Studio::Chromasound2Key, Chromasound_Studio::Emulator).toString();

    Chromasound* chromasound1 = nullptr;
    Chromasound* chromasound2 = nullptr;

    if (_output) {
        _output->stop();
        _output->destroy();
    }

    if (chromasound1Type == Chromasound_Studio::Emulator || (chromasound2Type == Chromasound_Studio::Emulator && numChromasounds == 2)) {
        _output = AudioOutput<int16_t>::instance();
        _output->init();

        int outputDeviceIndex = settings.value(Chromasound_Studio::OutputDeviceKey, AudioOutput<int16_t>::instance()->defaultDeviceIndex()).toInt();
        _output->open(outputDeviceIndex, audioBufferSize);
    }

    if (chromasound1Type == Chromasound_Studio::Emulator) {
        chromasound1 = new Chromasound_Emu(_project);
        _output->producer(dynamic_cast<Chromasound_Emu*>(chromasound1));
    } else {
#ifdef Q_OS_LINUX
        chromasound1 = new Chromasound_Direct(_project);
#endif
    }

    if (numChromasounds == 2) {
        if (chromasound2Type == Chromasound_Studio::Emulator) {
            chromasound2 = new Chromasound_Emu(_project);
            _output->producer(dynamic_cast<Chromasound_Emu*>(chromasound2));
        } else {
#ifdef Q_OS_LINUX
            chromasound2 = new Chromasound_Direct(_project);
#endif
        }
    }

    if (_chromasound) {
        delete _chromasound;
    }

    if (numChromasounds == 1) {
        _chromasound = chromasound1;
    } else {
        _chromasound = new Chromasound_Dual(chromasound1, chromasound2);
    }

    if (chromasound1Type == Chromasound_Studio::Emulator || (chromasound2Type == Chromasound_Studio::Emulator && numChromasounds == 2)) {
        _output->start();
    }
}

