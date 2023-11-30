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

VGMStream::VGMStream(const Format format)
    : _format(format)
{
    reset();
}

VGMStream::~VGMStream()
{
    for (Track::SettingsChange* sc : _createdSCs) {
        delete sc;
    }
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
        int channel = acquirePCMChannel(noteItem->time(), noteItem->note().duration());
        noteItem->setChannel(channel);
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
        StreamLFOItem* sli;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);
        } else if ((sli = dynamic_cast<StreamLFOItem*>(items[i])) != nullptr) {
            encodeLFOItem(sli, data);
        }
    }

    sortItems(items);
}

QByteArray VGMStream::compile(Project& project, const Pattern& pattern, bool header, int* loopOffsetData, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _loopOffsetData = 0;
    int _currentOffsetData = 0;

    processPattern(0, project, pattern, items, loopStart, loopEnd);
    applySettingsChanges(project, 0, pattern, items);
    assignChannelsAndExpand(items, project.tempo());
    applySettingsChanges2(project, 0, pattern, items);
    addSettingsAtCurrentOffset(items, currentOffset);

    sortItems(items);

    if (loopStart >= 0 && loopEnd >= 0) {
        pad(items, loopEnd);
        items.prepend(new StreamLFOItem(loopStart, project.lfoMode()));
        totalSamples = encode(project, items, data, loopStart, nullptr, currentOffset, &_currentOffsetData, true);
    } else {
        pad(items, project.getPatternBarLength(pattern));
        items.prepend(new StreamLFOItem(0, project.lfoMode()));
        totalSamples = encode(project, items, data, 0, nullptr, currentOffset, &_currentOffsetData, true);
    }

    for (StreamItem* item : items) {
        delete item;
    }

    if (project.hasPCM()) {
        QByteArray enableDac;
        enableDac.append(0x52);
        enableDac.append(0x2B);
        enableDac.append(0x80);

        data.insert((qsizetype)_currentOffsetData, enableDac);

        if (_loopOffsetData >= _currentOffsetData) {
            _loopOffsetData += 3;
        }

        QByteArray dataBlock;
        quint32 dataBlockSize;
        QByteArray pcmBlock;
        if (_format == Format::FM_PSG) {
            dataBlock = project.pcm();
        } else {
            dataBlock = encodeStandardPCM(project, pattern, loopStart, loopEnd);
        }
        dataBlockSize = dataBlock.size();

        pcmBlock.append(0x67);
        pcmBlock.append(0x66);
        pcmBlock.append((quint8)0x00);
        pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
        pcmBlock.append(dataBlock);

        _loopOffsetData += 7 + dataBlockSize;
        _currentOffsetData += 7 + dataBlockSize;

        data.prepend(pcmBlock);
    }

    if (header) {
        _loopOffsetData += 64;
        _currentOffsetData += 64;
        QByteArray headerData = generateHeader(project, data, totalSamples, _loopOffsetData);
        data.prepend(headerData);
    }

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    if (loopOffsetData) {
        *loopOffsetData = _loopOffsetData;
    }

    return data;
}

