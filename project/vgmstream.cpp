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

}

QByteArray VGMStream::compile(const Project& project)
{
    QList<StreamItem*> items;
    QByteArray data;

    if (project.playMode() == Project::PlayMode::PATTERN) {
        processPattern(0, project, project.getFrontPattern(), items);
        assignChannelsAndExpand(items);
        pad(items, qCeil(project.getFrontPattern().getLength() / project.beatsPerBar()) * project.beatsPerBar());
        encode(items, project.tempo(), data);

        for (StreamItem* item : items) {
            delete item;
        }

        return data;
    } else {
        generateItems(project, items);
        assignChannelsAndExpand(items);
        encode(items, project.tempo(), data);

        for (StreamItem* item : items) {
            delete item;
        }

        return data;
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
        if (_noiseChannels[i].acquire(time, duration)) {
            if (_noiseChannels[i].settings() != *settings) {
                items.append(new StreamSettingsItem(time, i, settings));
            }
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
            items.append(new StreamSettingsItem(time, i, settings));
            return i;
        }
    }

    return -1;
}

void VGMStream::processPattern(const float time, const Project& project, const Pattern& pattern, QList<StreamItem*>& items)
{
    for (auto it = pattern.tracks().cbegin(); it != pattern.tracks().cend(); ++it) {
        const Channel& channel = project.getChannel(it.key());
        const Track* track = it.value();

        if (channel.enabled()) {
            processTrack(time, channel, track, items);
        }
    }
}

void VGMStream::processTrack(const float time, const Channel& channel, const Track* track, QList<StreamItem*>& items)
{
    QList<Track::Item*> itemsCopy = track->items();
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Track::Item* a, const Track::Item* b){ return a->time() <= b->time(); });

    for (const Track::Item* item : itemsCopy) {
        items.append(new StreamNoteItem(time + item->time(), channel.type(), item->note(), &channel.settings()));
    }
}

void VGMStream::generateItems(const Project& project, QList<StreamItem*>& items)
{
    QList<Project::Playlist::Item*> itemsCopy(project.playlist().items());
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Project::Playlist::Item* a, const Project::Playlist::Item* b){ return a->time() <= b->time(); });

    for (const Project::Playlist::Item* item : itemsCopy) {
        processPattern(item->time(), project, project.getPattern(item->pattern()), items);
    }
}

void VGMStream::assignChannelsAndExpand(QList<StreamItem*>& items)
{
    QList<StreamItem*> itemsCopy = items;
    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const StreamItem* a, const StreamItem* b){ return a->time() <= b->time(); });

    for (StreamItem* item : itemsCopy) {
        StreamNoteItem* noteItem = dynamic_cast<StreamNoteItem*>(item);
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
        }
        StreamNoteItem* noteOffItem = new StreamNoteItem(*noteItem);
        noteOffItem->setTime(noteItem->time() + noteItem->note().duration());
        noteOffItem->setOn(false);
        items.append(noteOffItem);
    }

    std::sort(items.begin(), items.end(), [](const StreamItem* a, const StreamItem* b){
        if (a->time() == b->time()) {
            return dynamic_cast<const StreamNoteItem*>(b) != nullptr;
        }

        return a->time() < b->time();
    });
}

void VGMStream::pad(QList<StreamItem*>& items, const float toDuration)
{
    StreamEndItem* sei = new StreamEndItem(toDuration);
    items.append(sei);
}

void VGMStream::encode(const QList<StreamItem*> items, const int tempo, QByteArray& data)
{
    float lastTime = 0.0f;
    for (int i = 0; i < items.size(); i++) {
        encodeDelay(tempo, items[i]->time() - lastTime, data);
        lastTime = items[i]->time();
        StreamSettingsItem* ssi;
        StreamNoteItem* sni;
        if ((ssi = dynamic_cast<StreamSettingsItem*>(items[i])) != nullptr) {
            encodeSettingsItem(ssi, data);
        } else if ((sni = dynamic_cast<StreamNoteItem*>(items[i])) != nullptr) {
            encodeNoteItem(sni, data);
        }
    }
    data.append(0x66);
}

void VGMStream::encodeDelay(const int tempo, const float beats, QByteArray& data)
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

        datum = (fmcs->feedback() << 3) | fmcs->algorithm();
        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB0 + channel);
        data.append(datum);

        data.append((part == 1) ? 0x52 : 0x53);
        data.append(0xB4 + channel);
        data.append(0xC0);
    }
}

void VGMStream::encodeNoteItem(const StreamNoteItem* item, QByteArray& data)
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
            float f = frequencies[key] * qPow(2, octave);
            int n = 3579545 / (32 * f);
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
            att = (float)(100 - item->note().velocity())/100.0f * 30.0f;
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
            att = (float)(100 - item->note().velocity())/100.0f * 30.0f;
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
            int n = (octave << 10) | ym2612_frequencies[key];

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0xA4 + channel);
            data.append(n >> 10);

            data.append((part == 1) ? 0x52 : 0x53);
            data.append(0xA0 + channel);
            data.append(n & 0xFF);

            QList<int> sls = slotsByAlg[fmcs->algorithm()];
            for (int i = 0; i < sls.size(); i++) {
                int offset = (sls[i] * 4) + channel;
                int t1l = fmcs->operators()[sls[i]].envelopeSettings().t1l();
                int newT1l = t1l + (127 - t1l) * (100 - item->note().velocity())/100.0f;

                data.append((part == 1) ? 0x52 : 0x53);
                data.append(0x40 + offset);
                data.append(newT1l);
            }
        }

        data.append(0x52);
        int c = (item->channel() < 3) ? item->channel() : (1 + item->channel());
        data.append(0x28, ((item->on() ? 0xF : 0x0) << 4) | c);
    }
}

VGMStream::StreamNoteItem::StreamNoteItem(const float time, const Channel::Type type, const Note& note, const Settings* channelSettings)
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

const Settings* VGMStream::StreamNoteItem::channelSettings() const
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
    if (_time <= time && time < (_time+_duration)) {
        return false;
    }

    _time = time;
    _duration = duration;

    return true;
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
