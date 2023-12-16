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
    , _vgmStream(VGMStream::Format::STANDARD)
    , _startedInteractive(false)
{

    memset(_buffer, 0, sizeof(_buffer));

    _audcfg.loop_length = 180;
    _audcfg.resample = false;
    _audcfg.resample_rate = 32000;
    _audcfg.treble = 0;
    _audcfg.bass = 0;
    _audcfg.ignore_spc_length = false;
    _audcfg.echo = 0;
    _audcfg.inc_spc_reverb = false;

    _type = gme_vgm_type;
    _emu = gme_new_emu(_type, 44100);
    _emu->ignore_silence(true);

    // stereo echo depth
    gme_set_stereo_depth(_emu, 1.0 / 100 * _audcfg.echo);

    // set equalizer
    if (_audcfg.treble || _audcfg.bass)
    {
        Music_Emu::equalizer_t eq;

        // bass - logarithmic, 2 to 8194 Hz
        double bass = 1.0 - (_audcfg.bass / 200.0 + 0.5);
        eq.bass = (long) (2.0 + pow( 2.0, bass * 13 ));

        // treble - -50 to 0 to +5 dB
        double treble = _audcfg.treble / 100.0;
        eq.treble = treble * (treble < 0 ? 50.0 : 5.0);

        _emu->set_equalizer(eq);
    }

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

float FM_PSG_Soft::position()
{
    if (_info.loop_length < 0) {
        return (_position + _positionOffset) / 1000.0f / 60.0f * _project.tempo();
    }

    if (_info.intro_length < 0) {
        return (_positionOffset + (_position % _info.loop_length)) / 1000.0f / 60.0f * _project.tempo();
    } else {
        return (_positionOffset + ((_position < _info.intro_length)
                                       ? _position
                                       : (((_position - _info.intro_length) % _info.loop_length) + _info.intro_length)))
               / 1000.0f / 60.0f * _project.tempo();
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

    _stopped = false;
    _output->start();
}

void FM_PSG_Soft::play()
{
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
    _output->stop();

    _stopped = true;
    _position = 0;
    _positionOffset = 0;
    _startedInteractive = false;
}

bool FM_PSG_Soft::isPlaying() const
{
    return !_stopped;
}

void FM_PSG_Soft::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    VGMStream::StreamLFOItem* sli = new VGMStream::StreamLFOItem(0, project.lfoMode());
    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(0, channelType, nullptr, Note(key, 0, velocity), &settings);
    QList<VGMStream::StreamItem*> items;
    QByteArray data;
    items.append(sli);
    items.append(sni);
    _vgmStream.assignChannel(sni, items);
    _vgmStream.encode(project, items, data);
    _keys[key] = sni;

    delete sli;

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
}

void FM_PSG_Soft::keyOff(int key)
{
    if (!_keys.contains(key)) return;

    VGMStream::StreamNoteItem* sni = _keys[key];
    _vgmStream.releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    QList<VGMStream::StreamItem*> items;
    QByteArray data;
    items.append(sni);
    _vgmStream.encode(Project(), items, data);
    _keys.remove(key);

    Mem_File_Reader reader(data.constData(), data.size());

    if (log_err(_emu->append(reader)))
        return;

    log_warning(_emu);
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
    return QList<VGMStream::Format>({VGMStream::Format::STANDARD});
}
