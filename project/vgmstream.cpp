#include "vgmstream.h"

QList<float> VGMStream::frequencies = {
    16.35,
    17.32,
    18.35,
    19.45,
    20.60,
    21.83,
    23.12,
    24.50,
    25.96,
    27.50,
    29.14,
    30.87
};

QList<QList<int>> VGMStream::slotsByAlg = {
    {3},
    {3},
    {3},
    {3},
    {1,3},
    {1,2,3},
    {1,2,3},
    {0,1,2,3}
};

QList<int> VGMStream::ym2612_frequencies = {
    617, 653, 692, 733, 777, 823, 872, 924, 979, 1037, 1099, 1164
};

VGMStream::VGMStream()
{
    reset();
}

void VGMStream::assignChannel(StreamNoteItem* noteItem, QList<StreamItem*>& items)
{
    if (noteItem->type() == Channel::Type::TONE) {
        int channel = acquireToneChannel(noteItem->time(), noteItem->note().duration());
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::NOISE) {
        const NoiseChannelSettings* ncs = dynamic_cast<const NoiseChannelSettings*>(noteItem->channelSettings());
        int channel = acquireNoiseChannel(noteItem->time(), noteItem->note().duration(), ncs, items);
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::FM) {
        const FMChannelSettings* fmcs = dynamic_cast<const FMChannelSettings*>(noteItem->channelSettings());
        int channel = acquireFMChannel(noteItem->time(), noteItem->note().duration(), fmcs, items);
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::PCM) {
        noteItem->setChannel(0);
    }
}

void VGMStream::releaseChannel(const Channel::Type type, const int channel)
{
    if (type == Channel::Type::TONE) {
        _toneChannels[channel].release();
    } else if (type == Channel::Type::NOISE) {
        _noiseChannels[channel].release();
    } else if (type == Channel::Type::FM) {
        _fmChannels[channel].release();
    }
}

void VGMStream::encode(const Project& project, QList<StreamItem*>& items, QByteArray& data)
{
    for (int i = 0; i < items.size(); i++) {
        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);
        }
    }


    std::sort(items.begin(), items.end(), [](const StreamItem* a, const StreamItem* b){
        if (a->time() == b->time()) {
            const StreamNoteItem* an;
            const StreamNoteItem* bn;
            if((an = dynamic_cast<const StreamNoteItem*>(a)) != nullptr) {
                if((bn = dynamic_cast<const StreamNoteItem*>(b)) != nullptr) {
                    return !an->on() && bn->on();
                }
                return false;
            }
            return true;
        }

        return a->time() < b->time();
    });
}

