#include "vgmplayer.h"
#include "spi.h"

VGMPlayer::VGMPlayer(QObject *parent)
    : QThread{parent}
    , _mode(Mode::Interactive)
    , _time(0)
    , _position(0)
    , _stop(false)
    , _paused(false)
    , _loopOffsetSamples(0)
    , _loopOffsetData(0)
    , _playing(false)
    , _lastPCMBlockChecksum(0)
    , _fillWithPCM(false)
{
    _spiFd = spi_init();
}

VGMPlayer::~VGMPlayer()
{
    spi_close(_spiFd);
}

void VGMPlayer::setVGM(const QByteArray& vgm, const int currentOffsetData)
{
    int _currentOffsetData = currentOffsetData;

    _length = *(quint32*)&vgm.constData()[0x18];
    _loopLength = *(quint32*)&vgm.constData()[0x20];
    _introLength = _length - _loopLength;

    quint32 gd3Offset = *(quint32*)&vgm.constData()[0x14] + 0x14;
    quint32 dataOffset = *(quint32*)&vgm.constData()[0x34] + 0x34;

    _vgmLock.lock();

    if (vgm[dataOffset] == 0x67) {
        quint32 size = *(quint32*)&vgm.constData()[dataOffset + 3];
        _pcmBlock = vgm.mid(dataOffset, 7 + size);
        _vgm = vgm.mid(dataOffset + 7 + size, gd3Offset - dataOffset - 7 - size);
        if (_currentOffsetData != 0) {
            _currentOffsetData -= dataOffset + 7 + size;
        }
    } else {
        _pcmBlock.clear();
        _vgm = vgm.mid(dataOffset, gd3Offset - dataOffset);
        if (_currentOffsetData != 0) {
            _currentOffsetData -= dataOffset;
        }
    }

    _loopOffsetData = *(quint32*)&vgm.constData()[0x1C] + 0x1C - dataOffset - _pcmBlock.size();
    _loopOffsetSamples = _introLength;

    if (_loopOffsetData < 0) {
        _loopOffsetData = -1;
        _loopOffsetSamples = -1;
    }

    _position = _currentOffsetData;

    _vgmLock.unlock();
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

bool VGMPlayer::isPaused() const
{
    return _paused;
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

uint32_t VGMPlayer::length() const
{
    return _length;
}

uint32_t VGMPlayer::introLength() const
{
    return _introLength;
}

uint32_t VGMPlayer::loopLength() const
{
    return _loopLength;
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
    _fillWithPCM = enable;
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

void VGMPlayer::spi_write_wait(uint8_t val)
{
    while (_spiTimer.isValid() && _spiTimer.nsecsElapsed() < SPI_DELAY);
    spi_write(_spiFd, val);
    _spiTimer.start();
}

void VGMPlayer::spi_xfer_wait(uint8_t* tx, uint8_t* rx)
{
    while (_spiTimer.isValid() && _spiTimer.nsecsElapsed() < SPI_DELAY);
    spi_xfer(_spiFd, tx, rx);
    _spiTimer.start();
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
    uint8_t rx, tx;
    uint16_t space;

    _paused = false;
    _stop = false;

    bool fillWithPCM = _fillWithPCM;

    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        _stopLock.unlock();
        if (stop) {
            break;
        }

        spi_write_wait(REPORT_SPACE);
        spi_xfer_wait(&tx, &rx);
        space = rx;
        spi_xfer_wait(&tx, &rx);
        space |= (int)rx << 8;

        if (space > 0) {
            _vgmLock.lock();
            int remaining = _vgm.size() - _position;
            long count = space < remaining ? space : remaining;

            if (count > 0) {
                spi_write_wait(RECEIVE_DATA);

                for (int i = 0; i < 4; i++) {
                    spi_write_wait(((char*)&count)[i]);
                }

                for (int i = 0; i < count; i++) {
                    spi_write_wait(_vgm[_position++]);
                }
            }

            _vgmLock.unlock();
        }

        if (_fillWithPCM != fillWithPCM) {
            spi_write_wait(_fillWithPCM ? FILL_WITH_PCM : STOP_FILL_WITH_PCM);
            fillWithPCM = _fillWithPCM;
        }
    }
}

void VGMPlayer::runPlayback()
{
    uint8_t rx, tx;
    uint16_t space;

    bool loop = _loopOffsetData >= 0 && _loopOffsetSamples >= 0;
    bool paused = _paused;

    _paused = false;
    _stop = false;

    if (paused) {
        spi_write_wait(PAUSE_RESUME);
    } else {
        spi_write_wait(RESET);

        if (!_pcmBlock.isEmpty()) {
            quint32 lastPCMBlockChecksum = _lastPCMBlockChecksum;

            _lastPCMBlockChecksum = fletcher32(_pcmBlock);

            if (lastPCMBlockChecksum != _lastPCMBlockChecksum) {
                emit pcmUploadStarted();
                uint32_t position = 0;
                while (true) {
                    _stopLock.lock();
                    bool stop = _stop;
                    _stopLock.unlock();
                    if (stop) {
                        return;
                    }

                    spi_write_wait(REPORT_SPACE);
                    spi_xfer_wait(&tx, &rx);
                    space = rx;
                    spi_xfer_wait(&tx, &rx);
                    space |= (int)rx << 8;

                    if (space > 0) {
                        int remaining = _pcmBlock.size() - position;
                        long count = space < remaining ? space : remaining;

                        if (count > 0) {
                            spi_write_wait(RECEIVE_DATA);

                            for (int i = 0; i < 4; i++) {
                                spi_write_wait(((char*)&count)[i]);
                            }

                            for (int i = 0; i < count; i++) {
                                spi_write_wait(_pcmBlock[position++]);
                            }
                        }

                        if (count == remaining) {
                            break;
                        }
                    }
                }
                emit pcmUploadFinished();
            }
        }
    }

    _timer.start();
    _playing = true;

    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        bool paused = _paused;
        _stopLock.unlock();
        if (stop) {
            spi_write_wait(paused ? PAUSE_RESUME : STOP);
            if (!paused) {
                _timeLock.lock();
                _time = 0;
                _timeLock.unlock();
                _position = 0;
            }
            return;
        }

        spi_write_wait(REPORT_SPACE);
        spi_xfer_wait(&tx, &rx);
        space = rx;
        spi_xfer_wait(&tx, &rx);
        space |= (int)rx << 8;

        if (space > 0) {
            _vgmLock.lock();
            int remaining = _vgm.size() - _position;
            long count = space < remaining ? space : remaining;

            if (count > 0) {
                tx = RECEIVE_DATA;
                spi_xfer_wait(&tx, &rx);

                for (int i = 0; i < 4; i++) {
                    tx = ((char*)&count)[i];
                    spi_xfer_wait(&tx, &rx);
                }

                for (int i = 0; i < count; i++) {
                    tx = _vgm[_position++];
                    spi_xfer_wait(&tx, &rx);
                }
            }
            _vgmLock.unlock();

            if (loop && count == remaining) {
                _position = _loopOffsetData;
            }
        }

        _timeLock.lock();
        spi_write_wait(REPORT_TIME);
        spi_xfer_wait(&tx, &rx);
        _time = rx;
        spi_xfer_wait(&tx, &rx);
        _time |= (int)rx << 8;
        spi_xfer_wait(&tx, &rx);
        _time |= (int)rx << 16;
        spi_xfer_wait(&tx, &rx);
        _time |= (int)rx << 24;
        _timer.start();
        _timeLock.unlock();
    }
}
