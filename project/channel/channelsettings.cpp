#include "channelsettings.h"

ChannelSettings::ChannelSettings()
    : _volume(100)
    , _pitchRange(2)
    , _octaveOffset(0)
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

int ChannelSettings::pitchRange() const
{
    return _pitchRange;
}

void ChannelSettings::setPitchRange(const int semitones)
{
    _pitchRange = semitones;
}

int ChannelSettings::octaveOffset() const
{
    return _octaveOffset;
}

void ChannelSettings::setOctaveOffset(const int offset)
{
    _octaveOffset = offset;
}

bson_t ChannelSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    BSON_APPEND_INT32(&bson, "volume", _volume);
    BSON_APPEND_UTF8(&bson,  "type", type().toStdString().c_str());
    BSON_APPEND_INT32(&bson, "pitchRange", _pitchRange);
    BSON_APPEND_INT32(&bson, "octaveOffset", _octaveOffset);

    return bson;
}

void ChannelSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t volume;
    bson_iter_t octaveOffset;
    bson_iter_t pitchRange;

    if (bson_iter_find_descendant(&bson, "volume", &volume) && BSON_ITER_HOLDS_INT(&volume)) {
        _volume = bson_iter_int32(&volume);
    }
    if (bson_iter_find_descendant(&bson, "pitchRange", &pitchRange)) {
        _pitchRange = bson_iter_int32(&pitchRange);
    }
    if (bson_iter_find_descendant(&bson, "octaveOffset", &octaveOffset) && BSON_ITER_HOLDS_INT(&octaveOffset)) {
        _octaveOffset = bson_iter_int32(&octaveOffset);
    }
}
