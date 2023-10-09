#include "fm-psg_impl.h"

FM_PSG_Impl::FM_PSG_Impl(const Project& project)
    : _project(project)
{
    if (gpioInitialise() < 0) {
      throw "Error initializing pigpio.";
    }

    if ((_spi = spiOpen(0, 2500000, 0)) < 0) {
      throw "Error initializing SPI.";
    }

    _vgmPlayer = new VGMPlayer(_spi, this);
    reset();
}

FM_PSG_Impl::~FM_PSG_Impl()
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    delete _vgmPlayer;

    spiClose(_spi);

    gpioTerminate();
}

float FM_PSG_Impl::position()
{
    return ((float)_vgmPlayer->time() / 44100.0f) / 60.0f * _project.tempo();
}

void FM_PSG_Impl::play(const QByteArray& vgm, const bool loop)
{
    reset();

    _vgmPlayer->setVGM(vgm, loop);
    _vgmPlayer->start();
}

void FM_PSG_Impl::play()
{
    _vgmPlayer->start();
}

void FM_PSG_Impl::pause()
{
    _vgmPlayer->pause();
}

void FM_PSG_Impl::stop()
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();
}

bool FM_PSG_Impl::isPlaying() const
{
    return _vgmPlayer->isRunning();
}

void FM_PSG_Impl::reset()
{
    gpioWrite(2, 0);
    gpioDelay(100);
    gpioWrite(2, 1);
}
