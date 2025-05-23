#include "chromasound_emu.h"
#include "emu/Vgm_Emu.h"
#include "emu/Data_Reader.h"
#include "Chromasound_Studio.h"

static bool log_err(blargg_err_t err)
{
    if (err)
        fprintf(stderr, "%s\n", err);
    return !!err;
}

static void log_warning(Music_Emu * emu)
{
    const char *str = emu->warning();
    if (str)
        fprintf(stderr, "%s\n", str);
}

Chromasound_Emu::Chromasound_Emu(const Project& project, int readBufferSize)
    : _position(0)
    , _positionOffset(0)
    , _project(project)
    , _startedInteractive(false)
    , _buffer(0)
    , _bufferIdx(0)
    , _player(new Player(*this))
    , _stopped(true)
    , _paused(false)
    , _haveInfo(false)
    , _isSelection(false)
    , _readBufferSize(readBufferSize)
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

    _type = gme_vgm_type;
    _emu = gme_new_emu(_type, 44100);
    _emu->ignore_silence(true);

    _buffers[0] = nullptr;
    _buffers[1] = nullptr;
    setBufferSizes();
    setEqualizer();

    _player->start();
}

Chromasound_Emu::~Chromasound_Emu()
{
    _player->action(Player::Action::Exit);
    _player->quit();
    _player->wait();

    delete _player;

    delete _buffers[0];
    delete _buffers[1];

    delete _vgmStream;

    gme_delete(_emu);
}

void Chromasound_Emu::setEqualizer()
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    int _bass = settings.value(Chromasound_Studio::EqualizerBassKey, 0).toInt();
    int _treble = settings.value(Chromasound_Studio::EqualizerTrebleKey, 0).toInt();

    Music_Emu::equalizer_t eq;

    // bass - logarithmic, 2 to 8194 Hz
    double bass = 1.0 - (_bass / 200.0 + 0.5);
    eq.bass = (long) (2.0 + pow( 2.0, bass * 13 ));

    // treble - -50 to 0 to +5 dB
    double treble = _treble / 100.0;
    eq.treble = treble * (treble < 0 ? 50.0 : 5.0);

    _emu->set_equalizer(eq);
}

void Chromasound_Emu::setBufferSizes()
{
#ifdef Q_OS_WIN
    QSettings settings(Chromasound_Studio::SettingsFile, QSettings::IniFormat);
#else
    QSettings settings(Chromasound_Studio::Organization, Chromasound_Studio::Application);
#endif

    int audioBufferSize = settings.value(Chromasound_Studio::AudioBufferSizeKey, 1024).toInt();

    _framesPerReadBuffer = audioBufferSize * _readBufferSize;

    if (_buffers[0] != nullptr && _buffers[1] != nullptr) {
        delete _buffers[0];
        delete _buffers[1];
    }

    _buffers[0] = new Music_Emu::sample_t[_framesPerReadBuffer * 2];
    _buffers[1] = new Music_Emu::sample_t[_framesPerReadBuffer * 2];
    memset(_buffers[0], 0, _framesPerReadBuffer * 2 * sizeof(int16_t));
    memset(_buffers[1], 0, _framesPerReadBuffer * 2 * sizeof(int16_t));
}

quint32 Chromasound_Emu::position()
{
    quint32 pos = _positionOffset + _position;

    if (!_haveInfo) {
        return pos;
    }

    if (_info.loop_length <= 0) {
        quint32 lengthSamples = _info.length / 1000.0f * 44100;
        if (pos >= lengthSamples) {
            stop();
            return 0;
        }
        return pos;
    }

    quint32 loopLengthSamples = _info.loop_length / 1000.0f * 44100;
    if (_info.intro_length <= 0) {
        if (_isSelection) {
            return _positionOffset + (_position % loopLengthSamples);
        }
        return pos % loopLengthSamples;
    } else {
        quint32 introLengthSamples = _info.intro_length / 1000.0f * 44100;
        return ((pos < introLengthSamples)
                ? pos
                : (((pos - introLengthSamples) % loopLengthSamples) + introLengthSamples));
    }
}

void Chromasound_Emu::setPosition(const float pos)
{
    _positionOffset = pos / _project.tempo() * 60 * 44100;
    _position = 0;
}

