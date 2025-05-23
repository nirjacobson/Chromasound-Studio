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
    , _profile(Chromasound_Studio::ChromasoundProPreset)
    , _recording(false)
{
    setupChromasound();
}

Application::~Application()
{
    if (_output) {
        _output->stop();
        _output->destroy();
    }

    delete _chromasound;
}

bool Application::isRaspberryPi()
{
    QFile cpuinfo("/proc/cpuinfo");

    if (cpuinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (cpuinfo.isReadable()) {
            QString contents = cpuinfo.readAll();
            QTextStream textStream(&contents);

            while (!textStream.atEnd()) {
                QString line = textStream.readLine();

                if (line.startsWith("Model")) {
                    cpuinfo.close();
                    return line.contains("Raspberry Pi");
                }
            }
        }

        cpuinfo.close();
    } else {
        qDebug() << cpuinfo.errorString();
    }

    return false;
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
        bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
        bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
        bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
        Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
        _profile = Chromasound_Studio::Profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

        if (_project.playMode() == Project::PlayMode::PATTERN) {
            QThread* thread = QThread::create([&]() {
                int currentOffsetData;
                int currentOffsetSamples = position() / _project.tempo() * 60 * 44100;
                VGMStream vgmStream(_profile);

                emit compileStarted();
                QByteArray vgm = vgmStream.compile(_project, _project.getFrontPattern(), false, nullptr, -1, -1, position(), &currentOffsetData);
                emit compileFinished();

                _chromasound->play(vgm, _profile, currentOffsetSamples, currentOffsetData);
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
                VGMStream vgmStream(_profile);


                emit compileStarted();
                QByteArray vgm = vgmStream.compile(_project, false, &loopOffsetData, -1, -1, position(), &currentOffsetData);
                emit compileFinished();

                _chromasound->play(vgm, _profile, currentOffsetSamples, currentOffsetData);
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
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
    _profile = Chromasound_Studio::Profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
        int loopOffsetData;
        int currentOffsetData;
        int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
        int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

        emit compileStarted();
        QByteArray vgm = VGMStream(_profile).compile(_project, pattern, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
        emit compileFinished();

        if (_output) _output->stop();
        _chromasound->setPosition(loopStart);
        _chromasound->play(vgm, _profile, currentOffsetSamples, currentOffsetData, true);
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
    bool isChromasound = settings.value(Chromasound_Studio::IsChromasoundKey, Chromasound_Studio::ChromasoundProPreset.isChromasound()).toBool();
    bool discretePCM = settings.value(Chromasound_Studio::DiscretePCMKey, Chromasound_Studio::ChromasoundProPreset.discretePCM()).toBool();
    bool usePCMSRAM = settings.value(Chromasound_Studio::UsePCMSRAMKey, Chromasound_Studio::ChromasoundProPreset.usePCMSRAM()).toBool();
    Chromasound_Studio::PCMStrategy pcmStrategy = Chromasound_Studio::pcmStrategyFromString(settings.value(Chromasound_Studio::PCMStrategyKey, Chromasound_Studio::ChromasoundProPreset.pcmStrategy()).toString());
    _profile = Chromasound_Studio::Profile(pcmStrategy, isChromasound, discretePCM, usePCMSRAM);

    QThread* thread = QThread::create([&](const float loopStart, const float loopEnd) {
        int loopOffsetData;
        int currentOffsetData;
        int loopOffsetSamples = loopStart / _project.tempo() * 60 * 44100;
        int currentOffsetSamples = loopStart / _project.tempo() * 60 * 44100;

        emit compileStarted();
        QByteArray vgm = VGMStream(_profile).compile(_project, false, &loopOffsetData, loopStart, loopEnd, loopStart, &currentOffsetData);
        emit compileFinished();

        if (_output) _output->stop();
        _chromasound->setPosition(loopStart);
        _chromasound->play(vgm, _profile, currentOffsetSamples, currentOffsetData, true);
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
    _recording = false;
}

void Application::uploadPCM(const QByteArray &pcm)
{
#ifdef Q_OS_LINUX
    Chromasound_Dual* dual;
    Chromasound_Direct* direct;
    if ((direct = dynamic_cast<Chromasound_Direct*>(_chromasound))) {
        direct->uploadPCM(pcm);
    } else if ((dual = dynamic_cast<Chromasound_Dual*>(_chromasound))) {
        if ((direct = dynamic_cast<Chromasound_Direct*>(dual->chromasound1()))) {
            direct->uploadPCM(pcm);
        } else if ((direct = dynamic_cast<Chromasound_Direct*>(dual->chromasound2()))) {
            direct->uploadPCM(pcm);
        }
    }
#endif
}

void Application::record()
{
    _recording = true;
    _recordingMap.clear();
    _recordingTrack.clear();
    _recordingTimer.start();
}

float Application::position() const
{
    if (_ignoreCSTime) {
        return 0;
    }

    if (!_chromasound->isPlaying() && _recording) {
        return _recordingTimer.elapsed() / 1000.0f / 60 * _project.tempo();
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

void Application::keyOn(const Channel &channel, const int key, const int velocity)
{
    if (_recording) {
        _recordingMap[key] = QPair<float, int>(position(), velocity);
    }

    if (channel.enabled()) {
        _chromasound->keyOn(_project, channel.type(), channel.settings(), key, velocity);
    }
}

void Application::keyOn(const Channel &channel, const ChannelSettings &settings, const int key, const int velocity)
{
    if (_recording) {
        _recordingMap[key] = QPair<float, int>(position(), velocity);
    }

    if (channel.enabled()) {
        _chromasound->keyOn(_project, channel.type(), settings, key, velocity);
    }
}

void Application::keyOff(const Channel &channel, int key)
{
    if (_recording && _recordingMap.contains(key)) {
        Note n(key, position() - _recordingMap[key].first, _recordingMap[key].second);

        _recordingTrack.addItem(_recordingMap[key].first, n);

        _recordingMap.remove(key);
    }

    if (channel.enabled()) {
        _chromasound->keyOff(key);
    }
}

void Application::pitchChange(const int index, const float pitch, const int pitchRange)
{
    if (_recording) {
        _recordingTrack.addPitchChange(position(), pitch);
    }

    _chromasound->pitchBend(pitch, pitchRange);
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

    int audioBufferSize = settings.value(Chromasound_Studio::AudioBufferSizeKey, 1024).toInt();

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

        QString outputDevice = settings.value(Chromasound_Studio::OutputDeviceKey, QString::fromStdString(AudioOutput<int16_t>::instance()->devices()[AudioOutput<int16_t>::instance()->defaultDeviceIndex()])).toString();
        _output->open(outputDevice.toStdString(), audioBufferSize);
    }

    if (chromasound1Type == Chromasound_Studio::Emulator) {
        int readBufferSize = settings.value(Chromasound_Studio::ReadBufferSizeKey, 1).toInt();
        chromasound1 = new Chromasound_Emu(_project, readBufferSize);
        _output->producer(dynamic_cast<Chromasound_Emu*>(chromasound1));
    } else {
#ifdef Q_OS_LINUX
        chromasound1 = new Chromasound_Direct(_project);
#endif
    }

    if (numChromasounds == 2) {
        if (chromasound2Type == Chromasound_Studio::Emulator) {
            int readBufferSize = settings.value(chromasound1Type == Chromasound_Studio::Emulator ? Chromasound_Studio::InteractiveReadBufferSizeKey : Chromasound_Studio::ReadBufferSizeKey, 1).toInt();
            chromasound2 = new Chromasound_Emu(_project, readBufferSize);
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

    connect(_chromasound, &Chromasound::pcmUploadStarted, this, &Application::pcmUploadStarted);
    connect(_chromasound, &Chromasound::pcmUploadFinished, this, &Application::pcmUploadFinished);
}

void Application::midiSync()
{
    _chromasound->sync();
}

const Track &Application::recording() const
{
    return _recordingTrack;
}

void Application::clearRecording()
{
    _recordingTrack.clear();
}

