#include "chromasound_direct.h"
#include "direct/gpio.h"

Chromasound_Direct::Chromasound_Direct(const Project& project)
    : _project(project)
    , _timeOffset(0)
    , _startedInteractive(false)
    , _profile(Chromasound_Studio::ChromasoundProPreset)
{
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

    _vgmStream = new VGMStream(_profile);

    _gpioFd = gpio_init();

    _vgmPlayer = new VGMPlayer(this);
    reset();

    connect(_vgmPlayer, &VGMPlayer::pcmUploadStarted, this, &Chromasound::pcmUploadStarted);
    connect(_vgmPlayer, &VGMPlayer::pcmUploadFinished, this, &Chromasound::pcmUploadFinished);

    _vgmPlayer->setProfile(_profile);
    _vgmPlayer->start();
}

Chromasound_Direct::~Chromasound_Direct()
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    delete _vgmPlayer;

    delete _vgmStream;

    gpio_close(_gpioFd);
}

quint32 Chromasound_Direct::position()
{
    uint32_t time = _vgmPlayer->time();
    uint32_t introLength = _vgmPlayer->introLength();
    uint32_t loopLength = _vgmPlayer->loopLength();
    uint32_t length = _vgmPlayer->length();

    if (_vgmPlayer->loopLength() <= 0) {
        if (_vgmPlayer->isPlaying() && (time + _timeOffset) >= _vgmPlayer->length()) {
            stop();
            return 0;
        }
        return (time + _timeOffset);
    }

    if (_vgmPlayer->introLength() <= 0) {
        return (_timeOffset + (time % length));
    } else {
        uint32_t t = _timeOffset + time;
        return ((t < introLength)
                ? t
                : (((t - introLength) % loopLength) + introLength));
    }
}

void Chromasound_Direct::setPosition(const float pos)
{
    _timeOffset = pos / _project.tempo() * 60 * 44100;
}

void Chromasound_Direct::play(const QByteArray& vgm, const Chromasound_Studio::Profile& profile, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
{
    if (isSelection) {
        _timeOffset = currentOffsetSamples;
    }

    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _vgmPlayer->setProfile(profile);
    _vgmPlayer->setMode(VGMPlayer::Mode::Playback);
    _vgmPlayer->setVGM(vgm, currentOffsetData);
    _vgmPlayer->start();
}

void Chromasound_Direct::play()
{
    _vgmPlayer->start();
}

void Chromasound_Direct::pause()
{
    _vgmPlayer->pause();
}

void Chromasound_Direct::stop()
{
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

    if (_vgmStream) delete _vgmStream;

    _vgmStream = new VGMStream(_profile);

    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _timeOffset = 0;

    _startedInteractive = false;

    _vgmPlayer->setProfile(_profile);
    _vgmPlayer->setMode(VGMPlayer::Mode::Interactive);
    _vgmPlayer->start();
}

bool Chromasound_Direct::isPlaying() const
{
    return _vgmPlayer->isPlaying();
}

bool Chromasound_Direct::isPaused() const
{
    return _vgmPlayer->isPaused();
}

void Chromasound_Direct::uploadPCM(const QByteArray &pcm)
{
    _vgmPlayer->uploadPCM(pcm);
}

void Chromasound_Direct::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    if (isPlaying()) return;

    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(0, channelType, nullptr, Note(key, channelType == Channel::PCM ? 4 : 0, velocity), &settings);
    _keys[key] = sni;

    if (!_startedInteractive) {
        if (project.usesOPN()) {
            VGMStream::StreamLFOItem* sli = new VGMStream::StreamLFOItem(0, project.lfoMode());
            _items.append(sli);
        }

        if (project.usesSSG()) {
            VGMStream::StreamNoiseFrequencyItem* nfi = new VGMStream::StreamNoiseFrequencyItem(0, project.ssgNoiseFrequency());
            _items.append(nfi);

            VGMStream::StreamEnvelopeFrequencyItem* efi = new VGMStream::StreamEnvelopeFrequencyItem(0, project.ssgEnvelopeFrequency());
            _items.append(efi);

            VGMStream::StreamEnvelopeShapeItem* esi = new VGMStream::StreamEnvelopeShapeItem(0, project.ssgEnvelopeShape());
            _items.append(esi);
        }

        if (project.usesOPL()) {
            VGMStream::StreamUserToneItem* uti = new VGMStream::StreamUserToneItem(0, project.userTone());
            _items.append(uti);
        }
    }

    _vgmStream->assignChannel(project, sni, _items);
    _items.append(sni);
}