QByteArray VGMStream::compile(Project& project, bool header, int* loopOffsetData, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray headerData;
    QByteArray data;
    int totalSamples;

    int _loopOffsetData = 0;
    int _currentOffsetData = 0;
    if (project.playMode() == Project::PlayMode::PATTERN) {
        processPattern(0, project, project.getFrontPattern(), items);
        assignChannelsAndExpand(items);
        pad(items, project.getPatternBarLength(project.frontPattern()));
        totalSamples = encode(project, items, data, currentOffset, &_currentOffsetData);

        for (StreamItem* item : items) {
            delete item;
        }
    } else {
        generateItems(project, items);
        assignChannelsAndExpand(items);
        pad(items, project.getLength());
        if (project.playlist().doesLoop()) {
            totalSamples = encode(items, data, project.tempo(), project.playlist().loopOffset(), &_loopOffsetData, currentOffset, &_currentOffsetData, false);
        } else {
            totalSamples = encode(project, items, data, currentOffset, &_currentOffsetData);
        }

        for (StreamItem* item : items) {
            delete item;
        }
    }

    if (project.hasPCM()) {
        quint32 pcmSize = project.pcmSize();
        QByteArray pcmBlock;

        pcmBlock.append(0x67);
        pcmBlock.append(0x66);
        pcmBlock.append((quint8)0x00);
        pcmBlock.append((char*)&pcmSize, sizeof(pcmSize));
        pcmBlock.append(project.pcm());

        QByteArray enableDac;
        enableDac.append(0x52);
        enableDac.append(0x2B);
        enableDac.append(0x80);

        data.prepend(enableDac);
        data.prepend(pcmBlock);

        _loopOffsetData += 7 + pcmSize;
        _currentOffsetData += 7 + pcmSize;
    }

    if (header) {
        _loopOffsetData += 64;
        headerData = QByteArray(64, 0);

        // VGM header
        headerData[0] = 'V';
        headerData[1] = 'g';
        headerData[2] = 'm';
        headerData[3] = ' ';

        // EOF
        *(uint32_t*)&headerData[0x4] = data.size() + 64 - 0x4;
        // Version
        headerData[0x8] = 0x50;
        headerData[0x9] = 0x01;
        // SN76489 clock
        *(uint32_t*)&headerData[0xC] = 3579545;
        // GD3 offset
        *(uint32_t*)&headerData[0x14] = data.size() + 64 - 0xC;
        // Total samples
        *(uint32_t*)&headerData[0x18] = totalSamples;
        // Loop offset
        if (project.playMode() == Project::PlayMode::PATTERN) {
            *(uint32_t*)&headerData[0x1C] = 0x40 - 0x1C;
        } else {
            if (project.playlist().doesLoop()) {
                *(uint32_t*)&headerData[0x1C] = _loopOffsetData - 0x1C;
            } else {
                *(uint32_t*)&headerData[0x1C] = 0;
            }
        }
        // Loop # samples
        if (project.playMode() == Project::PlayMode::PATTERN) {
            *(uint32_t*)&headerData[0x20] = totalSamples;
        } else {
            if (project.playlist().doesLoop()) {
                *(uint32_t*)&headerData[0x20] = totalSamples - project.playlist().loopOffsetSamples();
            } else {
                *(uint32_t*)&headerData[0x20] = 0;
            }
        }
        // SN76489AN flags
        *(uint16_t*)&headerData[0x28] = 0x0003;
        headerData[0x2A] = 15;
        // YM2612 clock
        *(uint32_t*)&headerData[0x2C] = 7680000;
        // Data offset
        *(uint32_t*)&headerData[0x34] = 0xC;

        data.prepend(headerData);
    }

    if (project.playlist().doesLoop() && loopOffsetData) {
        *loopOffsetData = _loopOffsetData;
    }

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    return data;
}

QByteArray VGMStream::compile(const Project& project, const Pattern& pattern, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _currentOffsetData;

    processPattern(0, project, pattern, items, loopStart, loopEnd);
    assignChannelsAndExpand(items);
    pad(items, loopEnd - loopStart);
    totalSamples = encode(items, data, project.tempo(), loopStart, nullptr, currentOffset, &_currentOffsetData, true);

    for (StreamItem* item : items) {
        delete item;
    }

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    return data;
}

QByteArray VGMStream::compile(const Project& project, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _currentOffsetData;

    generateItems(project, items, loopStart, loopEnd);
    assignChannelsAndExpand(items);
    pad(items, loopEnd - loopStart);
    totalSamples = encode(items, data, project.tempo(), loopStart, nullptr, currentOffset, &_currentOffsetData, true);

    for (StreamItem* item : items) {
        delete item;
    }

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    return data;
}

void VGMStream::reset()
{
    for (int i = 0; i < TONE_CHANNELS; i++) {
        _toneChannels[i].reset();
    }

    for (int i = 0; i < NOISE_CHANNELS; i++) {
        _noiseChannels[i].reset();
    }

    for (int i = 0; i < FM_CHANNELS; i++) {
        _fmChannels[i].reset();
    }
}

int VGMStream::acquireToneChannel(const float time, const float duration)
{
    for (int i = 0; i < TONE_CHANNELS; i++) {
        if (_toneChannels[i].acquire(time, duration)) {
            return i;
        }
    }

    return -1;
}