QByteArray VGMStream::compile(Project& project, const bool header, int* loopOffsetData, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _loopOffsetData = 0;
    int _currentOffsetData = 0;

    processProject(project, items, loopStart, loopEnd);
    applySettingsChanges(project, items);
    assignChannelsAndExpand(items, project.tempo());
    applySettingsChanges2(project, items);
    addSettingsAtCurrentOffset(items, currentOffset);

    if (loopStart >= 0 && loopEnd >= 0) {
        pad(items, loopEnd);
    } else {
        pad(items, project.getLength());
    }

    QList<Playlist::LFOChange*> lfoChanges = project.playlist().lfoChanges();

    std::sort(lfoChanges.begin(), lfoChanges.end(), [](Playlist::LFOChange* a, Playlist::LFOChange* b) {
        return a->time() < b->time();
    });

    auto mostRecentLFOChangeIt = std::find_if(project.playlist().lfoChanges().rbegin(),
                                              project.playlist().lfoChanges().rend(),
                                              [&](Playlist::LFOChange* change){
                                                  return change->time() <= currentOffset;
                                              });
    if (mostRecentLFOChangeIt == project.playlist().lfoChanges().rend()) {
        items.prepend(new StreamLFOItem(currentOffset, project.lfoMode()));
    } else {
        items.prepend(new StreamLFOItem(currentOffset, (*mostRecentLFOChangeIt)->mode()));
    }

    sortItems(items);

    if (loopStart >= 0 && loopEnd >= 0) {
        totalSamples = encode(project, items, data, loopStart, nullptr, currentOffset, &_currentOffsetData, true);
    } else {
        if (project.playlist().doesLoop()) {
            totalSamples = encode(project, items, data, project.playlist().loopOffset(), &_loopOffsetData, currentOffset, &_currentOffsetData, false);
        } else {
            totalSamples = encode(project, items, data, 0, nullptr, currentOffset, &_currentOffsetData);
        }
    }

    for (StreamItem* item : items) {
        delete item;
    }

    if (project.hasPCM()) {
        QByteArray enableDac;
        enableDac.append(0x52);
        enableDac.append(0x2B);
        enableDac.append(0x80);

        data.insert((qsizetype)_currentOffsetData, enableDac);

        if (_loopOffsetData >= _currentOffsetData) {
            _loopOffsetData += 3;
        }

        QByteArray dataBlock;
        quint32 dataBlockSize;
        QByteArray pcmBlock;
        if (_format == Format::FM_PSG) {
            dataBlock = project.pcm();
        } else {
            dataBlock = encodeStandardPCM(project, loopStart, loopEnd);
        }
        dataBlockSize = dataBlock.size();

        pcmBlock.append(0x67);
        pcmBlock.append(0x66);
        pcmBlock.append((quint8)0x00);
        pcmBlock.append((char*)&dataBlockSize, sizeof(dataBlockSize));
        pcmBlock.append(dataBlock);

        _loopOffsetData += 7 + dataBlockSize;
        _currentOffsetData += 7 + dataBlockSize;

        data.prepend(pcmBlock);
    }

    if (header) {
        _loopOffsetData += 64;
        _currentOffsetData += 64;
        QByteArray headerData;
        if (project.playlist().doesLoop()) {
            headerData = generateHeader(project, data, totalSamples, _loopOffsetData);
        } else {
            headerData = generateHeader(project, data, totalSamples, 0);
        }
        data.prepend(headerData);
    }

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    if (loopOffsetData) {
        *loopOffsetData = _loopOffsetData;
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

    for (int i = 0; i < PCM_CHANNELS; i++) {
        _pcmChannels[i].reset();
    }
}

QByteArray VGMStream::encodeStandardPCM(const Project& project, const Pattern& pattern, const float loopStart, const float loopEnd)
{
    QByteArray data;

    QList<StreamItem*> items;

    int _loopOffsetData = 0;
    int _currentOffsetData;

    processPattern(0, project, pattern, items, loopStart, loopEnd);
    assignChannelsAndExpand(items, project.tempo());

    QMap<QString, QByteArray> pcmData;
    QMap<int, int> pcmOffsetsByChannel;
    QMap<int, QString> pcmPathsByChannel;

    items.erase(std::remove_if(items.begin(), items.end(), [](StreamItem* si){
        StreamNoteItem* sni;
        if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
            if (sni->type() == Channel::Type::PCM) {
                return false;
            }
        }
        return true;
    }), items.end());

    std::for_each(items.begin(), items.end(), [&](StreamItem* si) {
        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);
        const PCMChannelSettings* pcmSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
        if (!pcmData.contains(pcmSettings->path())) {
            QFile file(pcmSettings->path());
            file.open(QIODevice::ReadOnly);
            pcmData[pcmSettings->path()] = file.readAll();
            file.close();
        }
    });

    sortItems(items);

    float lastTime = 0.0;
    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastPCM = nullptr;
    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
            if (lastPCM) {
                quint32 delayToEndOfPCM = pcmSize - pcmWritten;
                quint32 pcmLength = qMin(fullDelaySamples, delayToEndOfPCM);

                QByteArray dataToAppend((qsizetype)pcmLength, (char)0);
                for (quint32 j = 0; j < pcmLength; j++) {
                    int result = 0x00;

                    for (auto it = pcmOffsetsByChannel.begin(); it != pcmOffsetsByChannel.end();) {
                        int sample = pcmData[pcmPathsByChannel[it.key()]][pcmOffsetsByChannel[it.key()]++];
                        if (pcmData[pcmPathsByChannel[it.key()]].size() == pcmOffsetsByChannel[it.key()]) {
                            it = pcmOffsetsByChannel.erase(it);
                        } else {
                            ++it;
                        }

                        result += sample - 0x80;
                    }

                    if (result < -0x80) {
                        dataToAppend[j] = 0;
                    } else if (result >= 0x80) {
                        dataToAppend[j] = 0xFF;
                    } else {
                        dataToAppend[j] = result + 0x80;
                    }
                }
                data.append(dataToAppend);

                pcmWritten += pcmLength;

                if (pcmWritten == pcmSize) {
                    lastPCM = nullptr;
                }
            }
        }

        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(items[i]);
        if (sni->on()) {
            const PCMChannelSettings* channelSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
            quint32 newPcmSize = qMin(QFileInfo(QFile(channelSettings->path())).size(), (qint64)(sni->note().duration() / project.tempo() * 60 * 44100));

            pcmOffsetsByChannel[sni->channel()] = 0;
            pcmPathsByChannel[sni->channel()] = channelSettings->path();

            if (pcmWritten + newPcmSize > pcmSize) {
                lastPCM = sni;
                pcmSize = pcmWritten + newPcmSize;
            }
        } else {
            pcmOffsetsByChannel.remove(sni->channel());
            pcmPathsByChannel.remove(sni->channel());
        }
    }

    return data;
}

