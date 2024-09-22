#include "vgmstream.h"

QList<float> VGMStream::frequencies = {
    16.35f,
    17.32f,
    18.35f,
    19.45f,
    20.60f,
    21.83f,
    23.12f,
    24.50f,
    25.96f,
    27.50f,
    29.14f,
    30.87f
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
    653, 692, 733, 777, 823, 872, 924, 979, 1037, 1099, 1164, 1235
};

QList<int> VGMStream::ym2413_frequencies = {
    172, 181, 192, 204, 216, 229, 242, 257, 272, 288, 305, 323
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

void VGMStream::assignChannel(const Project& project, StreamNoteItem* noteItem, QList<StreamItem*>& items)
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
    } else if (noteItem->type() == Channel::Type::SSG) {
        const SSGChannelSettings* scs = dynamic_cast<const SSGChannelSettings*>(noteItem->channelSettings());
        int channel = acquireSSGChannel(noteItem->time(), noteItem->note().duration(), scs, items);
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::MELODY) {
        const MelodyChannelSettings* mcs = dynamic_cast<const MelodyChannelSettings*>(noteItem->channelSettings());
        int channel = acquireMelodyChannel(noteItem->time(), noteItem->note().duration(), mcs, items);
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::RHYTHM) {
        const RhythmChannelSettings* rcs = dynamic_cast<const RhythmChannelSettings*>(noteItem->channelSettings());
        int channel = acquireRhythmChannel(noteItem->time(), noteItem->note().duration(), rcs, items);
        noteItem->setChannel(channel);
    } else if (noteItem->type() == Channel::Type::PCM) {
        const ROMChannelSettings* rcs = dynamic_cast<const ROMChannelSettings*>(noteItem->channelSettings());
        int channel = acquirePCMChannel(project, noteItem->time(), noteItem->note().duration(), rcs);
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
    } else if (type == Channel::Type::SSG) {
        _ssgChannels[channel].release();
    } else if (type == Channel::Type::MELODY) {
        _melodyChannels[channel].release();
    } else if (type == Channel::Type::RHYTHM) {
        _rhythmChannels[channel].release();
    } else if (type == Channel::Type::PCM) {
        _pcmChannels[channel].release();
    }
}

void VGMStream::encode(const Project& project, QList<StreamItem*>& items, QByteArray& data)
{
    for (int i = 0; i < items.size(); i++) {
        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        StreamLFOItem* sli;
        StreamNoiseFrequencyItem* snfi;
        StreamEnvelopeFrequencyItem* sefi;
        StreamEnvelopeShapeItem* sesi;
        StreamUserToneItem* suti;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(project, ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);
        } else if ((sli = dynamic_cast<StreamLFOItem*>(items[i])) != nullptr) {
            encodeLFOItem(sli, data);
        } else if ((snfi = dynamic_cast<StreamNoiseFrequencyItem*>(items[i])) != nullptr) {
            encodeNoiseFrequencyItem(snfi, data);
        } else if ((sefi = dynamic_cast<StreamEnvelopeFrequencyItem*>(items[i])) != nullptr) {
            encodeEnvelopeFrequencyItem(sefi, data);
        } else if ((sesi = dynamic_cast<StreamEnvelopeShapeItem*>(items[i])) != nullptr) {
            encodeEnvelopeShapeItem(sesi, data);
        } else if ((suti = dynamic_cast<StreamUserToneItem*>(items[i])) != nullptr) {
            encodeUserToneItem(suti, data);
        }
    }
}

QByteArray VGMStream::compile(Project& project, const Pattern& pattern, bool gd3, int* loopOffsetData, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _loopOffsetData = 0;
    int _currentOffsetData = 0;

    processPattern(0, project, pattern, items, loopStart, loopEnd);
    applySettingsChanges(0, pattern, items);
    assignChannelsAndExpand(project, items, project.tempo());
    applySettingsChanges2(0, pattern, items);
    addSettingsAtCurrentOffset(items, currentOffset);

    if (loopStart >= 0 && loopEnd >= 0) {
        pad(items, loopEnd);

        if (project.usesOPN()) {
            items.prepend(new StreamLFOItem(loopStart, project.lfoMode()));
        }
        if (project.usesSSG()) {
            items.prepend(new StreamNoiseFrequencyItem(loopStart, project.ssgNoiseFrequency()));
            items.prepend(new StreamEnvelopeFrequencyItem(loopStart, project.ssgEnvelopeFrequency()));
            items.prepend(new StreamEnvelopeShapeItem(loopStart, project.ssgEnvelopeShape()));
        }
        if (project.usesOPL()) {
            items.prepend(new StreamUserToneItem(loopStart, project.userTone()));
        }

        sortItems(items);
        totalSamples = encode(project, items, data, loopStart, nullptr, currentOffset, &_currentOffsetData, true);
    } else {
        pad(items, project.getPatternBarLength(pattern));

        if (project.usesOPN()) {
            items.prepend(new StreamLFOItem(0, project.lfoMode()));
        }
        if (project.usesSSG()) {
            items.prepend(new StreamNoiseFrequencyItem(0, project.ssgNoiseFrequency()));
            items.prepend(new StreamEnvelopeFrequencyItem(0, project.ssgEnvelopeFrequency()));
            items.prepend(new StreamEnvelopeShapeItem(0, project.ssgEnvelopeShape()));
        }
        if (project.usesOPL()) {
            items.prepend(new StreamUserToneItem(0, project.userTone()));
        }

        sortItems(items);
        totalSamples = encode(project, items, data, 0, nullptr, currentOffset, &_currentOffsetData, true);
    }

    for (StreamItem* item : items) {
        delete item;
    }

    if (project.usesRhythm()) {
        QByteArray enableRhythm;

        enableRhythm.append(0x51);
        enableRhythm.append(0x16);
        enableRhythm.append(0x20);

        enableRhythm.append(0x51);
        enableRhythm.append(0x17);
        enableRhythm.append(0x50);

        enableRhythm.append(0x51);
        enableRhythm.append(0x18);
        enableRhythm.append(0xC0);

        enableRhythm.append(0x51);
        enableRhythm.append(0x26);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x27);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x28);
        enableRhythm.append(0x01);

        _loopOffsetData += enableRhythm.size();

        data.prepend(enableRhythm);
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
        if (_format == Format::CHROMASOUND) {
            QFile romFile(project.resolve(project.pcmFile()));
            romFile.open(QIODevice::ReadOnly);
            dataBlock = romFile.readAll();
            romFile.close();
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

        if (_format == Format::STANDARD) {
            pcmBlock.append(0xE0);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);

            _loopOffsetData += 5;
            _currentOffsetData += 5;
        }

        data.prepend(pcmBlock);
    }

    QByteArray gd3Data;
    if (gd3) {
        gd3Data = GD3::generateGd3(project);
    }

    _loopOffsetData += 128;
    _currentOffsetData += 128;
    QByteArray headerData = generateHeader(project, data, totalSamples, _loopOffsetData, gd3Data.size(), !(loopStart < 0 || loopEnd < 0));
    data.prepend(headerData);

    data.append(gd3Data);

    if (currentOffsetData) {
        *currentOffsetData = _currentOffsetData;
    }

    if (loopOffsetData) {
        *loopOffsetData = _loopOffsetData;
    }

    return data;
}

