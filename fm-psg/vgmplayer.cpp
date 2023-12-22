#include "vgmplayer.h"

VGMPlayer::VGMPlayer(int spi, QObject *parent)
    : QThread{parent}
    , _mode(Mode::Interactive)
    , _spi(spi)
    , _time(0)
    , _position(0)
    , _stop(false)
    , _paused(false)
    , _loopOffsetSamples(0)
    , _loopOffsetData(0)
    , _playing(false)
    , _lastPCMBlockChecksum(0)
    , _spiDelay(SPI_DELAY)
{

}

void VGMPlayer::setVGM(const QByteArray& vgm, const bool loop, const int currentOffsetData)
{
    int _currentOffsetData = currentOffsetData;

    _vgmLock.lock();
    _vgm = vgm;
    _vgmLock.unlock();

    if (loop) {
        _loopOffsetSamples = 0;
        _loopOffsetData = 0;
    } else {
        _loopOffsetSamples = -1;
        _loopOffsetData = -1;
    }

    if (_vgm.isEmpty()) {
        return;
    }

    if (vgm[0] == 'V') {
        if (vgm[64] == 0x67) {
            quint32 size = *(quint32*)&vgm.constData()[64 + 3];
            _pcmBlock = vgm.mid(64, 7 + size);
            _vgm = vgm.mid(64 + 7 + size);
            _currentOffsetData -= 64 + 7 + size;
        }
    } else {
        if (vgm[0] == 0x67) {
            quint32 size = *(quint32*)&vgm.constData()[3];
            _pcmBlock = vgm.mid(0, 7 + size);
            _vgm = vgm.mid(7 + size);
            _currentOffsetData -= 7 + size;
        }
    }

    _position = _currentOffsetData;
}

void VGMPlayer::setVGM(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int currentOffsetData)
{
    int _currentOffsetData = currentOffsetData;

    _vgmLock.lock();
    _vgm = vgm;
    _vgmLock.unlock();

    _loopOffsetSamples = loopOffsetSamples;
    _loopOffsetData = loopOffsetData;

    if (_vgm.isEmpty()) {
        return;
    }

    if (vgm[0] == 'V') {
        if (vgm[64] == 0x67) {
            quint32 size = *(quint32*)&vgm.constData()[64 + 3];
            _pcmBlock = vgm.mid(64, 7 + size);
            _vgm = vgm.mid(64 + 7 + size);
            _currentOffsetData -= 64 + 7 + size;
            _loopOffsetData -= 64 + 7 + size;
        }
    } else {
        if (vgm[0] == 0x67) {
            quint32 size = *(quint32*)&vgm.constData()[3];
            _pcmBlock = vgm.mid(0, 7 + size);
            _vgm = vgm.mid(7 + size);
            _currentOffsetData -= 7 + size;
            _loopOffsetData -= 7 + size;
        }
    }

    _position = _currentOffsetData;
}

void VGMPlayer::setMode(const Mode mode)
{
    _mode = mode;

    if (_mode == Mode::Interactive) {
        _vgm.clear();
    }
}

bool VGMPlayer::isPlaying() const
{
    return _playing && isRunning();
}

void VGMPlayer::stop()
{
    _stopLock.lock();
    _stop = true;
    _paused = false;
    _stopLock.unlock();

    _position = 0;
    _playing = false;
}

void VGMPlayer::pause()
{
    _stopLock.lock();
    _stop = true;
    _paused = true;
    _stopLock.unlock();

    _playing = false;
}

uint32_t VGMPlayer::time()
{
    _timeLock.lock();
    uint32_t time = _time;
    _timeLock.unlock();

    if (_playing) {
        return time + ((float)_timer.nsecsElapsed() / 1e9 * 44100);
    } else {
        return time;
    }
}

void VGMPlayer::setTime(const uint32_t time)
{
    _timeLock.lock();
    _time = time;
    _timeLock.unlock();
}

void VGMPlayer::fillWithPCM(const bool enable)
{
    spi_write(enable ? FILL_WITH_PCM : STOP_FILL_WITH_PCM);
}

quint32 VGMPlayer::fletcher32(const QByteArray& data)
{
    quint32 c0, c1, i;
    quint32 len = data.size();
    len = (len + 1) & ~1;

    for (c0 = c1 = i = 0; len > 0; ) {
        quint32 blocklen = len;
        if (blocklen > 360*2) {
            blocklen = 360*2;
        }
        len -= blocklen;

        do {
            c0 = c0 + data[i++];
            c1 = c1 + c0;
        } while ((blocklen -= 2));

        c0 = c0 % 65535;
        c1 = c1 % 65535;
    }

    return ((c1 << 16) | c0);
}

void VGMPlayer::spi_write(char val)
{
    while (_spiTimer.isValid() && _spiTimer.nsecsElapsed() < _spiDelay);
    spiWrite(_spi, &val, 1);
    _spiTimer.restart();
}

void VGMPlayer::spi_xfer(char* tx, char* rx)
{
    while (_spiTimer.isValid() && _spiTimer.nsecsElapsed() < _spiDelay);
    spiXfer(_spi, tx, rx, 1);
    _spiTimer.restart();
}

