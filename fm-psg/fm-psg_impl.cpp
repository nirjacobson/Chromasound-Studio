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

    connect(_vgmPlayer, &VGMPlayer::pcmUploadStarted, this, &FM_PSG::pcmUploadStarted);
    connect(_vgmPlayer, &VGMPlayer::pcmUploadFinished, this, &FM_PSG::pcmUploadFinished);

    _vgmPlayer->start();
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

quint32 FM_PSG_Impl::position()
{
    return _vgmPlayer->time();
}

void FM_PSG_Impl::setPosition(const float pos)
{
    _vgmPlayer->setTime(pos / _project.tempo() * 60 * 44100);
}

void FM_PSG_Impl::play(const QByteArray& vgm, const int loopOffsetSamples, const int loopOffsetData, const int, const int currentOffsetData, const float)
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _vgmPlayer->setMode(VGMPlayer::Mode::Playback);
    _vgmPlayer->setVGM(vgm, loopOffsetSamples, loopOffsetData, currentOffsetData);
    _vgmPlayer->start();
}

void FM_PSG_Impl::play(const QByteArray& vgm, const bool loop, const int, const int currentOffsetData)
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _vgmPlayer->setMode(VGMPlayer::Mode::Playback);
    _vgmPlayer->setVGM(vgm, loop, currentOffsetData);
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

    reset();

    _vgmPlayer->setMode(VGMPlayer::Mode::Interactive);
    _vgmPlayer->start();
}

bool FM_PSG_Impl::isPlaying() const
{
    return _vgmPlayer->isPlaying();
}

void FM_PSG_Impl::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
{
    VGMStream::StreamLFOItem* sli = new VGMStream::StreamLFOItem(0, project.lfoMode());
    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(0, channelType, nullptr, Note(key, 0, velocity), &settings);
    QList<VGMStream::StreamItem*> items;
    QByteArray data;
    items.append(sli);
    items.append(sni);
    _vgmStream.assignChannel(sni, items);
    _vgmStream.encode(project, items, data);

    if (sni->type() == Channel::Type::PCM) {
        _vgmPlayer->fillWithPCM(true);
    }

    _keys[key] = sni;
    _vgmPlayer->setVGM(data, false, 0);

    delete sli;
}

void FM_PSG_Impl::keyOff(int key)
{
    VGMStream::StreamNoteItem* sni = _keys[key];
    _vgmStream.releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    QList<VGMStream::StreamItem*> items;
    QByteArray data;
    items.append(sni);
    _vgmStream.encode(Project(), items, data);


    bool havePCM = false;
    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        if (it.value()->type() == Channel::Type::PCM) {
            havePCM = true;
            break;
        }
    }

    if (!havePCM) {
        _vgmPlayer->fillWithPCM(false);
    }

    _keys.remove(key);
    _vgmPlayer->setVGM(data, false, 0);

    delete sni;
}

void FM_PSG_Impl::reset()
{
    gpioWrite(2, 0);
    gpioDelay(100);

    gpioWrite(2, 1);
    gpioDelay(100);

    _vgmStream.reset();
}

bool FM_PSG_Impl::requiresHeader() const
{
    return false;
}

QList<VGMStream::Format> FM_PSG_Impl::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::FM_PSG, VGMStream::Format::STANDARD});
}