int VGMStream::acquireNoiseChannel(const float time, const float duration, const NoiseChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < NOISE_CHANNELS; i++) {
        if (_noiseChannels[i].settings() == *settings) {
            if (_noiseChannels[i].acquire(time, duration)) {
                return i;
            }
        }
    }

    for (int i = 0; i < NOISE_CHANNELS; i++) {
        if (_noiseChannels[i].acquire(time, duration)) {
            _noiseChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

int VGMStream::acquireFMChannel(const float time, const float duration, const FMChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < FM_CHANNELS; i++) {
        if (_fmChannels[i].settings() == *settings) {
            if (_fmChannels[i].acquire(time, duration)) {
                return i;
            }
        }
    }

    for (int i = 0; i < FM_CHANNELS; i++) {
        if (_fmChannels[i].acquire(time, duration)) {
            _fmChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

void VGMStream::processPattern(const float time, const Project& project, const Pattern& pattern, QList<StreamItem*>& items, const float loopStart, const float loopEnd)
{
    for (auto it = pattern.tracks().cbegin(); it != pattern.tracks().cend(); ++it) {
        const Channel& channel = project.getChannel(it.key());
        const Track* track = it.value();

        if (channel.enabled()) {
            processTrack(time, channel, track, items, loopStart, loopEnd);
        }
    }
}

void VGMStream::processTrack(const float time, const Channel& channel, const Track* track, QList<StreamItem*>& items, const float loopStart, const float loopEnd)
{
    QList<Track::Item*> itemsCopy = track->items();
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Track::Item* a, const Track::Item* b){ return a->time() <= b->time(); });

    for (Track::Item* item : itemsCopy) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if ((time + item->time()) < loopStart || (time + item->time()) >= loopEnd) {
                continue;
            }

            if ((time + item->time() + item->duration()) > loopEnd) {
                item->setDuration(loopEnd - item->time());
            }
        }
        items.append(new StreamNoteItem(time + item->time(), channel.type(), item->note(), &channel.settings()));
    }
}

void VGMStream::generateItems(const Project& project, QList<StreamItem*>& items, const float loopStart, const float loopEnd)
{
    QList<Playlist::Item*> itemsCopy(project.playlist().items());
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Playlist::Item* a, const Playlist::Item* b){ return a->time() <= b->time(); });

    for (Playlist::Item* item : itemsCopy) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < item->time() || loopStart > (item->time() + item->duration())) {
                continue;
            }

            processPattern(item->time(), project, project.getPattern(item->pattern()), items, loopStart, loopEnd);
        } else {
            processPattern(item->time(), project, project.getPattern(item->pattern()), items);
        }
    }
}

void VGMStream::assignChannelsAndExpand(QList<StreamItem*>& items)
{
    QList<StreamItem*> itemsCopy = items;
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const StreamItem* a, const StreamItem* b){ return a->time() <= b->time(); });

    for (StreamItem* item : itemsCopy) {
        StreamNoteItem* noteItem = dynamic_cast<StreamNoteItem*>(item);
        assignChannel(noteItem, items);
        StreamNoteItem* noteOffItem = new StreamNoteItem(*noteItem);
        noteOffItem->setTime(noteItem->time() + noteItem->note().duration());
        noteOffItem->setOn(false);
        items.append(noteOffItem);
    }

    std::sort(items.begin(), items.end(), [](const StreamItem* a, const StreamItem* b){
        if (a->time() == b->time()) {
            const StreamNoteItem* an;
            const StreamNoteItem* bn;
            if((an = dynamic_cast<const StreamNoteItem*>(a)) != nullptr) {
                if((bn = dynamic_cast<const StreamNoteItem*>(b)) != nullptr) {
                    return !an->on() && bn->on();
                }
                return false;
            }
            return true;
        }

        return a->time() < b->time();
    });
}

void VGMStream::pad(QList<StreamItem*>& items, const float toDuration)
{
    StreamEndItem* sei = new StreamEndItem(toDuration);
    items.append(sei);
}

