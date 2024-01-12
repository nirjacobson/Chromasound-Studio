#include "ssgchannelsettings.h"
#include "project/channel/channel.h"

SSGChannelSettings::SSGChannelSettings()
    : _envelope(false)
    , _noise(false)
    , _tone(false)
{

}

bool SSGChannelSettings::envelope() const
{
    return _envelope;
}

void SSGChannelSettings::setEnvelope(const bool enabled)
{
    _envelope = enabled;
}

bool SSGChannelSettings::noise() const
{
    return _noise;
}

void SSGChannelSettings::setNoise(const bool enabled)
{
    _noise = enabled;
}

bool SSGChannelSettings::tone() const
{
    return _tone;
}

void SSGChannelSettings::setTone(const bool enabled)
{
    _tone = enabled;
}

bool SSGChannelSettings::operator==(const SSGChannelSettings& other) const
{
    return _envelope == other._envelope &&
           _noise == other._noise &&
           _tone == other._tone;
}

bson_t SSGChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_BOOL(&bson, "envelope", _envelope);
    BSON_APPEND_BOOL(&bson, "noise", _noise);
    BSON_APPEND_BOOL(&bson, "tone", _tone);

    return bson;
}

void SSGChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t envelope;
    bson_iter_t noise;
    bson_iter_t tone;

    if (bson_iter_find_descendant(&bson, "envelope", &envelope) && BSON_ITER_HOLDS_BOOL(&envelope)) {
        _envelope = bson_iter_bool(&envelope);
    }

    if (bson_iter_find_descendant(&bson, "noise", &noise) && BSON_ITER_HOLDS_BOOL(&noise)) {
        _noise = bson_iter_bool(&noise);
    }

    if (bson_iter_find_descendant(&bson, "tone", &tone) && BSON_ITER_HOLDS_BOOL(&tone)) {
        _tone = bson_iter_bool(&tone);
    }
}

QString SSGChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::SSG);
}

ChannelSettings* SSGChannelSettings::copy() const
{
    SSGChannelSettings* cs = new SSGChannelSettings;
    *cs = *this;

    return cs;
}
