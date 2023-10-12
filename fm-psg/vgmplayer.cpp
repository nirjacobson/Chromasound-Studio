#include "vgmplayer.h"

VGMPlayer::VGMPlayer(int spi, QObject *parent)
    : QThread{parent}
    , _mode(Mode::Interactive)
    , _spi(spi)
    , _time(0)
    , _position(0)
    , _stop(false)
    , _paused(false)
    , _loop(false)
{

}

void VGMPlayer::setVGM(const QByteArray& vgm, const bool loop)
{
    _vgmLock.lock();
    _vgm = vgm;
    _vgmLock.unlock();

    _loop = loop;
    _time = 0;
    _position = 0;
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

void VGMPlayer::runPlayback()
{
    char rx, tx;
    uint16_t space;

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
                spi_write(RECEIVE_DATA);

                for (int i = 0; i < 4; i++) {
                    spi_write(((char*)&count)[i]);
                }

                for (int i = 0; i < count; i++) {
                    spi_write(_vgm[_position++]);
                }
            }
            _vgmLock.unlock();

            if (_loop && count == remaining) {
                _position = 0;
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
        _timeLock.unlock();
    }
}