QByteArray VGMStream::compile(Project& project, bool gd3, int* loopOffsetData, const float loopStart, const float loopEnd, const float currentOffset, int* const currentOffsetData)
{
    QList<StreamItem*> items;
    QByteArray data;
    int totalSamples;

    int _loopOffsetData = 0;
    int _currentOffsetData = 0;

    processProject(project, items, loopStart, loopEnd);
    applySettingsChanges(project, items);
    assignChannelsAndExpand(project, items, project.tempo());
    applySettingsChanges2(project, items);
    addSettingsAtCurrentOffset(items, currentOffset);

    if (loopStart >= 0 && loopEnd >= 0) {
        pad(items, loopEnd);
    } else {
        pad(items, project.getLength());
    }

    if (project.usesOPN()) {
        // LFO changes
        QList<Playlist::LFOChange*> lfoChanges = project.playlist().lfoChanges();

        std::sort(lfoChanges.begin(), lfoChanges.end(), [](Playlist::LFOChange* a, Playlist::LFOChange* b) {
            return a->time() < b->time();
        });

        auto mostRecentLFOChangeIt = std::find_if(lfoChanges.rbegin(), lfoChanges.rend(),
        [&](Playlist::LFOChange* change) {
            return change->time() <= currentOffset;
        });
        if (mostRecentLFOChangeIt == lfoChanges.rend()) {
            items.prepend(new StreamLFOItem(currentOffset, project.lfoMode()));
        } else {
            items.prepend(new StreamLFOItem(currentOffset, (*mostRecentLFOChangeIt)->mode()));
        }
    }

    if (project.usesSSG()) {
        // SSG noise frequency
        QList<Playlist::NoiseFrequencyChange*> nfChanges = project.playlist().noiseFrequencyChanges();

        std::sort(nfChanges.begin(), nfChanges.end(), [](Playlist::NoiseFrequencyChange* a, Playlist::NoiseFrequencyChange* b) {
            return a->time() < b->time();
        });

        auto mostRecentNFChangeIt = std::find_if(nfChanges.rbegin(), nfChanges.rend(), [&](Playlist::NoiseFrequencyChange* change) {
            return change->time() <= currentOffset;
        });
        if (mostRecentNFChangeIt == nfChanges.rend()) {
            items.prepend(new StreamNoiseFrequencyItem(currentOffset, project.ssgNoiseFrequency()));
        } else {
            items.prepend(new StreamNoiseFrequencyItem(currentOffset, (*mostRecentNFChangeIt)->frequency()));
        }

        // SSG envelope frequency
        QList<Playlist::EnvelopeFrequencyChange*> efChanges = project.playlist().envelopeFrequencyChanges();

        std::sort(efChanges.begin(), efChanges.end(), [](Playlist::EnvelopeFrequencyChange* a, Playlist::EnvelopeFrequencyChange* b) {
            return a->time() < b->time();
        });

        auto mostRecentEFChangeIt = std::find_if(efChanges.rbegin(), efChanges.rend(), [&](Playlist::EnvelopeFrequencyChange* change) {
            return change->time() <= currentOffset;
        });
        if (mostRecentEFChangeIt == efChanges.rend()) {
            items.prepend(new StreamEnvelopeFrequencyItem(currentOffset, project.ssgEnvelopeFrequency()));
        } else {
            items.prepend(new StreamEnvelopeFrequencyItem(currentOffset, (*mostRecentEFChangeIt)->frequency()));
        }

        // SSG envelope shape
        QList<Playlist::EnvelopeShapeChange*> esChanges = project.playlist().envelopeShapeChanges();

        std::sort(esChanges.begin(), esChanges.end(), [](Playlist::EnvelopeShapeChange* a, Playlist::EnvelopeShapeChange* b) {
            return a->time() < b->time();
        });

        auto mostRecentESChangeIt = std::find_if(esChanges.rbegin(), esChanges.rend(), [&](Playlist::EnvelopeShapeChange* change) {
            return change->time() <= currentOffset;
        });
        if (mostRecentESChangeIt == esChanges.rend()) {
            items.prepend(new StreamEnvelopeShapeItem(currentOffset, project.ssgEnvelopeShape()));
        } else {
            items.prepend(new StreamEnvelopeShapeItem(currentOffset, (*mostRecentESChangeIt)->shape()));
        }
    }

    if (project.usesOPL()) {
        // OPL User tone
        QList<Playlist::UserToneChange*> utChanges = project.playlist().userToneChanges();

        std::sort(utChanges.begin(), utChanges.end(), [](Playlist::UserToneChange* a, Playlist::UserToneChange* b) {
            return a->time() < b->time();
        });

        auto mostRecentUTChangeIt = std::find_if(utChanges.rbegin(), utChanges.rend(), [&](Playlist::UserToneChange* change) {
            return change->time() <= currentOffset;
        });
        if (mostRecentUTChangeIt == utChanges.rend()) {
            items.prepend(new StreamUserToneItem(currentOffset, project.userTone()));
        } else {
            items.prepend(new StreamUserToneItem(currentOffset, (*mostRecentUTChangeIt)->userTone()));
        }
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

    if (project.usesRhythm()) {
        QByteArray enableRhythm;

        enableRhythm.append(0x51);
        enableRhythm.append(0x16);
        enableRhythm.append(0x20);

        enableRhythm.append(0x51);
        enableRhythm.append(0x17);
        enableRhythm.append(0x50);

        enableRhythm.append(0x51);
        enableRhythm.append(0x18);
        enableRhythm.append(0xC0);

        enableRhythm.append(0x51);
        enableRhythm.append(0x26);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x27);
        enableRhythm.append(0x05);

        enableRhythm.append(0x51);
        enableRhythm.append(0x28);
        enableRhythm.append(0x01);

        _loopOffsetData += enableRhythm.size();

        data.prepend(enableRhythm);
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
        if (_format == Format::CHROMASOUND) {
            QFile romFile(project.resolve(project.pcmFile()));
            romFile.open(QIODevice::ReadOnly);
            dataBlock = romFile.readAll();
            romFile.close();
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

        if (_format == Format::STANDARD) {
            pcmBlock.append(0xE0);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);
            pcmBlock.append((quint8)0x00);

            _loopOffsetData += 5;
            _currentOffsetData += 5;
        }

        data.prepend(pcmBlock);
    }

    QByteArray gd3Data;
    if (gd3) {
        gd3Data = GD3::generateGd3(project);
    }

    _loopOffsetData += 128;
    _currentOffsetData += 128;
    QByteArray headerData;
    if (project.playlist().doesLoop() || !(loopStart < 0 || loopEnd < 0)) {
        headerData = generateHeader(project, data, totalSamples, _loopOffsetData, gd3Data.size(), !(loopStart < 0 || loopEnd < 0));
    } else {
        headerData = generateHeader(project, data, totalSamples, 0, gd3Data.size(), false);
    }
    data.prepend(headerData);

    data.append(gd3Data);

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

    _lastSSGMixer = 0;

    for (int i = 0; i < SSG_CHANNELS; i++) {
        _lastSSGLevel[i] = 0;
        _ssgChannels[i].reset();
    }

    for (int i = 0; i < MELODY_CHANNELS; i++) {
        _lastChanVal[i] = 0;
        _melodyChannels[i].reset();
    }

    for (int i = 0; i < RHYTHM_CHANNELS; i++) {
        _rhythmChannels[i].reset();
    }

    for (int i = 0; i < PCM_CHANNELS; i++) {
        _pcmChannels[i].reset();
    }

    _lastRhythm = 0;
}