int VGMStream::encode(const Project& project, const QList<StreamItem*>& items, QByteArray& data, const float currentTime, int* const currentOffsetData)
{
    float lastTime = 0.0f;
    int totalSamples = 0;
    bool setCurrentOffsetData = false;
    int _currentOffsetData;

    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastPCM = nullptr;
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->time() >= currentTime && !setCurrentOffsetData) {
            _currentOffsetData = data.size();
            setCurrentOffsetData = true;
        }

        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (lastPCM) {
            quint32 delayToEndOfPCM = pcmSize - pcmWritten;
            totalSamples += encodeDelay(qMin(fullDelaySamples, delayToEndOfPCM), data, true);
            pcmWritten += qMin(fullDelaySamples, delayToEndOfPCM);

            if (delayToEndOfPCM < fullDelaySamples) {
                totalSamples += encodeDelay(fullDelaySamples - delayToEndOfPCM, data, false);
            }

            if (pcmWritten == pcmSize) {
                lastPCM = nullptr;
            }
        } else {
            totalSamples += encodeDelay(fullDelaySamples, data, false);
        }

        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);

            if (sni->on() && sni->type() == Channel::Type::PCM) {
                const PCMChannelSettings* channelSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
                lastPCM = sni;
                pcmSize = qMin(QFileInfo(QFile(channelSettings->path())).size(), (qint64)(sni->note().duration() / project.tempo() * 60 * 44100));
                pcmWritten = 0;
            }
        }
    }
    data.append(0x66);

    if (setCurrentOffsetData && currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    return totalSamples;
}

int VGMStream::encode(const QList<StreamItem*>& items,  QByteArray& data, const int tempo, const float loopTime, int* const loopOffsetData, const float currentTime, int* const currentOffsetData, const bool startAtLoop)
{
    float lastTime = startAtLoop ? loopTime : 0.0f;
    int totalSamples = 0;
    bool setLoopOffsetData = false;
    int _loopOffsetData;
    bool setCurrentOffsetData = false;
    int _currentOffsetData;
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->time() >= loopTime && !setLoopOffsetData) {
            _loopOffsetData = data.size();
            setLoopOffsetData = true;
        }

        if (items[i]->time() >= currentTime && !setCurrentOffsetData) {
            _currentOffsetData = data.size();
            setCurrentOffsetData = true;
        }

        totalSamples += encodeDelay(tempo, items[i]->time() - lastTime, data);
        lastTime = items[i]->time();

        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(Project(), sni, data);
        }
    }
    data.append(0x66);

    if (setLoopOffsetData && loopOffsetData) {
        *loopOffsetData = _loopOffsetData;
    }

    if (setCurrentOffsetData && currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    return totalSamples;
}

int VGMStream::encodeDelay(const quint32 samples, QByteArray& data, const bool pcm) {
    quint32 s = samples;

    if (pcm && samples > 8) {
        data.append(0x96);
        data.append((char*)&samples, sizeof(samples));

        return samples;
    }

    while (s > 0) {
        if (pcm) {
            data.append(0x81);
            s--;
        } else {
            if (s == 735) {
                data.append(0x62);
                s = 0;
            } else if (s == 882) {
                data.append(0x63);
                s = 0;
            } else if (s <= 16) {
                data.append(0x70 | (s-1));
                s = 0;
            } else {
                int ss = s < 0xFFFF ? s : 0xFFFF;

                data.append(0x61);
                data.append(ss & 0xFF);
                data.append((ss >> 8) & 0xFF);

                s -= ss;
            }
        }
    }

    return samples;
}

int VGMStream::encodeDelay(const int tempo, const float beats, QByteArray& data)
{
    int samples = beats * 60.0f/tempo * 44100;

    while (samples > 0) {
        if (samples == 735) {
            data.append(0x62);
            samples = 0;
        } else if (samples == 882) {
            data.append(0x63);
            samples = 0;
        } else if (samples <= 16) {
            data.append(0x70 | (samples-1));
            samples = 0;
        } else {
            int s = samples < 0xFFFF ? samples : 0xFFFF;

            data.append(0x61);
            data.append(s & 0xFF);
            data.append((s >> 8) & 0xFF);

            samples -= s;
        }
    }

    return samples;
}