QByteArray VGMStream::encodeStandardPCM(const Project& project, const float loopStart, const float loopEnd)
{
    QByteArray data;

    QList<StreamItem*> items;

    int _loopOffsetData = 0;
    int _currentOffsetData;

    processProject(project, items, loopStart, loopEnd);
    assignChannelsAndExpand(items, project.tempo());

    QMap<QString, QByteArray> pcmData;
    QMap<int, int> pcmOffsetsByChannel;
    QMap<int, QString> pcmPathsByChannel;

    items.erase(std::remove_if(items.begin(), items.end(), [](StreamItem* si){
                    StreamNoteItem* sni;
                    if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
                        if (sni->type() == Channel::Type::PCM) {
                            return false;
                        }
                    }
                    return true;
                }), items.end());

    std::for_each(items.begin(), items.end(), [&](StreamItem* si) {
        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);
        const PCMChannelSettings* pcmSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
        if (!pcmData.contains(pcmSettings->path())) {
            QFile file(pcmSettings->path());
            file.open(QIODevice::ReadOnly);
            pcmData[pcmSettings->path()] = file.readAll();
            file.close();
        }
    });

    sortItems(items);

    float lastTime = 0.0;
    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastPCM = nullptr;
    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
            if (lastPCM) {
                quint32 delayToEndOfPCM = pcmSize - pcmWritten;
                quint32 pcmLength = qMin(fullDelaySamples, delayToEndOfPCM);

                QByteArray dataToAppend((qsizetype)pcmLength, (char)0);
                for (quint32 j = 0; j < pcmLength; j++) {
                    dataToAppend[j] = 0x80;

                    for (auto it = pcmOffsetsByChannel.begin(); it != pcmOffsetsByChannel.end();) {
                        quint8 sample = pcmData[pcmPathsByChannel[it.key()]][pcmOffsetsByChannel[it.key()]++];
                        if (pcmData[pcmPathsByChannel[it.key()]].size() == pcmOffsetsByChannel[it.key()]) {
                            it = pcmOffsetsByChannel.erase(it);
                        } else {
                            ++it;
                        }

                        dataToAppend[j] += ((int)sample) - 0x80;
                    }
                }
                data.append(dataToAppend);

                pcmWritten += pcmLength;

                if (pcmWritten == pcmSize) {
                    lastPCM = nullptr;
                }
            }
        }

        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(items[i]);
        if (sni->on()) {
            const PCMChannelSettings* channelSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
            quint32 newPcmSize = qMin(QFileInfo(QFile(channelSettings->path())).size(), (qint64)(sni->note().duration() / project.tempo() * 60 * 44100));

            pcmOffsetsByChannel[sni->channel()] = 0;
            pcmPathsByChannel[sni->channel()] = channelSettings->path();

            if (pcmWritten + newPcmSize > pcmSize) {
                lastPCM = sni;
                pcmSize = pcmWritten + newPcmSize;
            }
        } else {
            pcmOffsetsByChannel.remove(sni->channel());
        }
    }

    return data;
}

int VGMStream::acquireToneChannel(const float time, const float duration)
{
    for (int i = 0; i < TONE_CHANNELS; i++) {
        bool first;
        if (_toneChannels[i].acquire(time, duration, first)) {
            return i;
        }
    }

    return -1;
}