void Chromasound_Emu::play(const QByteArray& vgm, const Chromasound_Studio::Profile& profile, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
{
    Vgm_Emu_Impl* impl = dynamic_cast<Vgm_Emu_Impl*>(_emu);

    impl->set_pcm_discrete(profile.discretePCM());

    deactivate();
    Mem_File_Reader reader(vgm.constData(), vgm.size());
    if (log_err(_emu->load(reader)))
        return;

    log_warning(_emu);

    // start track
    if (log_err(_emu->start_track(0)))
        return;

    log_warning(_emu);

    _emu->track_info(&_info);
    _haveInfo = true;

    _isSelection = isSelection;

    _position = 0;
    if (isSelection) {
        _positionOffset = currentOffsetSamples;
    } else {
        if (log_err(_emu->skip(currentOffsetSamples * 2)))
            return;
    }

    _startedInteractive = false;

    setEqualizer();

    _loadLock.lock();
    _bufferIdx = 0;
    _player->buffer(_buffer);
    _player->action(Player::Action::Load);
    _loadLock.unlock();

    _stopped = false;
    _paused = false;
    activate();
}

void Chromasound_Emu::play()
{
    deactivate();
    setEqualizer();

    _stopped = false;
    _paused = false;
    activate();
}

void Chromasound_Emu::pause()
{
    _stopped = true;
    _paused = true;
    deactivate();
}

void Chromasound_Emu::stop()
{
    _stopped = true;
    _paused = false;
    _position = 0;
    _positionOffset = 0;
    _startedInteractive = false;

    _vgmStream->reset();

    QByteArray data;
    data.prepend(_vgmStream->generateHeader(_project, data, -1, 0, 0, false));
    data.append(0x66);
    Mem_File_Reader reader(data.constData(), data.size());

    if (log_err(_emu->load(reader)))
        return;

    log_warning(_emu);

    Vgm_Emu_Impl* impl = dynamic_cast<Vgm_Emu_Impl*>(_emu);
    impl->reset();

    // start track
    if (log_err(_emu->start_track(0)))
        return;

    log_warning(_emu);

    setEqualizer();

    emit stopped();
}

bool Chromasound_Emu::isPlaying() const
{
    return !_stopped;
}

bool Chromasound_Emu::isPaused() const
{
    return _paused;
}

void Chromasound_Emu::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
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

void Chromasound_Emu::keyOff(int key)
{
    if (isPlaying()) return;

    if (!_keys.contains(key)) return;

    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(*_keys[key]);

    _vgmStream->releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    _items.append(sni);
}

void Chromasound_Emu::pitchBend(float pitch, int pitchRange)
{
    QByteArray data;

    for (int key : _keys.keys()) {
        VGMStream::StreamPitchItem* pitchItem = new VGMStream::StreamPitchItem(0, _keys[key]->type(), nullptr, pitch, pitchRange);
        pitchItem->setChannel(_keys[key]->channel());
        _vgmStream->encodePitchItem(pitchItem, _keys[key]->note(), data);
        _items.append(pitchItem);
    }

    Mem_File_Reader reader(data.constData(), data.size());

    _startedInteractive = true;

    if (log_err(_emu->append(reader)))
        return;

}

void Chromasound_Emu::sync()
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
                _keys.remove(sni->note().key());
            }
        }
    }

    if (!_startedInteractive && _project.usesRhythm()) {
        QByteArray enableRhythm;

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x16);
        enableRhythm.append((unsigned char)0x20);

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x17);
        enableRhythm.append((unsigned char)0x50);

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x18);
        enableRhythm.append((unsigned char)0xC0);

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x26);
        enableRhythm.append((unsigned char)0x05);

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x27);
        enableRhythm.append((unsigned char)0x05);

        enableRhythm.append((unsigned char)0x51);
        enableRhythm.append((unsigned char)0x28);
        enableRhythm.append((unsigned char)0x01);

        data.prepend(enableRhythm);
    }

    if (havePCM) {
        if (_profile.pcmStrategy() == Chromasound_Studio::PCMStrategy::INLINE) {
            _items.append(new VGMStream::StreamEndItem(4));
            QByteArray pcm = _vgmStream->encodeStandardPCM(_project, _items);

            QByteArray pcmData;
            quint32 s = pcm.size() / 2;

            for (quint32 i = 0; i < s; i++) {
                pcmData.append(pcm[i * 2]);

            }
            data.append((unsigned char)0x97);
            data.append((unsigned char)0xFE);
            data.append((char*)&s, sizeof(s));
            data.append(pcmData);
        } else if (_profile.pcmStrategy() == Chromasound_Studio::PCMStrategy::SEQUENTIAL) {
            _items.append(new VGMStream::StreamEndItem(4));
            QByteArray pcm = _vgmStream->encodeStandardPCM(_project, _items);
            quint32 dataBlockSize = pcm.size();

            if (dataBlockSize > 0) {
                QByteArray pcmBlock;
                pcmBlock.append((unsigned char)0x67);
                pcmBlock.append((unsigned char)0x66);
                pcmBlock.append((unsigned char)0x00);
                pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                pcmBlock.append(pcm);
                pcmBlock.append((unsigned char)0x52);
                pcmBlock.append((unsigned char)0x2B);
                pcmBlock.append((unsigned char)0x80);

                data.prepend(pcmBlock);

                data.append((unsigned char)0xE0);
                data.append((unsigned char)0x00);
                data.append((unsigned char)0x00);
                data.append((unsigned char)0x00);
                data.append((unsigned char)0x00);

                if (_profile.isChromasound()) {
                    data.append((unsigned char)0x96);
                    data.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                } else {
                    for (quint32 i = 0; i < dataBlockSize; i++) {
                        data.append((unsigned char)0x81);
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
                pcmBlock.append((unsigned char)0x67);
                pcmBlock.append((unsigned char)0x66);
                pcmBlock.append((unsigned char)0x00);
                pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
                pcmBlock.append(dataBlock);
                pcmBlock.append((unsigned char)0x52);
                pcmBlock.append((unsigned char)0x2B);
                pcmBlock.append((unsigned char)0x80);

                data.prepend(pcmBlock);
            }
        }
    }

    _emu->set_fill_past_end_with_pcm(havePCM);

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
        data.prepend(_vgmStream->generateHeader(_project, data, -1, 0, 0, false));
    }

    Mem_File_Reader reader(data.constData(), data.size());

    if (!_startedInteractive) {
        if (log_err(_emu->load(reader)))
            return;
    } else {
        if (log_err(_emu->append(reader)))
            return;
    }

    log_warning(_emu);

    if (!_startedInteractive) {
        // start track
        if (log_err(_emu->start_track(0)))
            return;

        log_warning(_emu);
    }

    if (!_startedInteractive) {
        _startedInteractive = true;
    }
}

