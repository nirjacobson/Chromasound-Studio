#include "channelsettings.h"

ChannelSettings::ChannelSettings()
    : _volume(100)
{

}

ChannelSettings::~ChannelSettings()
{

}

int ChannelSettings::volume() const
{
    return _volume;
}

void ChannelSettings::setVolume(const int volume)
{
    _volume = volume;
}


bson_t ChannelSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    BSON_APPEND_INT32(&bson, "volume", _volume);

    return bson;
}

void ChannelSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t volume;

    if (bson_iter_find_descendant(&bson, "volume", &volume) && BSON_ITER_HOLDS_INT(&volume)) {
        _volume = bson_iter_int32(&volume);
    }
}
