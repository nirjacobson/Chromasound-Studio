#include "pcmchannelsettings.h"
#include "project/channel/channel.h"

PCMChannelSettings::PCMChannelSettings()
{

}

QMap<int, int>& PCMChannelSettings::keySampleMappings()
{
    return _keySampleMappings;
}

const QMap<int, int>& PCMChannelSettings::keySampleMappings() const
{
    return _keySampleMappings;
}

void PCMChannelSettings::setSample(const int key, const int sample)
{
    _keySampleMappings[key] = sample;
}

bool PCMChannelSettings::operator==(const PCMChannelSettings& o) const
{
    return _keySampleMappings == o._keySampleMappings;
}

bson_t PCMChannelSettings::toBSON() const
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

void PCMChannelSettings::fromBSON(bson_iter_t& bson)
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

QString PCMChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::PCM);
}

ChannelSettings* PCMChannelSettings::copy() const
{
    PCMChannelSettings* rcs = new PCMChannelSettings;
    *rcs = *this;

    return rcs;
}