int16_t* Chromasound_Emu::next(int size)
{
    if (_bufferIdx == 0) {
        _loadLock.lock();
        _player->buffer(1 - _buffer);
        _player->action(Player::Action::Load);
        _loadLock.unlock();
    }

    int16_t* addr = &_buffers[_buffer][_bufferIdx];

    _bufferIdx += size;
    _bufferIdx %= _framesPerReadBuffer * 2;

    if (!_stopped) {
        _position += size / 2;
    }

    if (_bufferIdx == 0) {
        _buffer = 1 - _buffer;
    }

    return addr;
}

void Chromasound_Emu::setOPLLPatchset(OPLL::Type type)
{
    dynamic_cast<Vgm_Emu*>(_emu)->set_opll_patchset(static_cast<int>(type));
}

Chromasound_Emu::Player::Player(Chromasound_Emu& emu)
    : _emu(emu)
    , _action(Action::Idle)
    , _buffer(emu._buffer)
{

}

void Chromasound_Emu::Player::action(const Action action)
{
    _actionLock.lock();
    _action = action;
    _actionLock.unlock();
}

Chromasound_Emu::Player::Action Chromasound_Emu::Player::action()
{
    _actionLock.lock();
    Action a = _action;
    _actionLock.unlock();

    return a;
}

void Chromasound_Emu::Player::buffer(const int buffer)
{
    _buffer = buffer;
}

void Chromasound_Emu::Player::run()
{
    while (true) {
        Action a = action();

        switch (a) {
            case Idle:
                break;
            case Load:
                _emu._loadLock.lock();
                _emu._emu->play(_emu._framesPerReadBuffer * 2, _emu._buffers[_buffer]);
                _emu._loadLock.unlock();

                if (action() == Action::Exit) {
                    return;
                }

                action(Action::Idle);
                break;
            case Exit:
                return;
        }
    }
}