QByteArray VGMStream::encodeStandardPCM(const Project& project, const Pattern& pattern, const float loopStart, const float loopEnd)
{
    QByteArray data;

    QList<StreamItem*> items;

    int _loopOffsetData = 0;
    int _currentOffsetData;

    processPattern(0, project, pattern, items, loopStart, loopEnd);
    applySettingsChanges(0, pattern, items);
    assignChannelsAndExpand(project, items, project.tempo());
    applySettingsChanges2(0, pattern, items);
    addSettingsAtCurrentOffset(items, qMax(0.0f, loopStart));

    QByteArray pcmData;
    QMap<int, quint32> pcmOffsetsByChannel;
    QMap<int, int> pcmSampleByChannel;
    QMap<int, int> pcmVolumeByChannel;

    items.erase(std::remove_if(items.begin(), items.end(), [](StreamItem* si) {
        StreamNoteItem* sni;
        if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
            if (sni->type() == Channel::Type::PCM) {
                return false;
            }
        }
        return true;
    }), items.end());

    if (!project.pcmFile().isEmpty()) {
        QFile romFile(project.resolve(project.pcmFile()));
        romFile.open(QIODevice::ReadOnly);
        pcmData = romFile.readAll();
        romFile.close();
    }

    sortItems(items);

    ROM pcmROM(project.resolve(project.pcmFile()));

    float lastTime = 0.0;
    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastNoteItem = nullptr;
    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
            if (lastNoteItem) {
                quint32 delayToEndOfPCM = pcmSize - pcmWritten;
                quint32 pcmLength = qMin(fullDelaySamples, delayToEndOfPCM);

                QByteArray dataToAppend((qsizetype)pcmLength, (char)0);
                for (quint32 j = 0; j < pcmLength; j++) {
                    int result = 0x00;

                    for (auto it = pcmOffsetsByChannel.begin(); it != pcmOffsetsByChannel.end();) {
                        int sample = pcmData[pcmOffsetsByChannel[it.key()]++];
                        int volume = pcmVolumeByChannel[it.key()];
                        int att = MAX_PCM_ATTENUATION * (100 - volume) / 100.0f;

                        if (pcmROM.size(pcmSampleByChannel[it.key()]) == pcmOffsetsByChannel[it.key()] - pcmROM.offsets()[pcmSampleByChannel[it.key()]]) {
                            it = pcmOffsetsByChannel.erase(it);
                        } else {
                            ++it;
                        }

                        result += (sample - 0x80) >> att;
                    }

                    result += 0x80;

                    if (result < 0) {
                        dataToAppend[j] = 0;
                    } else if (result >= 0xFF) {
                        dataToAppend[j] = 0xFF;
                    } else {
                        dataToAppend[j] = result;
                    }
                }
                data.append(dataToAppend);

                pcmWritten += pcmLength;

                if (pcmWritten == pcmSize) {
                    lastNoteItem = nullptr;
                }
            }
        }

        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(items[i]);
        if (sni->on()) {
            quint32 newPcmSize;
            if (sni->type() == Channel::Type::PCM) {

                const ROMChannelSettings* channelSettings = dynamic_cast<const ROMChannelSettings*>(sni->channelSettings());
                newPcmSize = 0;

                if (channelSettings->keySampleMappings().contains(sni->note().key())) {
                    newPcmSize = qMin(pcmROM.size(channelSettings->keySampleMappings()[sni->note().key()]), (quint32)(sni->note().duration() / project.tempo() * 60 * 44100));
                    pcmOffsetsByChannel[sni->channel()] = pcmROM.offsets()[channelSettings->keySampleMappings()[sni->note().key()]];
                    pcmSampleByChannel[sni->channel()] = channelSettings->keySampleMappings()[sni->note().key()];
                    pcmVolumeByChannel[sni->channel()] = channelSettings->volume() * sni->note().velocity() / 100.0f;
                }
            }
            if (pcmWritten + newPcmSize > pcmSize) {
                lastNoteItem = sni;
                pcmSize = pcmWritten + newPcmSize;
            }
        } else {
            if (sni->type() == Channel::Type::PCM) {
                pcmOffsetsByChannel.remove(sni->channel());
                pcmSampleByChannel.remove(sni->channel());
                pcmVolumeByChannel.remove(sni->channel());
            }
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
    applySettingsChanges(project, items);
    assignChannelsAndExpand(project, items, project.tempo());
    applySettingsChanges2(project, items);
    addSettingsAtCurrentOffset(items, qMax(0.0f, loopStart));

    QByteArray pcmData;
    QMap<int, int> pcmOffsetsByChannel;
    QMap<int, int> pcmSampleByChannel;
    QMap<int, int> pcmVolumeByChannel;

    items.erase(std::remove_if(items.begin(), items.end(), [](StreamItem* si) {
        StreamNoteItem* sni;
        if ((sni = dynamic_cast<StreamNoteItem*>(si))) {
            if (sni->type() == Channel::Type::PCM) {
                return false;
            }
        }
        return true;
    }), items.end());

    if (!project.pcmFile().isEmpty()) {
        QFile romFile(project.resolve(project.pcmFile()));
        romFile.open(QIODevice::ReadOnly);
        pcmData = romFile.readAll();
        romFile.close();
    }

    sortItems(items);

    ROM pcmROM(project.resolve(project.pcmFile()));

    float lastTime = 0.0;
    quint32 pcmSize = 0;
    quint32 pcmWritten = 0;
    StreamNoteItem* lastNoteItem = nullptr;
    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
            if (lastNoteItem) {
                quint32 delayToEndOfPCM = pcmSize - pcmWritten;
                quint32 pcmLength = qMin(fullDelaySamples, delayToEndOfPCM);

                QByteArray dataToAppend((qsizetype)pcmLength, (char)0);
                for (quint32 j = 0; j < pcmLength; j++) {
                    int result = 0x00;

                    for (auto it = pcmOffsetsByChannel.begin(); it != pcmOffsetsByChannel.end();) {
                        int sample = pcmData[pcmOffsetsByChannel[it.key()]++];
                        int volume = pcmVolumeByChannel[it.key()];
                        int att = MAX_PCM_ATTENUATION * (100 - volume) / 100.0f;

                        if (pcmROM.size(pcmSampleByChannel[it.key()]) == pcmOffsetsByChannel[it.key()] - pcmROM.offsets()[pcmSampleByChannel[it.key()]]) {
                            it = pcmOffsetsByChannel.erase(it);
                        } else {
                            ++it;
                        }

                        result += (sample - 0x80) >> att;
                    }

                    result += 0x80;

                    if (result < 0) {
                        dataToAppend[j] = 0;
                    } else if (result >= 0xFF) {
                        dataToAppend[j] = 0xFF;
                    } else {
                        dataToAppend[j] = result;
                    }
                }
                data.append(dataToAppend);

                pcmWritten += pcmLength;

                if (pcmWritten == pcmSize) {
                    lastNoteItem = nullptr;
                }
            }
        }

        StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(items[i]);
        if (sni->on()) {
            quint32 newPcmSize;
            if (sni->type() == Channel::Type::PCM) {
                const ROMChannelSettings* channelSettings = dynamic_cast<const ROMChannelSettings*>(sni->channelSettings());
                newPcmSize = 0;

                if (channelSettings->keySampleMappings().contains(sni->note().key())) {
                    newPcmSize = qMin(pcmROM.size(channelSettings->keySampleMappings()[sni->note().key()]), (quint32)(sni->note().duration() / project.tempo() * 60 * 44100));
                    pcmOffsetsByChannel[sni->channel()] = pcmROM.offsets()[channelSettings->keySampleMappings()[sni->note().key()]];
                    pcmSampleByChannel[sni->channel()] = channelSettings->keySampleMappings()[sni->note().key()];
                    pcmVolumeByChannel[sni->channel()] = channelSettings->volume() * sni->note().velocity() / 100.0f;
                }
            }
            if (pcmWritten + newPcmSize > pcmSize) {
                lastNoteItem = sni;
                pcmSize = pcmWritten + newPcmSize;
            }
        } else {
            if (sni->type() == Channel::Type::PCM) {
                pcmOffsetsByChannel.remove(sni->channel());
                pcmSampleByChannel.remove(sni->channel());
                pcmVolumeByChannel.remove(sni->channel());
            }
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

int VGMStream::acquireSSGChannel(const float time, const float duration, const SSGChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < SSG_CHANNELS; i++) {
        if (_ssgChannels[i].settings() == *settings) {
            bool first;
            if (_ssgChannels[i].acquire(time, duration, first)) {
                if (first) {
                    items.append(new StreamSettingsItem(time, i, settings));
                }
                return i;
            }
        }
    }

    for (int i = 0; i < SSG_CHANNELS; i++) {
        bool first;
        if (_ssgChannels[i].acquire(time, duration, first)) {
            _ssgChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

int VGMStream::acquireMelodyChannel(const float time, const float duration, const MelodyChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < MELODY_CHANNELS; i++) {
        if (_melodyChannels[i].settings() == *settings) {
            bool first;
            if (_melodyChannels[i].acquire(time, duration, first)) {
                if (first) {
                    items.append(new StreamSettingsItem(time, i, settings));
                }
                return i;
            }
        }
    }

    for (int i = 0; i < MELODY_CHANNELS; i++) {
        bool first;
        if (_melodyChannels[i].acquire(time, duration, first)) {
            _melodyChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

int VGMStream::acquireRhythmChannel(const float time, const float duration, const RhythmChannelSettings* settings, QList<StreamItem*>& items)
{
    for (int i = 0; i < RHYTHM_CHANNELS; i++) {
        if (_rhythmChannels[i].settings() == *settings) {
            bool first;
            if (_rhythmChannels[i].acquire(time, duration, first)) {
                if (first) {
                    items.append(new StreamSettingsItem(time, i, settings));
                }
                return i;
            }
        }
    }

    for (int i = 0; i < RHYTHM_CHANNELS; i++) {
        bool first;
        if (_rhythmChannels[i].acquire(time, duration, first)) {
            _rhythmChannels[i].settings() = *settings;
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

int VGMStream::acquirePCMChannel(const Project& project, const float time, const float duration, const ROMChannelSettings* settings)
{
    ROM pcmROM(project.resolve(project.pcmFile()));

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
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Track::Item* a, const Track::Item* b) {
        return a->time() < b->time();
    });

    for (Track::Item* item : itemsCopy) {
        Note note = item->note();
        if (loopStart >= 0 && loopEnd >= 0) {
            if ((time + item->time()) < loopStart || (time + item->time()) >= loopEnd) {
                continue;
            }

            if ((time + item->time() + item->duration()) > loopEnd) {
                note.setDuration(loopEnd - (time + item->time()));
            }
        }
        items.append(new StreamNoteItem(time + item->time(), channel.type(), track, note, &channel.settings()));
    }
}

void VGMStream::processProject(const Project& project, QList<StreamItem*>& items, const float loopStart, const float loopEnd)
{
    QList<Playlist::Item*> itemsCopy(project.playlist().items());
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Playlist::Item* a, const Playlist::Item* b) {
        return a->time() < b->time();
    });

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

    for (Playlist::NoiseFrequencyChange* change : project.playlist().noiseFrequencyChanges()) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < change->time() || loopStart > change->time()) {
                continue;
            }
        }
        items.append(new StreamNoiseFrequencyItem(change->time(), change->frequency()));
    }

    for (Playlist::EnvelopeFrequencyChange* change : project.playlist().envelopeFrequencyChanges()) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < change->time() || loopStart > change->time()) {
                continue;
            }
        }
        items.append(new StreamEnvelopeFrequencyItem(change->time(), change->frequency()));
    }

    for (Playlist::EnvelopeShapeChange* change : project.playlist().envelopeShapeChanges()) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < change->time() || loopStart > change->time()) {
                continue;
            }
        }
        items.append(new StreamEnvelopeShapeItem(change->time(), change->shape()));
    }

    for (Playlist::UserToneChange* change : project.playlist().userToneChanges()) {
        if (loopStart >= 0 && loopEnd >= 0) {
            if (loopEnd < change->time() || loopStart > change->time()) {
                continue;
            }
        }
        items.append(new StreamUserToneItem(change->time(), change->userTone()));
    }
}