void VGMStream::encodeSettingsItem(const StreamSettingsItem* item, QByteArray& data)
{
    const NoiseChannelSettings* ncs;
    const FMChannelSettings* fmcs;
    if ((ncs = dynamic_cast<const NoiseChannelSettings*>(item->channelSettings())) != nullptr) {
        int fb = ncs->noiseType();
        int nfo = ncs->shiftRate();
        uint8_t datum = 0xE0 | (fb << 2) | nfo;

        data.append(0x50);
        data.append(datum);
    } else if ((fmcs = dynamic_cast<const FMChannelSettings*>(item->channelSettings())) != nullptr) {
        int part = 1 + (item->channel() >= 3);
        int channel = item->channel() % 3;

        uint8_t datum;
        for (int i = 0; i < 4; i++) {
            int offset = (i * 4) + channel;

            datum = (fmcs->operators()[i].dt() << 4) | fmcs->operators()[i].mul();
            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x30 + offset);
            data.append(datum);

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x40 + offset);
            data.append(fmcs->operators()[i].envelopeSettings().t1l());

            datum = (fmcs->operators()[i].rs() << 6) | fmcs->operators()[i].envelopeSettings().ar();
            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x50 + offset);
            data.append(datum);

            datum = (fmcs->operators()[i].am() << 7) | fmcs->operators()[i].envelopeSettings().d1r();
            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x60 + offset);
            data.append(datum);

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x70 + offset);
            data.append(fmcs->operators()[i].envelopeSettings().d2r());

            datum = (fmcs->operators()[i].envelopeSettings().t2l() << 4) | fmcs->operators()[i].envelopeSettings().rr();
            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x80 + offset);
            data.append(datum);
        }

        datum = (fmcs->algorithm().feedback() << 3) | fmcs->algorithm().algorithm();
        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB0 + channel);
        data.append(datum);

        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB4 + channel);
        data.append(0xC0);
    }
}

void VGMStream::encodeNoteItem(const Project& project, const StreamNoteItem* item, QByteArray& data)
{
    if (item->channel() < 0) {
        return;
    }

    int addr;
    if (item->type() == Channel::Type::TONE) {
        if (item->on()) {
            addr = (item->channel() * 2);
            int octave = item->note().key() / 12;
            int key = (item->note().key() - 1) % 12;
            if (key == 11) octave--;
            float f = frequencies[key] * (float)qPow(2, octave);
            int n = 3579545.0f / (32.0f * f);
            uint8_t datum1 = n & 0xF;
            uint8_t datum2 = n >> 4;
            data.append(0x50);
            data.append(0x80 | (addr << 4) | datum1);
            data.append(0x50);
            data.append(datum2);
        }

        addr = (item->channel() * 2) + 1;
        int att;
        if (item->on()) {
            att = (30.0f * (float)(100 - item->channelSettings()->volume())/100.0f);
            att += (30.0f - att) * (float)(100 - item->note().velocity())/100.0f;
            att >>= 1;
        } else {
            att = 0xF;
        }
        data.append(0x50);
        data.append(0x80 | (addr << 4) | att);
    } else if (item->type() == Channel::Type::NOISE) {
        addr = 7;
        int att;
        if (item->on()) {
            att = (30.0f * (float)(100 - item->channelSettings()->volume())/100.0f);
            att += (30.0f - att) * (float)(100 - item->note().velocity())/100.0f;
            att >>= 1;
        } else {
            att = 0xF;
        }
        data.append(0x50);
        data.append(0x80 | (addr << 4) | att);

    } else if (item->type() == Channel::Type::FM) {
        const FMChannelSettings* fmcs = dynamic_cast<const FMChannelSettings*>(item->channelSettings());
        int part = 1 + (item->channel() >= 3);
        int channel = item->channel() % 3;

        if (item->on()) {
            int octave = item->note().key() / 12;
            int key = item->note().key() % 12;
            int n = (octave << 11) | ym2612_frequencies[key];

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0xA4 + channel);
            data.append((n >> 8) & 0xFF);

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0xA0 + channel);
            data.append(n & 0xFF);

            QList<int> sls = slotsByAlg[fmcs->algorithm().algorithm()];
            for (int i = 0; i < sls.size(); i++) {
                int offset = (sls[i] * 4) + channel;
                int t1l = fmcs->operators()[sls[i]].envelopeSettings().t1l();

                int amt = (127 - t1l) *
                          (100 - item->channelSettings()->volume())/100.0f;
                amt += ((127 - t1l) - amt) * (100 - item->note().velocity())/100.0f;
                int newT1l = t1l + amt;

                data.append((part == 1) ? 0x52 : 0x53);
                data.append(0x40 + offset);
                data.append(newT1l);
            }
        }

        data.append(0x52);
        int c = (item->channel() < 3) ? item->channel() : (1 + item->channel());
        data.append(0x28);
        data.append(((item->on() ? 0xF : 0x0) << 4) | c);
    } else if (item->on() && item->type() == Channel::Type::PCM) {
        const PCMChannelSettings* pcmcs = dynamic_cast<const PCMChannelSettings*>(item->channelSettings());
        quint32 offset = project.pcmOffset(pcmcs->path());

        data.append(0xE0);
        data.append((char*)&offset, sizeof(offset));
    }
}