void VGMPlayer::run() {
    if (_mode == Mode::Interactive) {
        runInteractive();
    } else {
        runPlayback();
    }
}

void VGMPlayer::runInteractive()
{
    char rx, tx;
    uint16_t space;

    _spiDelay = SPI_DELAY_FAST;

    _paused = false;
    _stop = false;

    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        _stopLock.unlock();
        if (stop) {
            break;
        }

        spi_write(REPORT_SPACE);
        spi_xfer(&tx, &rx);
        space = rx;
        spi_xfer(&tx, &rx);
        space |= (int)rx << 8;

        if (space > 0) {
            _vgmLock.lock();
            int remaining = _vgm.size() - _position;
            long count = space < remaining ? space : remaining;

            if (count > 0) {
                spi_write(RECEIVE_DATA);

                for (int i = 0; i < 4; i++) {
                    spi_write(((char*)&count)[i]);
                }

                for (int i = 0; i < count; i++) {
                    spi_write(_vgm[_position++]);
                }
            }
            _vgmLock.unlock();
        }
    }
}

void VGMPlayer::runPlayback()
{
    char rx, tx;
    uint16_t space;

    bool loop = _loopOffsetData >= 0 && _loopOffsetSamples >= 0;
    bool paused = _paused;

    _spiDelay = SPI_DELAY;

    _paused = false;
    _stop = false;

    if (paused) {
        spi_write(PAUSE_RESUME);
    } else {
        spi_write(SET_LOOP_TIME);
        for (int i = 0; i < 4; i++) {
            spi_write(loop ? ((char*)&_loopOffsetSamples)[i] : 0);
        }

        _timeLock.lock();
        spi_write(SET_TIME);
        for (int i = 0; i < 4; i++) {
            spi_write(((char*)&_time)[i]);
        }
        _timeLock.unlock();

        if (!_pcmBlock.isEmpty()) {
            quint32 lastPCMBlockChecksum = _lastPCMBlockChecksum;

            _lastPCMBlockChecksum = fletcher32(_pcmBlock);

            if (lastPCMBlockChecksum != _lastPCMBlockChecksum) {
                emit pcmUploadStarted();
                _spiDelay = SPI_DELAY_FAST;
                uint32_t position = 0;
                while (true) {
                    _stopLock.lock();
                    bool stop = _stop;
                    _stopLock.unlock();
                    if (stop) {
                        return;
                    }

                    spi_write(REPORT_SPACE);
                    spi_xfer(&tx, &rx);
                    space = rx;
                    spi_xfer(&tx, &rx);
                    space |= (int)rx << 8;

                    if (space > 0) {
                        int remaining = _pcmBlock.size() - position;
                        long count = space < remaining ? space : remaining;

                        if (count > 0) {
                            spi_write(RECEIVE_DATA);

                            for (int i = 0; i < 4; i++) {
                                spi_write(((char*)&count)[i]);
                            }

                            for (int i = 0; i < count; i++) {
                                spi_write(_pcmBlock[position++]);
                            }
                        }

                        if (count == remaining) {
                            break;
                        }
                    }
                }
                _spiDelay = SPI_DELAY;
                emit pcmUploadFinished();
            }
        }
    }

    _timer.restart();
    _playing = true;

    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        bool paused = _paused;
        _stopLock.unlock();
        if (stop) {
            spi_write(paused ? PAUSE_RESUME : STOP);
            if (!paused) {
                _timeLock.lock();
                _time = 0;
                _timeLock.unlock();
                _position = 0;
            }
            return;
        }

        spi_write(REPORT_SPACE);
        spi_xfer(&tx, &rx);
        space = rx;
        spi_xfer(&tx, &rx);
        space |= (int)rx << 8;

        if (space > 0) {
            _vgmLock.lock();
            int remaining = _vgm.size() - _position;
            long count = space < remaining ? space : remaining;

            if (count > 0) {
                tx = RECEIVE_DATA;
                spi_xfer(&tx, &rx);

                for (int i = 0; i < 4; i++) {
                    tx = ((char*)&count)[i];
                    spi_xfer(&tx, &rx);
                }

                for (int i = 0; i < count; i++) {
                    tx = _vgm[_position++];
                    spi_xfer(&tx, &rx);

                    if (i > 0) {
                        _spiDelay = !!rx ? SPI_DELAY_SLOW : SPI_DELAY;
                    }
                }
            }
            _vgmLock.unlock();

            if (loop && count == remaining) {
                _position = _loopOffsetData;
            }
        }

        _timeLock.lock();
        spi_write(REPORT_TIME);
        spi_xfer(&tx, &rx);
        _time = rx;
        spi_xfer(&tx, &rx);
        _time |= (int)rx << 8;
        spi_xfer(&tx, &rx);
        _time |= (int)rx << 16;
        spi_xfer(&tx, &rx);
        _time |= (int)rx << 24;
        _timer.restart();
        _timeLock.unlock();
    }
}