void VGMStream::assignChannelsAndExpand(const Project& project, QList<StreamItem*>& items, const int tempo)
{
    QList<StreamItem*> itemsCopy = items;
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const StreamItem* a, const StreamItem* b) {
        return a->time() < b->time();
    });

    ROM pcmROM(project.resolve(project.pcmFile()));

    for (StreamItem* item : itemsCopy) {
        StreamNoteItem* noteItem = dynamic_cast<StreamNoteItem*>(item);

        if (!noteItem) continue;

        assignChannel(project, noteItem, items);
        StreamNoteItem* noteOffItem = new StreamNoteItem(*noteItem);

        float duration = noteItem->note().duration();
        if (noteItem->type() == Channel::Type::PCM) {
            const ROMChannelSettings* channelSettings = dynamic_cast<const ROMChannelSettings*>(noteItem->channelSettings());
            quint32 size = 0;
            quint32 durationSamples = duration / tempo * 60 * 44100;

            if (channelSettings->keySampleMappings().contains(noteItem->note().key())) {
                size = pcmROM.size(channelSettings->keySampleMappings()[noteItem->note().key()]);
            }

            if (size < durationSamples) {
                duration = size / 44100.0f / 60 * tempo;
            }
        }
        noteOffItem->setTime(noteItem->time() + duration);
        noteOffItem->setOn(false);
        items.append(noteOffItem);
    }
}

