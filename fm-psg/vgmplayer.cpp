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
            _vgm = vgm.mid(0, 64);
            _vgm.append(vgm.mid(64 + 7 + size));
            _currentOffsetData -= 7 + size;
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
            _vgm = vgm.mid(0, 64);
            _vgm.append(vgm.mid(64 + 7 + size));
            _currentOffsetData -= 7 + size;
            _loopOffsetData -= 7 + size;
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
    return _mode == Mode::Playback && isRunning();
}

void VGMPlayer::stop()
{
    _stopLock.lock();
    _stop = true;
    _paused = false;
    _stopLock.unlock();

    _position = 0;
}

void VGMPlayer::pause()
{
    _stopLock.lock();
    _stop = true;
    _paused = true;
    _stopLock.unlock();
}

uint32_t VGMPlayer::time()
{
    _timeLock.lock();
    uint32_t time = _time;
    _timeLock.unlock();

    return time;
}

void VGMPlayer::setTime(const uint32_t time)
{
    _timeLock.lock();
    _time = time;
    _timeLock.unlock();
}

void VGMPlayer::start(Priority p)
{
    if (_paused) {
        spi_write(PAUSE_RESUME);
    } else if (_stop) {
        spi_write(STOP_START);
    }

    _stopLock.lock();
    _stop = false;
    _paused = false;
    _stopLock.unlock();

    QThread::start(p);
}

void VGMPlayer::spi_write(char val)
{
    spiWrite(_spi, &val, 1);
    gpioDelay(SPI_DELAY);
}

void VGMPlayer::spi_xfer(char* tx, char* rx)
{
    spiXfer(_spi, tx, rx, 1);
    gpioDelay(SPI_DELAY);
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

    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        _stopLock.unlock();
        if (stop) {
            spi_write(STOP_START);
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
#include <QtDebug>

void VGMPlayer::runPlayback()
{
    char rx, tx;
    uint16_t space;

    bool loop = _loopOffsetData >= 0 && _loopOffsetSamples >= 0;

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
        uint32_t position = 0;
        while (true) {
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
    }

    bool wait = false;
    while (true) {
        _stopLock.lock();
        bool stop = _stop;
        bool paused = _paused;
        _stopLock.unlock();
        if (stop) {
            spi_write(paused ? PAUSE_RESUME : STOP_START);
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
                        wait = !!rx;
                    }

                    if (wait) {
                        QElapsedTimer timer;
                        timer.start();
                        while (timer.elapsed() < 10) ;
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
        _time = (uint8_t)rx;
        spi_xfer(&tx, &rx);
        _time |= (uint32_t)rx << 8;
        spi_xfer(&tx, &rx);
        _time |= (uint32_t)rx << 16;
        spi_xfer(&tx, &rx);
        _time |= (uint32_t)rx << 24;
        _timeLock.unlock();
    }
}
