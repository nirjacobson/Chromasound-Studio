#include "fm-psg_impl.h"

FM_PSG_Impl::FM_PSG_Impl(const Project& project)
    : _project(project)
    , _timeOffset(0)
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

            _vgmPlayer->fillWithPCM(true);
        }

        data.prepend(_vgmStream.generateHeader(project, data, 0, 0, 0, false));

        _vgmPlayer->setVGM(data, 0);
    });
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
    uint32_t time = _vgmPlayer->time();
    uint32_t introLength = _vgmPlayer->introLength();
    uint32_t loopLength = _vgmPlayer->loopLength();
    uint32_t length = _vgmPlayer->length();

    if (_vgmPlayer->loopLength() <= 0) {
        if (_vgmPlayer->isPlaying() && (time + _timeOffset) >= _vgmPlayer->length()) {
            stop();
            return 0;
        }
        return (time + _timeOffset);
    }

    if (_vgmPlayer->introLength() <= 0) {
        return (_timeOffset + (time % length));
    } else {
        uint32_t t = _timeOffset + time;
        return ((t < introLength)
                ? t
                : (((t - introLength) % loopLength) + introLength));
    }
}

void FM_PSG_Impl::setPosition(const float pos)
{
    _timeOffset = pos / _project.tempo() * 60 * 44100;
}

void FM_PSG_Impl::play(const QByteArray& vgm, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
{
    if (isSelection) {
        _timeOffset = currentOffsetSamples;
    }

    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _vgmPlayer->setMode(VGMPlayer::Mode::Playback);
    _vgmPlayer->setVGM(vgm, currentOffsetData);
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

    _timeOffset = 0;

    _vgmPlayer->setMode(VGMPlayer::Mode::Interactive);
    _vgmPlayer->start();
}

bool FM_PSG_Impl::isPlaying() const
{
    return _vgmPlayer->isPlaying();
}

void FM_PSG_Impl::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
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

void FM_PSG_Impl::keyOff(int key)
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
        _vgmPlayer->fillWithPCM(false);
    }

    _timer.start(20);
}

void FM_PSG_Impl::reset()
{
    gpioWrite(2, 0);
    gpioDelay(100);

    gpioWrite(2, 1);
    gpioDelay(100);

    _vgmStream.reset();
}

QList<VGMStream::Format> FM_PSG_Impl::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::FM_PSG, VGMStream::Format::STANDARD});
}
