#include "fm-psg_soft.h"

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

FM_PSG_Soft::FM_PSG_Soft(const Project& project)
    : _position(0)
    , _positionOffset(0)
    , _project(project)
    , _startedInteractive(false)
{
    _timer.setSingleShot(true);

    _timer.callOnTimeout([&]() {
        QByteArray data;

        _mutex.lock();

        _vgmStream.encode(project, _items, data);

        bool havePCM = false;
        for (VGMStream::StreamItem* item : _items) {
            VGMStream::StreamNoteItem* sni;
            if ((sni = dynamic_cast<VGMStream::StreamNoteItem*>(item))) {
                if (sni->on()) {
                    if (sni->type() == Channel::Type::PCM) {
                        havePCM = true;
                    }
                    continue;
                } else {
                    delete _keys[sni->note().key()];
                    _keys.remove(sni->note().key());
                    delete item;
                    continue;
                }
            }
            delete item;
        }

        _items.clear();

        _mutex.unlock();

        QByteArray dataBlock = project.pcm();
        quint32 dataBlockSize = dataBlock.size();

        if (havePCM && dataBlockSize > 0) {
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

            _emu->set_fill_past_end_with_pcm(true);
        }

        if (!_startedInteractive) {
            data.prepend(_vgmStream.generateHeader(project, data, -1, 0, 0, false));
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
    });

    memset(_buffer, 0, sizeof(_buffer));

    _type = gme_vgm_type;
    _emu = gme_new_emu(_type, 44100);
    _emu->ignore_silence(true);

    setEqualizer();

    _output = AudioOutput<int16_t>::instance();
    _output->producer(this);

    _stopped = true;

    _output->init();
    _output->start();
}

FM_PSG_Soft::~FM_PSG_Soft()
{
    _output->stop();
    _output->destroy();

    gme_delete(_emu);
}

void FM_PSG_Soft::setEqualizer()
{
    QSettings settings(FM_PSG_Studio::Organization, FM_PSG_Studio::Application);

    int _bass = settings.value(FM_PSG_Studio::EqualizerBass, 0).toInt();
    int _treble = settings.value(FM_PSG_Studio::EqualizerTreble, 0).toInt();

    Music_Emu::equalizer_t eq;

    // bass - logarithmic, 2 to 8194 Hz
    double bass = 1.0 - (_bass / 200.0 + 0.5);
    eq.bass = (long) (2.0 + pow( 2.0, bass * 13 ));

    // treble - -50 to 0 to +5 dB
    double treble = _treble / 100.0;
    eq.treble = treble * (treble < 0 ? 50.0 : 5.0);

    _emu->set_equalizer(eq);
}

quint32 FM_PSG_Soft::position()
{
    if (_info.loop_length <= 0) {
        if ((_position + _positionOffset) >= _info.length) {
            stop();
            return 0;
        }
        return (_position + _positionOffset) / 1000.0f * 44100;
    }

    if (_info.intro_length <= 0) {
        return (_positionOffset + (_position % _info.loop_length)) / 1000.0f * 44100;
    } else {
        return (_positionOffset + ((_position < _info.intro_length)
                                   ? _position
                                   : (((_position - _info.intro_length) % _info.loop_length) + _info.intro_length)))
               / 1000.0f * 44100;
    }
}

void FM_PSG_Soft::setPosition(const float pos)
{
    _positionOffset = pos / _project.tempo() * 60 * 1000;
    _position = 0;
}

void FM_PSG_Soft::play(const QByteArray& vgm, const bool loop, const int currentOffsetSamples, const int currentOffsetData)
{
    _output->stop();

    Mem_File_Reader reader(vgm.constData(), vgm.size());
    if (log_err(_emu->load(reader)))
        return;

    log_warning(_emu);

    // start track
    if (log_err(_emu->start_track(0)))
        return;

    log_warning(_emu);

    if (log_err(_emu->skip(currentOffsetSamples * 2)))
        return;

    _emu->track_info(&_info);

    _position = 0;
    _positionOffset = 0;

    _startedInteractive = false;

    setEqualizer();

    _stopped = false;
    _output->start();
}

void FM_PSG_Soft::play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int currentOffsetSamples, const int currentOffsetData, const float duration)
{
    _output->stop();

    Mem_File_Reader reader(vgm.constData(), vgm.size());
    if (log_err(_emu->load(reader)))
        return;

    log_warning(_emu);

    // start track
    if (log_err(_emu->start_track(0)))
        return;

    log_warning(_emu);

    if (duration < 0) {
        if (log_err(_emu->skip(currentOffsetSamples * 2)))
            return;

        _position = 0;
        _positionOffset = 0;
    } else {
        _positionOffset = currentOffsetSamples / 44100.0f * 1000.0f;
        _position = 0;
    }

    _emu->track_info(&_info);

    _startedInteractive = false;

    setEqualizer();

    _stopped = false;
    _output->start();
}

void FM_PSG_Soft::play()
{
    setEqualizer();
    _output->start();
    _stopped = false;
}

void FM_PSG_Soft::pause()
{
    _output->stop();
    _stopped = true;
}

void FM_PSG_Soft::stop()
{
    _stopped = true;
    _position = 0;
    _positionOffset = 0;
    _startedInteractive = false;

    _vgmStream.reset();

    QByteArray data;
    data.prepend(_vgmStream.generateHeader(_project, data, -1, 0, 0, false));
    data.append(0x66);
    Mem_File_Reader reader(data.constData(), data.size());

    if (log_err(_emu->load(reader)))
        return;

    log_warning(_emu);

    // start track
    if (log_err(_emu->start_track(0)))
        return;

    log_warning(_emu);

    setEqualizer();

    emit stopped();
}

bool FM_PSG_Soft::isPlaying() const
{
    return !_stopped;
}

void FM_PSG_Soft::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(0, channelType, nullptr, Note(key, 0, velocity), &settings);

    _keys[key] = sni;

    _mutex.lock();

    VGMStream::StreamLFOItem* sli = new VGMStream::StreamLFOItem(0, project.lfoMode());
    _items.append(sli);

    _items.append(sni);
    _vgmStream.assignChannel(project, sni, _items);

    _mutex.unlock();

    _timer.start(20);
}

void FM_PSG_Soft::keyOff(int key)
{
    if (!_keys.contains(key)) return;

    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(*_keys[key]);
    _vgmStream.releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    _items.append(sni);

    bool havePCM = false;
    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        if (it.value()->type() == Channel::Type::PCM) {
            havePCM = true;
            break;
        }
    }

    if (!havePCM) {
        _emu->set_fill_past_end_with_pcm(false);
    }

    _timer.start(20);
}

int16_t* FM_PSG_Soft::next(int size)
{
    if (!_stopped) {
        _position = _emu->tell();
    }

    _emu->play(size, _buffer);

    return _buffer;
}

QList<VGMStream::Format> FM_PSG_Soft::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::FM_PSG, VGMStream::Format::STANDARD});
}

bool FM_PSG_Soft::requiresHeader() const
{
    return true;
}
