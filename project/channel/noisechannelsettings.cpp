#include "noisechannelsettings.h"

NoiseChannelSettings::NoiseChannelSettings()
    : _type(NoiseType::Periodic)
    , _rate(ShiftRate::FIVE_TWELVE)
{

}

NoiseChannelSettings::NoiseType NoiseChannelSettings::noiseType() const
{
    return _type;
}

void NoiseChannelSettings::setNoiseType(const NoiseType type)
{
    _type = type;
}

NoiseChannelSettings::ShiftRate NoiseChannelSettings::shiftRate() const
{
    return _rate;
}

void NoiseChannelSettings::setShiftRate(const ShiftRate rate)
{
    _rate = rate;
}

bson_t NoiseChannelSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);
    BSON_APPEND_UTF8(&bson, "_type", "NOISE");
    BSON_APPEND_UTF8(&bson, "type", noiseTypeToString(_type).toStdString().c_str());
    BSON_APPEND_UTF8(&bson, "shiftRate", shiftRateToString(_rate).toStdString().c_str());

    return bson;
}

void NoiseChannelSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t __type;
    bson_iter_t type;
    bson_iter_t rate;

    if (bson_iter_find_descendant(&bson, "_type", &__type) && BSON_ITER_HOLDS_UTF8(&__type)) {
        if (QString(bson_iter_utf8(&__type, nullptr)) == "NOISE") {

            if (bson_iter_find_descendant(&bson, "type", &type) && BSON_ITER_HOLDS_UTF8(&type)) {
                _type = noiseTypeFromString(bson_iter_utf8(&type, nullptr));
            }
            if (bson_iter_find_descendant(&bson, "shiftRate", &rate) && BSON_ITER_HOLDS_UTF8(&rate)) {
                _rate = shiftRateFromString(bson_iter_utf8(&rate, nullptr));
            }
        }
    }
}

bool NoiseChannelSettings::operator==(const NoiseChannelSettings& other) const
{
    return _type == other._type && _rate == other._rate;
}

bool NoiseChannelSettings::operator!=(const NoiseChannelSettings& other) const
{
    return !operator==(other);
}

QString NoiseChannelSettings::noiseTypeToString(const NoiseType type)
{
    switch(type) {
        case Periodic:
            return "PERIODIC";
        case White:
            return "WHITE";
    }

    return "INVALID";
}

QString NoiseChannelSettings::shiftRateToString(const ShiftRate rate)
{
    switch (rate) {
        case FIVE_TWELVE:
            return "1/512";
        case TEN_TWENTYFOUR:
            return "1/1024";
        case TWENTY_FORTYEIGHT:
            return "1/2048";
    }
    return "INVALID";
}

NoiseChannelSettings::NoiseType NoiseChannelSettings::noiseTypeFromString(const QString& str)
{
    if (str == "WHITE") {
        return NoiseType::White;
    }

    return NoiseType::Periodic;
}

NoiseChannelSettings::ShiftRate NoiseChannelSettings::shiftRateFromString(const QString& str)
{
    if (str == "1/1024") {
        return ShiftRate::TEN_TWENTYFOUR;
    }

    if (str == "1/2048") {
        return ShiftRate::TWENTY_FORTYEIGHT;
    }

    return ShiftRate::FIVE_TWELVE;
}