int VGMStream::acquireNoiseChannel(const float time, const float duration, const NoiseChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < NOISE_CHANNELS; i++) {
        if (_noiseChannels[i].settings() == *settings) {
            bool first;
            if (_noiseChannels[i].acquire(time, duration, first)) {
                if (first) {
                    items.append(new StreamSettingsItem(time, i, settings));
                }
                return i;
            }
        }
    }

    for (int i = 0; i < NOISE_CHANNELS; i++) {
        bool first;
        if (_noiseChannels[i].acquire(time, duration, first)) {
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
            bool first;
            if (_fmChannels[i].acquire(time, duration, first)) {
                if (first) {
                    items.append(new StreamSettingsItem(time, i, settings));
                }
                return i;
            }
        }
    }

    for (int i = 0; i < FM_CHANNELS; i++) {
        bool first;
        if (_fmChannels[i].acquire(time, duration, first)) {
            _fmChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

int VGMStream::acquirePCMChannel(const float time, const float duration)
{
    for (int i = 0; i < PCM_CHANNELS; i++) {
        bool first;
        if (_pcmChannels[i].acquire(time, duration, first)) {
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
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Track::Item* a, const Track::Item* b){ return a->time() < b->time(); });

    for (Track::Item* item : itemsCopy) {
        Note note = item->note();
        if (loopStart >= 0 && loopEnd >= 0) {
            if ((time + item->time()) < loopStart || (time + item->time()) >= loopEnd) {
                continue;
            }

            if ((time + item->time() + item->duration()) > loopEnd) {
                note.setDuration(loopEnd - item->time());
            }
        }
        items.append(new StreamNoteItem(time + item->time(), channel.type(), track, note, &channel.settings()));
    }
}

void VGMStream::processProject(const Project& project, QList<StreamItem*>& items, const float loopStart, const float loopEnd)
{
    QList<Playlist::Item*> itemsCopy(project.playlist().items());
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Playlist::Item* a, const Playlist::Item* b){ return a->time() < b->time(); });

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

    for (Playlist::LFOChange* change : project.playlist().lfoChanges()) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < change->time() || loopStart > change->time()) {
                continue;
            }
        }
        items.append(new StreamLFOItem(change->time(), change->mode()));
    }
}

void VGMStream::assignChannelsAndExpand(QList<StreamItem*>& items, const int tempo)
{
    QList<StreamItem*> itemsCopy = items;
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const StreamItem* a, const StreamItem* b){ return a->time() < b->time(); });

    for (StreamItem* item : itemsCopy) {
        StreamNoteItem* noteItem = dynamic_cast<StreamNoteItem*>(item);

        if (!noteItem) continue;

        assignChannel(noteItem, items);
        StreamNoteItem* noteOffItem = new StreamNoteItem(*noteItem);

        noteOffItem->setTime(noteItem->time() + noteItem->note().duration());
        noteOffItem->setOn(false);
        items.append(noteOffItem);
    }
}

void VGMStream::applySettingsChanges(Project& project, const float time, const Pattern& pattern, QList<StreamItem*>& items)
{
    QMap<int, Track*> tracks = pattern.tracks();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        QList<Track::SettingsChange*> settingChanges = it.value()->settingsChanges();

        std::sort(settingChanges.begin(), settingChanges.end(), [](Track::SettingsChange* a, Track::SettingsChange* b) {
            return a->time() < b->time();
        });

        QList<StreamItem*> trackNoteItems = items;
        trackNoteItems.erase(std::remove_if(trackNoteItems.begin(), trackNoteItems.end(), [&](StreamItem* si){
            StreamNoteItem* sni;
            if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
                if (sni->track() == it.value()) {
                    return false;
                }
            }
            return true;
        }), trackNoteItems.end());

        for (auto it2 = settingChanges.begin(); it2 != settingChanges.end(); ++it2) {
            for (StreamItem* si : trackNoteItems) {
                StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);
                if ((sni->time() - time) >= (*it2)->time()) {
                    sni->setChannelSettings(&(*it2)->settings());
                }
            }
        }
    }
}