void Chromasound_Direct::keyOff(int key)
{
    if (isPlaying()) return;

    if (!_keys.contains(key)) return;

    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(*_keys[key]);

    _vgmStream->releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    _items.append(sni);
}

void Chromasound_Direct::sync()
{
    if (isPlaying()) return;

    QByteArray data;

    _vgmStream->encode(_project, _items, data);

    bool havePCM = false;
    for (VGMStream::StreamItem* item : _items) {
        VGMStream::StreamNoteItem* sni;
        if ((sni = dynamic_cast<VGMStream::StreamNoteItem*>(item))) {
            if (sni->on()) {
                if (sni->type() == Channel::Type::PCM) {
                    havePCM = true;
                }
            } else {
                int key = sni->note().key();
                _keys.remove(key);
            }
        }
    }

    if (!_startedInteractive && _project.usesRhythm()) {
        QByteArray enableRhythm;

        enableRhythm.append(0x51);
        enableRhythm.append(0x16);
        enableRhythm.append(0x20);

        enableRhythm.append(0x51);
        enableRhythm.append(0x17);
        enableRhythm.append(0x50);

        enableRhythm.append(0x51);
        enableRhythm.append(0x18);
        enableRhythm.append(0xC0);

        enableRhythm.append(0x51);
        enableRhythm.append(0x26);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x27);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x28);
        enableRhythm.append(0x01);

        data.prepend(enableRhythm);
    }

    if (havePCM) {
        if (_profile.pcmStrategy() == Chromasound_Studio::PCMStrategy::INLINE) {
            _items.append(new VGMStream::StreamEndItem(4));
            QByteArray pcm = _vgmStream->encodeStandardPCM(_project, _items);

            QByteArray pcmData;
            quint32 s = pcm.size() / 2;

            for (int i = 0; i < s; i++) {
                pcmData.append(pcm[i * 2]);

            }
            data.append(0x97);
            data.append(0xFE);
            data.append((char*)&s, sizeof(s));
            data.append(pcmData);
        } else if (_profile.pcmStrategy() == Chromasound_Studio::PCMStrategy::SEQUENTIAL) {
            _items.append(new VGMStream::StreamEndItem(4));
            QByteArray pcm = _vgmStream->encodeStandardPCM(_project, _items);
            quint32 dataBlockSize = pcm.size();

            if (dataBlockSize > 0) {
                QByteArray pcmBlock;
                pcmBlock.append(0x67);
                pcmBlock.append(0x66);
                pcmBlock.append((quint8)0x00);
                pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                pcmBlock.append(pcm);
                pcmBlock.append(0x52);
                pcmBlock.append(0x2B);
                pcmBlock.append(0x80);

                data.prepend(pcmBlock);

                data.append(0xE0);
                data.append((quint8)0x00);
                data.append((quint8)0x00);
                data.append((quint8)0x00);
                data.append((quint8)0x00);

                if (_profile.isChromasound()) {
                    data.append(0x96);
                    data.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                } else {
                    for (quint32 i = 0; i < dataBlockSize; i++) {
                        data.append(0x81);
                    }
                }
            }
        } else if (!_startedInteractive) {
            QFile romFile(_project.pcmFile());
            romFile.open(QIODevice::ReadOnly);
            QByteArray dataBlock = romFile.readAll();
            romFile.close();

            quint32 dataBlockSize = dataBlock.size();

            if (dataBlockSize > 0) {
                QByteArray pcmBlock;
                pcmBlock.append(0x67);
                pcmBlock.append(0x66);
                pcmBlock.append((quint8)0x00);
                pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                pcmBlock.append(dataBlock);
                pcmBlock.append(0x52);
                pcmBlock.append(0x2B);
                pcmBlock.append(0x80);

                data.prepend(pcmBlock);
            }
        }
    }

    _vgmPlayer->fillWithPCM(havePCM);

    for (VGMStream::StreamItem* item : _items) {
        VGMStream::StreamNoteItem* sni;
        if ((sni = dynamic_cast<VGMStream::StreamNoteItem*>(item))) {
            if (sni->on()) {
                continue;
            }
        }
        delete item;
    }

    _items.clear();

    if (!_startedInteractive) {
        data.prepend(_vgmStream->generateHeader(_project, data, 0, 0, 0, false));

        _vgmPlayer->setVGM(data, 0);
    } else {
        _vgmPlayer->addVGM(data);
    }

    if (!_startedInteractive) {
        _startedInteractive = true;
    }
}

void Chromasound_Direct::reset()
{
    gpio_write(_gpioFd, 2, 0);
    QThread::usleep(10000);

    gpio_write(_gpioFd, 2, 1);
    QThread::usleep(10000);

    _vgmStream->reset();
}
