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

    _info.intro_length = -1;
    _info.loop_length = -1;

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

    _stopped = false;
}

void FM_PSG_Soft::play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int currentOffsetSamples, const int currentOffsetData, const float duration)
{
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

    _stopped = false;
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
    _stopped = true;
    _position = 0;
    _positionOffset = 0;
    _info.intro_length = -1;
    _info.loop_length = -1;
    memset(_buffer, 0, sizeof(_buffer));
}

bool FM_PSG_Soft::isPlaying() const
{
    return !_stopped;
}

void FM_PSG_Soft::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    // Not supported
}

void FM_PSG_Soft::keyOff(int key)
{
    // Not supported
}

int16_t* FM_PSG_Soft::next(int size)
{
    if (!_stopped) {
        _emu->play(size, _buffer);
        _position = _emu->tell();
    }

    return _buffer;
}

QList<VGMStream::Format> FM_PSG_Soft::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::STANDARD});
}
