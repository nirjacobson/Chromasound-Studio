#include "melodychannelsettings.h"
#include "project/channel/channel.h"

MelodyChannelSettings::MelodyChannelSettings()
    : _patch(0)
{

}

int MelodyChannelSettings::patch() const
{
    return _patch;
}

void MelodyChannelSettings::setPatch(const int patch)
{
    _patch = patch;
}

bool MelodyChannelSettings::operator==(const MelodyChannelSettings& o) const
{
    return _patch == o._patch;
}

bson_t MelodyChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_INT32(&bson, "patch", _patch);

    return bson;
}

void MelodyChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t patch;

    if (bson_iter_find_descendant(&bson, "patch", &patch) && BSON_ITER_HOLDS_INT(&patch)) {
        _patch = bson_iter_int32(&patch);
    }
}

QString MelodyChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::MELODY);
}

ChannelSettings* MelodyChannelSettings::copy() const
{
    MelodyChannelSettings* mcs = new MelodyChannelSettings;
    *mcs = *this;

    return mcs;
}