VGMStream::StreamNoteItem::StreamNoteItem(const float time, const Channel::Type type, const Note& note, const ChannelSettings* channelSettings)
    : StreamItem(time)
    , _type(type)
    , _channel(-1)
    , _note(note)
    , _channelSettings(channelSettings)
    , _on(true)
{

}

void VGMStream::StreamNoteItem::setChannel(const int channel)
{
    _channel = channel;
}

void VGMStream::StreamNoteItem::setOn(const bool on)
{
    _on = on;
}

Channel::Type VGMStream::StreamNoteItem::type() const
{
    return _type;
}

const Note& VGMStream::StreamNoteItem::note() const
{
    return _note;
}

const ChannelSettings* VGMStream::StreamNoteItem::channelSettings() const
{
    return _channelSettings;
}

int VGMStream::StreamNoteItem::channel() const
{
    return _channel;
}

bool VGMStream::StreamNoteItem::on() const
{
    return _on;
}

VGMStream::StreamItem::~StreamItem()
{

}

VGMStream::StreamItem::StreamItem(const float time)
    : _time(time)
{

}

float VGMStream::StreamItem::time() const
{
    return _time;
}

void VGMStream::StreamItem::setTime(const float time)
{
    _time = time;
}

bool VGMStream::PhysicalChannel::acquire(float time, float duration)
{
    if (_acquiredIndefinitely) {
        return false;
    }

    if (_duration > 0 && _time <= time && time < (_time+_duration)) {
        return false;
    }

    _time = time;
    _duration = duration;

    if (duration == 0) {
        _acquiredIndefinitely = true;
    }

    return true;
}

void VGMStream::PhysicalChannel::release()
{
    _acquiredIndefinitely = false;
}

void VGMStream::PhysicalChannel::reset()
{
    _time = 0;
    _duration = 0;
    _acquiredIndefinitely = false;
}

VGMStream::StreamSettingsItem::StreamSettingsItem(const float time, const int channel, const Settings* channelSettings)
    : StreamItem(time)
    , _channel(channel)
    , _channelSettings(channelSettings)
{

}

const Settings* VGMStream::StreamSettingsItem::channelSettings() const
{
    return _channelSettings;
}

int VGMStream::StreamSettingsItem::channel() const
{
    return _channel;
}

FMChannelSettings& VGMStream::FMChannel::settings()
{
    return _settings;
}

NoiseChannelSettings& VGMStream::NoiseChannel::settings()
{
    return _settings;
}

VGMStream::StreamEndItem::StreamEndItem(const float time)
    : StreamItem(time)
{

}