void VGMStream::applySettingsChanges(const float time, const Pattern& pattern, QList<StreamItem*>& items)
{
    QMap<int, Track*> tracks = pattern.tracks();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        QList<Track::SettingsChange*> settingChanges = it.value()->settingsChanges();

        std::sort(settingChanges.begin(), settingChanges.end(), [](Track::SettingsChange* a, Track::SettingsChange* b) {
            return a->time() < b->time();
        });

        QList<StreamItem*> trackNoteItems = items;
        trackNoteItems.erase(std::remove_if(trackNoteItems.begin(), trackNoteItems.end(), [&](StreamItem* si) {
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

void VGMStream::applySettingsChanges2(const float time, const Pattern& pattern, QList<StreamItem*>& items)
{
    QMap<int, Track*> tracks = pattern.tracks();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        QList<Track::SettingsChange*> settingChanges = it.value()->settingsChanges();

        std::sort(settingChanges.begin(), settingChanges.end(), [](Track::SettingsChange* a, Track::SettingsChange* b) {
            return a->time() < b->time();
        });

        QList<StreamItem*> trackNoteItems = items;
        trackNoteItems.erase(std::remove_if(trackNoteItems.begin(), trackNoteItems.end(), [&](StreamItem* si) {
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
            trackNoteItemsAtChange.erase(std::remove_if(trackNoteItemsAtChange.begin(), trackNoteItemsAtChange.end(), [&](StreamItem* si) {
                StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);

                return !((sni->time() - time) < (*it2)->time() && (*it2)->time() < (sni->time() - time + sni->note().duration()));
            }), trackNoteItemsAtChange.end());

            QMap<int, int> channelsAndVelocities;
            for (StreamItem* si : trackNoteItemsAtChange) {
                StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(si);
                if (!channelsAndVelocities.contains(sni->channel())) {
                    channelsAndVelocities.insert(sni->channel(), sni->note().velocity());
                }
            }

            for (auto it3 = channelsAndVelocities.begin(); it3 != channelsAndVelocities.end(); ++it3) {
                StreamSettingsItem* ssi = new StreamSettingsItem(time + (*it2)->time(), it3.key(), &(*it2)->settings(), it3.value());
                items.append(ssi);
            }
        }
    }
}

void VGMStream::applySettingsChanges(const Project& project, QList<StreamItem*>& items)
{
    for (Playlist::Item* item : project.playlist().items()) {
        applySettingsChanges(item->time(), project.getPattern(item->pattern()), items);
    }
}

void VGMStream::applySettingsChanges2(const Project& project, QList<StreamItem*>& items)
{
    for (Playlist::Item* item : project.playlist().items()) {
        applySettingsChanges2(item->time(), project.getPattern(item->pattern()), items);
    }
}

void VGMStream::addSettingsAtCurrentOffset(QList<StreamItem*>& items, const float currentTime)
{
    if (currentTime == 0) {
        return;
    }

    sortItems(items);

    for (int i = 0; i < TONE_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::TONE &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamNoteItem*>(*it)->channelSettings());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const ToneChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < NOISE_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::NOISE &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const NoiseChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < FM_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::FM &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const FMChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < SSG_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::SSG &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const SSGChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < MELODY_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::MELODY &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const MelodyChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < RHYTHM_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::RHYTHM &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const RhythmChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }

    for (int i = 0; i < PCM_CHANNELS; i++) {
        auto it = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
            StreamNoteItem* sni;
            return (sni = dynamic_cast<StreamNoteItem*>(si)) &&
                   sni->type() == Channel::Type::PCM &&
                   sni->channel() == i && sni->time() <= currentTime;
        });

        if (it != items.rend()) {
            StreamNoteItem* sni = dynamic_cast<StreamNoteItem*>(*it);
            StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, sni->channelSettings(), sni->note().velocity());
            items.append(ssi);
        } else {
            auto it2 = std::find_if(items.rbegin(), items.rend(), [&](StreamItem* si) {
                StreamSettingsItem* ssi;
                return (ssi = dynamic_cast<StreamSettingsItem*>(si)) &&
                       dynamic_cast<const ROMChannelSettings*>(ssi->channelSettings()) &&
                       ssi->channel() == i && ssi->time() <= currentTime;
            });

            if (it2 != items.rend()) {
                StreamSettingsItem* ssi = new StreamSettingsItem(currentTime, i, dynamic_cast<StreamSettingsItem*>(*it2)->channelSettings());
                items.append(ssi);
            }
        }
    }
}