void VGMStream::applySettingsChanges2(Project& project, const float time, const Pattern& pattern, QList<StreamItem*>& items)
{
    QMap<int, Track*> tracks = pattern.tracks();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        QList<Track::SettingsChange*> settingChanges = it.value()->settingsChanges();

        std::sort(settingChanges.begin(), settingChanges.end(), [](Track::SettingsChange* a, Track::SettingsChange* b) {
            return a->time() < b->time();
        });

        QList<StreamItem*> trackNoteItems = items;
        trackNoteItems.erase(std::remove_if(trackNoteItems.begin(), trackNoteItems.end(), [&](StreamItem* si){
            StreamNoteItem* sni;
            if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
                if (sni->track() == it.value()) {
                    return false;
                }
            }
            return true;
        }), trackNoteItems.end());

        for (auto it2 = settingChanges.begin(); it2 != settingChanges.end(); ++it2) {
            QList<StreamItem*> trackNoteItemsAtChange = trackNoteItems;
            trackNoteItemsAtChange.erase(std::remove_if(trackNoteItemsAtChange.begin(), trackNoteItemsAtChange.end(), [&](StreamItem* si){
                StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);

                return !((sni->time() - time) < (*it2)->time() && (*it2)->time() < (sni->time() - time + sni->note().duration()));
            }), trackNoteItemsAtChange.end());

            QList<int> channels;
            for (StreamItem* si : trackNoteItemsAtChange) {
                StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);
                if (!channels.contains(sni->channel())) {
                    channels.append(sni->channel());
                }
            }

            for (int c : channels) {
                StreamSettingsItem* ssi = new StreamSettingsItem(time + (*it2)->time(), c, &(*it2)->settings());
                items.append(ssi);
            }
        }
    }
}

void VGMStream::applySettingsChanges(Project& project, QList<StreamItem*>& items)
{
    for (Playlist::Item* item : project.playlist().items()) {
        applySettingsChanges(project, item->time(), project.getPattern(item->pattern()), items);
    }
}

void VGMStream::applySettingsChanges2(Project& project, QList<StreamItem*>& items)
{
    for (Playlist::Item* item : project.playlist().items()) {
        applySettingsChanges2(project, item->time(), project.getPattern(item->pattern()), items);
    }
}

void VGMStream::addSettingsAtCurrentOffset(QList<StreamItem*>& items, const float currentTime)
{
    if (currentTime == 0) {
        return;
    }

    sortItems(items);

    for (int i = 0; i < NOISE_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamSettingsItem* ssi;
            return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                   dynamic_cast<const NoiseChannelSettings*>(ssi->channelSettings()) &&
                   ssi->channel() == i && ssi->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it)->channelSettings());
            items.append(ssi);
        }
    }

    for (int i = 0; i < FM_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamSettingsItem* ssi;
            return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                   dynamic_cast<const FMChannelSettings*>(ssi->channelSettings()) &&
                   ssi->channel() == i && ssi->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it)->channelSettings());
            items.append(ssi);
        }
    }
}

void VGMStream::sortItems(QList<StreamItem*>& items)
{
    std::sort(items.begin(), items.end(), [](const StreamItem* a, const StreamItem* b){
        if (a->time() == b->time()) {
            const StreamNoteItem* an;
            const StreamNoteItem* bn;
            if((an = dynamic_cast<const StreamNoteItem*>(a)) != nullptr) {
                if((bn = dynamic_cast<const StreamNoteItem*>(b)) != nullptr) {
                    if (!an->on() && bn->on()) {
                        return true;
                    } if (an->on() && !bn->on()) {
                        return false;
                    } else {
                        return an->channel() < bn->channel();
                    }
                }
                return false;
            } else {
                if((bn = dynamic_cast<const StreamNoteItem*>(b)) != nullptr) {
                    return true;
                } else {
                    return a < b; // doesn't matter
                }
            }
        }

        return a->time() < b->time();
    });
}

void VGMStream::pad(QList<StreamItem*>& items, const float toDuration)
{
    StreamEndItem* sei = new StreamEndItem(toDuration);
    items.append(sei);
}

