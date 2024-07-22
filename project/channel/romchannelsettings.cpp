#include "romchannelsettings.h"
#include "project/channel/channel.h"

ROMChannelSettings::ROMChannelSettings()
{

}

QMap<int, int>& ROMChannelSettings::keySampleMappings()
{
    return _keySampleMappings;
}

const QMap<int, int>& ROMChannelSettings::keySampleMappings() const
{
    return _keySampleMappings;
}

void ROMChannelSettings::setSample(const int key, const int sample)
{
    _keySampleMappings[key] = sample;
}

bool ROMChannelSettings::operator==(const ROMChannelSettings& o) const
{
    return _keySampleMappings == o._keySampleMappings;
}

bson_t ROMChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    bson_t mappings;

    BSON_APPEND_DOCUMENT_BEGIN(&bson, "keySampleMappings", &mappings);
    for (auto it = _keySampleMappings.begin(); it != _keySampleMappings.end(); ++it) {
        char keybuff[8];
        const char* key;
        bson_uint32_to_string(it.key(), &key, keybuff, sizeof keybuff);

        BSON_APPEND_INT32(&mappings, key, it.value());
    }
    bson_append_document_end(&bson, &mappings);

    return bson;
}

void ROMChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t mappings;
    bson_iter_t child;

    if (bson_iter_find_descendant(&bson, "keySampleMappings", &mappings) && BSON_ITER_HOLDS_DOCUMENT(&mappings) && bson_iter_recurse(&mappings, &child)) {
        while (bson_iter_next(&child)) {
            const char* key = bson_iter_key(&child);
            int sample = bson_iter_int32(&child);

            _keySampleMappings[QString(key).toInt()] = sample;
        }
    }
}

QString ROMChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::SPCM);
}

ChannelSettings* ROMChannelSettings::copy() const
{
    ROMChannelSettings* rcs = new ROMChannelSettings;
    *rcs = *this;

    return rcs;
}
