#include "chromasound_direct.h"
#include "direct/gpio.h"

Chromasound_Direct::Chromasound_Direct(const Project& project)
    : _project(project)
    , _timeOffset(0)
{
    _timer.setSingleShot(true);

    _timer.callOnTimeout([&]() {
        QByteArray data;

        _mutex.lock();

        _vgmStream.encode(project, _items, data);

        bool havePCM = false;
        bool haveROM = false;
        for (VGMStream::StreamItem* item : _items) {
            VGMStream::StreamNoteItem* sni;
            if ((sni = dynamic_cast<VGMStream::StreamNoteItem*>(item))) {
                if (sni->on()) {
                    if (sni->type() == Channel::Type::DPCM) {
                        havePCM = true;
                    }
                    if (sni->type() == Channel::Type::SPCM) {
                        haveROM = true;
                    }
                    continue;
                } else {
                    int key = sni->note().key();
                    _keys.remove(key);
                }
            }
            delete item;
        }

        _items.clear();

        _mutex.unlock();

        QFile romFile(project.resolve(project.dpcmFile()));
        romFile.open(QIODevice::ReadOnly);
        QByteArray dataBlock = romFile.readAll();
        romFile.close();

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
        } else if (haveROM) {
            QByteArray pcmBlock;
            pcmBlock.append(0x52);
            pcmBlock.append(0x2B);
            pcmBlock.append(0x80);

            data.prepend(pcmBlock);

            _vgmPlayer->fillWithPCM(true);
        }

        data.prepend(_vgmStream.generateHeader(project, data, 0, 0, 0, false));

        _vgmPlayer->setVGM(data, 0);
    });
    _gpioFd = gpio_init();

    _vgmPlayer = new VGMPlayer(this);
    reset();

    connect(_vgmPlayer, &VGMPlayer::pcmUploadStarted, this, &Chromasound::pcmUploadStarted);
    connect(_vgmPlayer, &VGMPlayer::pcmUploadFinished, this, &Chromasound::pcmUploadFinished);

    _vgmPlayer->start();
}

Chromasound_Direct::~Chromasound_Direct()
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    delete _vgmPlayer;

    gpio_close(_gpioFd);
}

quint32 Chromasound_Direct::position()
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

void Chromasound_Direct::setPosition(const float pos)
{
    _timeOffset = pos / _project.tempo() * 60 * 44100;
}

void Chromasound_Direct::play(const QByteArray& vgm, const int currentOffsetSamples, const int currentOffsetData, const bool isSelection)
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

void Chromasound_Direct::play()
{
    _vgmPlayer->start();
}

void Chromasound_Direct::pause()
{
    _vgmPlayer->pause();
}

void Chromasound_Direct::stop()
{
    _vgmPlayer->stop();
    _vgmPlayer->quit();
    _vgmPlayer->wait();

    reset();

    _timeOffset = 0;

    _vgmPlayer->setMode(VGMPlayer::Mode::Interactive);
    _vgmPlayer->start();
}

bool Chromasound_Direct::isPlaying() const
{
    return _vgmPlayer->isPlaying();
}

void Chromasound_Direct::keyOn(const Project& project, const Channel::Type channelType, const ChannelSettings& settings, const int key, const int velocity)
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

void Chromasound_Direct::keyOff(int key)
{
    _mutex.lock();

    if (!_keys.contains(key)) {
        _mutex.unlock();
        return;
    }

    VGMStream::StreamNoteItem* sni = new VGMStream::StreamNoteItem(*_keys[key]);

    _vgmStream.releaseChannel(sni->type(), sni->channel());
    sni->setOn(false);
    _items.append(sni);

    bool havePCM = false;
    for (auto it = _keys.begin(); it != _keys.end(); ++it) {
        if (it.value()->type() == Channel::Type::DPCM || it.value()->type() == Channel::Type::SPCM) {
            havePCM = true;
            break;
        }
    }

    _mutex.unlock();

    if (!havePCM) {
        _vgmPlayer->fillWithPCM(false);
    }

    _timer.start(20);
}

void Chromasound_Direct::reset()
{
    gpio_write(_gpioFd, 2, 0);
    QThread::usleep(10000);

    gpio_write(_gpioFd, 2, 1);
    QThread::usleep(10000);

    _vgmStream.reset();
}

QList<VGMStream::Format> Chromasound_Direct::supportedFormats()
{
    return QList<VGMStream::Format>({VGMStream::Format::CHROMASOUND, VGMStream::Format::STANDARD});
}