int VGMStream::encode(const Project& project, const QList<StreamItem*>& items,  QByteArray& data, const float loopTime, int* const loopOffsetData, const float currentTime, int* const currentOffsetData, const bool startAtLoop)
{
    float lastTime = startAtLoop ? loopTime : 0.0f;
    int totalSamples = 0;
    bool setLoopOffsetData = false;
    int _loopOffsetData;
    bool setCurrentOffsetData = false;
    int _currentOffsetData;

    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastPCM = nullptr;
    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
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
        }

        if (items[i]->time() >= loopTime && !setLoopOffsetData) {
            _loopOffsetData = data.size();
            if (_format == Format::STANDARD) {
                data.append(0xE0);
                data.append((char*)&pcmWritten, sizeof(pcmWritten));
            }
            setLoopOffsetData = true;
        }

        if (items[i]->time() >= currentTime && !setCurrentOffsetData) {
            _currentOffsetData = data.size();
            setCurrentOffsetData = true;
        }

        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        StreamLFOItem* sli;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);

            if (sni->on() && sni->type() == Channel::Type::PCM) {
                const PCMChannelSettings* channelSettings = dynamic_cast<const PCMChannelSettings*>(sni->channelSettings());
                quint32 newPcmSize = qMin(QFileInfo(QFile(channelSettings->path())).size(), (qint64)(sni->note().duration() / project.tempo() * 60 * 44100));
                if (pcmWritten + newPcmSize > pcmSize) {
                    lastPCM = sni;
                    pcmSize = pcmWritten + newPcmSize;
                }
            }
        } else if ((sli = dynamic_cast<StreamLFOItem*>(items[i])) != nullptr) {
            encodeLFOItem(sli, data);
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

    if (_format == Format::FM_PSG && pcm && samples > 8) {
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

        datum = 0xC0 | (fmcs->lfo().ams() << 4) | fmcs->lfo().fms();
        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB4 + channel);
        data.append(datum);
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
    } else if (item->type() == Channel::Type::PCM) {
        if (_format == Format::FM_PSG) {
            if (item->on()) {
                const PCMChannelSettings* pcmcs = dynamic_cast<const PCMChannelSettings*>(item->channelSettings());
                quint32 offset = project.pcmOffset(pcmcs->path());

                data.append(0xE0 | item->channel());
                data.append((char*)&offset, sizeof(offset));
            } else {
                quint32 offset = -1;

                data.append(0xE0 | item->channel());
                data.append((char*)&offset, sizeof(offset));
            }
        }
    }
}

void VGMStream::encodeLFOItem(const StreamLFOItem* item, QByteArray& data)
{
    data.append(0x52);
    data.append(0x22);
    if (item->mode() > 0) {
        data.append(0x8 | (item->mode() - 1));
    } else {
        data.append((quint8)0x00);
    }
}

QByteArray VGMStream::generateHeader(const Project& project, const QByteArray& data, const int totalSamples, const int loopOffsetData)
{
    QByteArray headerData(64, 0);

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
    *(uint32_t*)&headerData[0x14] = data.size() + 64 - 0x14;
    // Total samples
    *(uint32_t*)&headerData[0x18] = totalSamples;
    // Loop offset
    *(uint32_t*)&headerData[0x1C] = (loopOffsetData == 0) ? 0 : (loopOffsetData - 0x1C);
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

    return headerData;
}

VGMStream::StreamNoteItem::StreamNoteItem(const float time, const Channel::Type type, const Track* track, const Note& note, const ChannelSettings* channelSettings)
    : StreamItem(time)
    , _type(type)
    , _track(track)
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

void VGMStream::StreamNoteItem::setChannelSettings(const ChannelSettings* settings)
{
    _channelSettings = settings;
}

Channel::Type VGMStream::StreamNoteItem::type() const
{
    return _type;
}

const Track* VGMStream::StreamNoteItem::track() const
{
    return _track;
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

VGMStream::PhysicalChannel::PhysicalChannel()
    : _time(0)
    , _duration(0)
    , _acquiredIndefinitely(false)
    , _acquiredEver(false)
{

}

bool VGMStream::PhysicalChannel::acquire(float time, float duration, bool& first)
{
    first = !_acquiredEver;

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

    _acquiredEver = true;

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

void VGMStream::FMChannel::reset()
{
    PhysicalChannel::reset();
    _settings = FMChannelSettings();
}

NoiseChannelSettings& VGMStream::NoiseChannel::settings()
{
    return _settings;
}

void VGMStream::NoiseChannel::reset()
{
    PhysicalChannel::reset();
    _settings = NoiseChannelSettings();
}

VGMStream::StreamEndItem::StreamEndItem(const float time)
    : StreamItem(time)
{

}

VGMStream::StreamLFOItem::StreamLFOItem(const float time, const int mode)
    : StreamItem(time)
    , _mode(mode)
{

}

int VGMStream::StreamLFOItem::mode() const
{
    return _mode;
}
