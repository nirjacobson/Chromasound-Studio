#include "romchannelsettings.h"
#include "project/channel/channel.h"

ROMChannelSettings::ROMChannelSettings()
    : _patch(0)
{

}

int ROMChannelSettings::patch() const
{
    return _patch;
}

void ROMChannelSettings::setPatch(const int patch)
{
    _patch = patch;
}

bool ROMChannelSettings::operator==(const ROMChannelSettings& o) const
{
    return _patch == o._patch;
}

bson_t ROMChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_INT32(&bson, "patch", _patch);

    return bson;
}

void ROMChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t patch;

    if (bson_iter_find_descendant(&bson, "patch", &patch) && BSON_ITER_HOLDS_INT(&patch)) {
        _patch = bson_iter_int32(&patch);
    }
}

QString ROMChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::ROM);
}

ChannelSettings* ROMChannelSettings::copy() const
{
    ROMChannelSettings* rcs = new ROMChannelSettings;
    *rcs = *this;

    return rcs;
}