void VGMStream::sortItems(QList<StreamItem*>& items)
{
    std::sort(items.begin(), items.end(), [](const StreamItem* a, const StreamItem* b) {
        if (a->time() == b->time()) {
            const StreamNoteItem* an;
            const StreamNoteItem* bn;
            if((an = dynamic_cast<const StreamNoteItem*>(a)) != nullptr) {
                if((bn = dynamic_cast<const StreamNoteItem*>(b)) != nullptr) {
                    if (!an->on() && bn->on()) {
                        return true;
                    }
                    if (an->on() && !bn->on()) {
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
    StreamNoteItem* lastNoteItem = nullptr;

    ROM pcmROM(project.resolve(project.pcmFile()));

    for (int i = 0; i < items.size(); i++) {
        quint32 fullDelaySamples = (quint32)((items[i]->time() - lastTime) / project.tempo() * 60 * 44100);
        lastTime = items[i]->time();
        if (fullDelaySamples > 0) {
            if (lastNoteItem) {
                quint32 delayToEndOfPCM = pcmSize - pcmWritten;
                totalSamples += encodeDelay(qMin(fullDelaySamples, delayToEndOfPCM), data, true);
                pcmWritten += qMin(fullDelaySamples, delayToEndOfPCM);

                if (delayToEndOfPCM < fullDelaySamples) {
                    totalSamples += encodeDelay(fullDelaySamples - delayToEndOfPCM, data, false);
                }

                if (pcmWritten == pcmSize) {
                    lastNoteItem = nullptr;
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
        StreamNoiseFrequencyItem* snfi;
        StreamEnvelopeFrequencyItem* sefi;
        StreamEnvelopeShapeItem* sesi;
        StreamUserToneItem* suti;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(project, ssi, data);
            continue;
        }

        if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(project, sni, data);

            if (sni->on() && sni->type() == Channel::Type::PCM) {
                const ROMChannelSettings* channelSettings = dynamic_cast<const ROMChannelSettings*>(sni->channelSettings());
                quint32 newPcmSize = 0;

                if (channelSettings->keySampleMappings().contains(sni->note().key())) {
                    newPcmSize = qMin(pcmROM.size(channelSettings->keySampleMappings()[sni->note().key()]), (quint32)(sni->note().duration() / project.tempo() * 60 * 44100));
                }

                if (pcmWritten + newPcmSize > pcmSize) {
                    lastNoteItem = sni;
                    pcmSize = pcmWritten + newPcmSize;
                }
            }
            continue;
        }

        if ((sli = dynamic_cast<StreamLFOItem*>(items[i])) != nullptr) {
            encodeLFOItem(sli, data);
            continue;
        }

        if ((snfi = dynamic_cast<StreamNoiseFrequencyItem*>(items[i])) != nullptr) {
            encodeNoiseFrequencyItem(snfi, data);
            continue;
        } else if ((sefi = dynamic_cast<StreamEnvelopeFrequencyItem*>(items[i])) != nullptr) {
            encodeEnvelopeFrequencyItem(sefi, data);
            continue;
        } else if ((sesi = dynamic_cast<StreamEnvelopeShapeItem*>(items[i])) != nullptr) {
            encodeEnvelopeShapeItem(sesi, data);
            continue;
        }

        if ((suti = dynamic_cast<StreamUserToneItem*>(items[i])) != nullptr) {
            encodeUserToneItem(suti, data);
            continue;
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

    if (_format == Format::CHROMASOUND && pcm && samples > 8) {
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

void VGMStream::encodeSettingsItem(const Project& project, const StreamSettingsItem* item, QByteArray& data)
{
    const ToneChannelSettings* tcs;
    const NoiseChannelSettings* ncs;
    const FMChannelSettings* fmcs;
    const SSGChannelSettings* scs;
    const MelodyChannelSettings* mcs;
    const RhythmChannelSettings* rhcs;
    const ROMChannelSettings* rcs;
    if ((tcs = dynamic_cast<const ToneChannelSettings*>(item->channelSettings())) != nullptr) {
        int addr = (item->channel() * 2) + 1;
        int att;
        att = (30.0f * (float)(100 - item->channelSettings()->volume())/100.0f);
        att += (30.0f - att) * (float)(100 - item->velocity())/100.0f;
        att >>= 1;
        data.append(0x50);
        data.append(0x80 | (addr << 4) | att);
    } else if ((ncs = dynamic_cast<const NoiseChannelSettings*>(item->channelSettings())) != nullptr) {
        int fb = ncs->noiseType();
        int nfo = ncs->shiftRate();
        uint8_t datum = 0xE0 | (fb << 2) | nfo;

        data.append(0x50);
        data.append(datum);

        int att;
        att = (30.0f * (float)(100 - item->channelSettings()->volume())/100.0f);
        att += (30.0f - att) * (float)(100 - item->velocity())/100.0f;
        att >>= 1;
        data.append(0x50);
        data.append(0x80 | (7 << 4) | att);
    } else if ((fmcs = dynamic_cast<const FMChannelSettings*>(item->channelSettings())) != nullptr) {
        int part = 1 + (item->channel() >= 3);
        int channel = item->channel() % 3;

        uint8_t datum;
        QList<int> sls = slotsByAlg[fmcs->algorithm().algorithm()];
        for (int i = 0; i < 4; i++) {
            int offset = (i * 4) + channel;

            datum = (fmcs->operators()[i].dt() << 4) | fmcs->operators()[i].mul();
            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x30 + offset);
            data.append(datum);

            if (!sls.contains(i)) {
                data.append((part == 1) ? 0x52 : 0x53);
                data.append(0x40 + offset);
                data.append(fmcs->operators()[i].envelopeSettings().t1l());
            }

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

        for (int i = 0; i < sls.size(); i++) {
            int offset = (sls[i] * 4) + channel;
            int t1l = fmcs->operators()[sls[i]].envelopeSettings().t1l();

            int volume = fmcs->volume() * item->velocity() / 100;
            int amt = (127 - t1l) * (100 - volume)/100.0f;
            int newT1l = t1l + amt;

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0x40 + offset);
            data.append(newT1l);
        }

        datum = (fmcs->algorithm().feedback() << 3) | fmcs->algorithm().algorithm();
        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB0 + channel);
        data.append(datum);

        datum = 0xC0 | (fmcs->lfo().ams() << 4) | fmcs->lfo().fms();
        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB4 + channel);
        data.append(datum);
    } else if ((rcs = dynamic_cast<const ROMChannelSettings*>(item->channelSettings())) != nullptr) {
        if (_format == Format::CHROMASOUND) {
            int volume = rcs->volume() * item->velocity() / 100;
            int att = MAX_PCM_ATTENUATION * (float)(100 - volume) / 100;
            data.append(0xF0 | item->channel());
            data.append(att);
        }
    } else if ((scs = dynamic_cast<const SSGChannelSettings*>(item->channelSettings())) != nullptr) {
        uint8_t newMixer = _lastSSGMixer;

        uint8_t mask = 0b1001 << item->channel();

        newMixer &= ~mask;
        newMixer |= ((!scs->noise() << 3) | (!scs->tone())) << item->channel();

        data.append(0xA0);
        data.append(0x7);
        data.append(newMixer);

        _lastSSGMixer = newMixer;

        mask = 1 << 4;
        uint8_t datum = _lastSSGLevel[item->channel()];
        datum &= ~mask;
        datum |= scs->envelope() << 4;

        data.append(0xA0);
        data.append(0x8 + item->channel());
        data.append(datum);

        _lastSSGLevel[item->channel()] = datum;
    } else if ((mcs = dynamic_cast<const MelodyChannelSettings*>(item->channelSettings())) != nullptr) {
        uint8_t newChanVal = _lastChanVal[item->channel()];

        uint8_t mask = 0xF0;

        newChanVal &= ~mask;
        newChanVal |= (mcs->patch() << 4);

        data.append(0x51);
        data.append(0x30 + item->channel());
        data.append(newChanVal);

        _lastChanVal[item->channel()] = newChanVal;
    } else if ((rhcs = dynamic_cast<const RhythmChannelSettings*>(item->channelSettings())) != nullptr) {
        // Do nothing
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
            int key = item->note().key() % 12;
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

                int volume = fmcs->volume() * item->note().velocity() / 100;
                int amt = (127 - t1l) * (100 - volume)/100.0f;
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
        ROM rom(project.resolve(project.pcmFile()));

        if (_format == Format::CHROMASOUND) {
            if (item->on()) {
                const ROMChannelSettings* rcs = dynamic_cast<const ROMChannelSettings*>(item->channelSettings());

                if (rcs->keySampleMappings().contains(item->note().key())) {
                    int volume = rcs->volume() * item->note().velocity() / 100;
                    int att = MAX_PCM_ATTENUATION * (float)(100 - volume) / 100;
                    data.append(0xF0 | item->channel());
                    data.append(att);

                    quint32 offset = rom.offsets()[rcs->keySampleMappings()[item->note().key()]];
                    quint16 size = rom.size(rcs->keySampleMappings()[item->note().key()]);

                    data.append(0xD0 | item->channel());
                    data.append((char*)&size, sizeof(size));
                    data.append(0xE0 | item->channel());
                    data.append((char*)&offset, sizeof(offset));
                }
            } else {
                quint32 offset = -1;

                data.append(0xE0 | item->channel());
                data.append((char*)&offset, sizeof(offset));
            }
        }
    } else if (item->type() == Channel::Type::SSG) {
        const SSGChannelSettings* settings = dynamic_cast<const SSGChannelSettings*>(item->channelSettings());
        if (item->on()) {
            addr = item->channel() * 2;

            int octave = item->note().key() / 12;
            int key = item->note().key() % 12;
            float f = frequencies[key] * (float)qPow(2, octave);
            int n = 3579545.0f / (32.0f * f);
            uint8_t datum1 = n & 0xFF;
            uint8_t datum2 = n >> 8;
            data.append(0xA0);
            data.append(addr);
            data.append(datum1);
            data.append(0xA0);
            data.append(addr + 1);
            data.append(datum2);
        }

        addr = 0x8 + item->channel();
        uint8_t datum;
        if (item->on()) {
            int vol = 30.0f * (float)item->channelSettings()->volume()/100.0f
                      * (float)item->note().velocity()/100.0f;
            datum = vol >> 1;
            datum |= (settings->envelope() << 4);
        } else {
            datum = 0;
        }
        data.append(0xA0);
        data.append(addr);
        data.append(datum);
        _lastSSGLevel[item->channel()] = datum;
    } else if (item->type() == Channel::Type::MELODY) {
        const MelodyChannelSettings* settings = dynamic_cast<const MelodyChannelSettings*>(item->channelSettings());
        if (item->on()) {
            int octave = item->note().key() / 12;
            int key = item->note().key() % 12;
            int n = (octave << 9) | ym2413_frequencies[key];

            int volume = settings->volume() * item->note().velocity() / 100;
            int att = 30 * (100 - volume)/100.0f;
            att >>= 1;

            uint8_t newChanVal = _lastChanVal[item->channel()];
            uint8_t mask = 0x0F;
            newChanVal &= ~mask;
            newChanVal |= att;

            data.append(0x51);
            data.append(0x30 + item->channel());
            data.append(newChanVal);

            _lastChanVal[item->channel()] = newChanVal;

            data.append(0x51);
            data.append(0x10 + item->channel());
            data.append(n & 0xFF);

            data.append(0x51);
            data.append(0x20 + item->channel());
            data.append((n >> 8) | (1 << 4));
        } else {
            data.append(0x51);
            data.append(0x20 + item->channel());
            data.append((quint8)0);
        }
    } else if (item->type() == Channel::Type::RHYTHM) {
        const RhythmChannelSettings* settings = dynamic_cast<const RhythmChannelSettings*>(item->channelSettings());
        uint8_t newRhythm = _lastRhythm;
        uint8_t mask = 1 << (4 - settings->instrument());
        if (item->on()) {
            newRhythm |= mask;

            int volume = settings->volume() * item->note().velocity() / 100;
            int att = 30 * (100 - volume)/100.0f;
            att >>= 1;

            int offset = 0;
            bool high = false;
            switch (settings->instrument()) {
                case RhythmChannelSettings::BassDrum:
                    high = false;
                    break;
                case RhythmChannelSettings::SnareDrum:
                    high = false;
                    offset++;
                    break;
                case RhythmChannelSettings::HighHat:
                    high = true;
                    offset++;
                    break;
                case RhythmChannelSettings::TopCymbal:
                    high = false;
                    offset += 2;
                    break;
                case RhythmChannelSettings::TomTom:
                    high = true;
                    offset += 2;
                    break;
            }

            uint8_t newChanVal = _lastChanVal[6 + offset];
            uint8_t mask = 0x0F << (4 * high);
            newChanVal &= ~mask;
            newChanVal |= att << (4 * high);

            data.append(0x51);
            data.append(0x36 + offset);
            data.append(newChanVal);

            _lastChanVal[6 + offset] = newChanVal;

            data.append(0x51);
            data.append(0x0E);
            data.append((1 << 5) | newRhythm);
        } else {
            newRhythm &= ~mask;

            data.append(0x51);
            data.append(0x0E);
            data.append(((newRhythm > 0) << 5) |newRhythm);
        }
        _lastRhythm = newRhythm;
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

void VGMStream::encodeNoiseFrequencyItem(const StreamNoiseFrequencyItem* item, QByteArray& data)
{
    data.append(0xA0);
    data.append(0x6);
    data.append(item->freq());
}

void VGMStream::encodeEnvelopeFrequencyItem(const StreamEnvelopeFrequencyItem* item, QByteArray& data)
{
    data.append(0xA0);
    data.append(0xB);
    data.append(item->freq() & 0xFF);

    data.append(0xA0);
    data.append(0xC);
    data.append(item->freq() >> 8);
}

void VGMStream::encodeEnvelopeShapeItem(const StreamEnvelopeShapeItem* item, QByteArray& data)
{
    data.append(0xA0);
    data.append(0xD);
    data.append((item->settings().cont() << 3) |
                (item->settings().att() << 2) |
                (item->settings().alt() << 1) |
                item->settings().hold());
}

void VGMStream::encodeUserToneItem(const StreamUserToneItem* item, QByteArray& data)
{
    quint8 data00 = (item->settings().operators()[0].am() << 7) |
                    (item->settings().operators()[0].vib() << 6) |
                    (item->settings().operators()[0].envelopeSettings().type() << 5) |
                    (item->settings().operators()[0].ksr() << 4) |
                    item->settings().operators()[0].multi();
    quint8 data01 = (item->settings().operators()[1].am() << 7) |
                    (item->settings().operators()[1].vib() << 6) |
                    (item->settings().operators()[1].envelopeSettings().type() << 5) |
                    (item->settings().operators()[1].ksr() << 4) |
                    item->settings().operators()[1].multi();
    quint8 data02 = item->settings().tl() | (item->settings().operators()[0].ksl() << 6);
    quint8 data03 = (item->settings().operators()[1].d() << 4) |
                    (item->settings().operators()[0].d() << 3) |
                    (item->settings().fb()) |
                    (item->settings().operators()[1].ksl() << 6);
    quint8 data04 = (item->settings().operators()[0].envelopeSettings().ar() << 4) |
                    item->settings().operators()[0].envelopeSettings().dr();
    quint8 data05 = (item->settings().operators()[1].envelopeSettings().ar() << 4) |
                    item->settings().operators()[1].envelopeSettings().dr();
    quint8 data06 = (item->settings().operators()[0].envelopeSettings().sl() << 4) |
                    item->settings().operators()[0].envelopeSettings().rr();
    quint8 data07 = (item->settings().operators()[1].envelopeSettings().sl() << 4) |
                    item->settings().operators()[1].envelopeSettings().rr();

    QList<quint8> d = {{ data00, data01, data02, data03, data04, data05, data06, data07 }};

    for (int i = 0; i < d.size(); i++) {
        data.append(0x51);
        data.append((quint8)i);
        data.append(d[i]);
    }
}

QByteArray VGMStream::generateHeader(const Project& project, const QByteArray& data, const int totalSamples, const int loopOffsetData, const int gd3size, const bool selectionLoop)
{
    QByteArray headerData(128, 0);

    // VGM header
    headerData[0] = 'V';
    headerData[1] = 'g';
    headerData[2] = 'm';
    headerData[3] = ' ';

    // EOF
    *(uint32_t*)&headerData[0x4] = data.size() + gd3size + 128 - 0x4;
    // Version
    headerData[0x8] = 0x50;
    headerData[0x9] = 0x01;
    // SN76489 clock
    *(uint32_t*)&headerData[0xC] = 3579545;
    // YM2413 clock
    *(uint32_t*)&headerData[0x10] = project.usesOPL() ? 3579545 : 0;
    // GD3 offset
    *(uint32_t*)&headerData[0x14] = data.size() + 128 - 0x14;
    // Total samples
    *(uint32_t*)&headerData[0x18] = totalSamples;
    // Loop offset
    *(uint32_t*)&headerData[0x1C] = (loopOffsetData == 0) ? 0 : (loopOffsetData - 0x1C);
    // Loop # samples
    if (project.playMode() == Project::PlayMode::PATTERN) {
        *(uint32_t*)&headerData[0x20] = totalSamples;
    } else {
        if (selectionLoop) {
            *(uint32_t*)&headerData[0x20] = totalSamples;
        } else if (project.playlist().doesLoop()) {
            *(uint32_t*)&headerData[0x20] = totalSamples - project.playlist().loopOffsetSamples();
        } else {
            *(uint32_t*)&headerData[0x20] = 0;
        }
    }
    // SN76489AN flags
    *(uint16_t*)&headerData[0x28] = 0x0003;
    headerData[0x2A] = 15;
    // YM2612 clock
    *(uint32_t*)&headerData[0x2C] = project.usesOPL() ? 0 : 7680000;
    // Data offset
    *(uint32_t*)&headerData[0x34] = headerData.size() - 0x34;
    // AY8910 clock
    *(uint32_t*)&headerData[0x74] = 1789773;

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
    _acquiredEver = false;
}

VGMStream::StreamSettingsItem::StreamSettingsItem(const float time, const int channel, const ChannelSettings* channelSettings, const int velocity)
    : StreamItem(time)
    , _channel(channel)
    , _channelSettings(channelSettings)
    , _velocity(velocity)
{

}

const ChannelSettings* VGMStream::StreamSettingsItem::channelSettings() const
{
    return _channelSettings;
}

int VGMStream::StreamSettingsItem::channel() const
{
    return _channel;
}

int VGMStream::StreamSettingsItem::velocity() const
{
    return _velocity;
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

SSGChannelSettings& VGMStream::SSGChannel::settings()
{
    return _settings;
}

void VGMStream::SSGChannel::reset()
{
    PhysicalChannel::reset();
    _settings = SSGChannelSettings();
}

VGMStream::StreamNoiseFrequencyItem::StreamNoiseFrequencyItem(const float time, const int freq)
    : StreamItem(time)
    , _freq(freq)
{

}

int VGMStream::StreamNoiseFrequencyItem::freq() const
{
    return _freq;
}

VGMStream::StreamEnvelopeFrequencyItem::StreamEnvelopeFrequencyItem(const float time, const int freq)
    : StreamItem(time)
    , _freq(freq)
{

}

int VGMStream::StreamEnvelopeFrequencyItem::freq() const
{
    return _freq;
}

VGMStream::StreamEnvelopeShapeItem::StreamEnvelopeShapeItem(const float time, const SSGEnvelopeSettings& shape)
    : StreamItem(time)
    , _settings(shape)
{

}

const SSGEnvelopeSettings& VGMStream::StreamEnvelopeShapeItem::settings() const
{
    return _settings;
}

MelodyChannelSettings& VGMStream::MelodyChannel::settings()
{
    return _settings;
}

void VGMStream::MelodyChannel::reset()
{
    PhysicalChannel::reset();
    _settings = MelodyChannelSettings();
}

RhythmChannelSettings& VGMStream::RhythmChannel::settings()
{
    return _settings;
}

void VGMStream::RhythmChannel::reset()
{
    PhysicalChannel::reset();
    _settings = RhythmChannelSettings();
}

VGMStream::StreamUserToneItem::StreamUserToneItem(const float time, const OPLSettings& tone)
    : StreamItem(time)
    , _settings(tone)
{

}

const OPLSettings& VGMStream::StreamUserToneItem::settings() const
{
    return _settings;
}

VGMStream::PCMChannel::PCMChannel()
{

}
