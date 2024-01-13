#include "rhythmchannelsettings.h"
#include "project/channel/channel.h"

RhythmChannelSettings::RhythmChannelSettings()
{

}

RhythmChannelSettings::Instrument RhythmChannelSettings::instrument() const
{
    return _instrument;
}

void RhythmChannelSettings::setInstrument(const Instrument instr)
{
    _instrument = instr;
}

QString RhythmChannelSettings::instrumentToString(const Instrument instr)
{
    switch (instr) {

        case BassDrum:
            return "BassDrum";
        case SnareDrum:
            return "SnareDrum";
        case TomTom:
            return "TomTom";
        case TopCymbal:
            return "TopCymbal";
        case HighHat:
            return "HighHat";
    }

    return "BassDrum";
}

RhythmChannelSettings::Instrument RhythmChannelSettings::instrumentFromString(const QString& str)
{
    if (str == "BassDrum") {
        return Instrument::BassDrum;
    }

    if (str == "SnareDrum") {
        return Instrument::SnareDrum;
    }

    if (str == "TomTom") {
        return Instrument::TomTom;
    }

    if (str == "TopCymbal") {
        return Instrument::TopCymbal;
    }

    if (str == "HighHat") {
        return Instrument::HighHat;
    }

    return Instrument::BassDrum;
}

bson_t RhythmChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_UTF8(&bson, "instrument", instrumentToString(_instrument).toStdString().c_str());

    return bson;
}

void RhythmChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t instrument;

    if (bson_iter_find_descendant(&bson, "instrument", &instrument) && BSON_ITER_HOLDS_UTF8(&instrument)) {
        _instrument = instrumentFromString(bson_iter_utf8(&instrument, nullptr));
    }
}

QString RhythmChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::RHYTHM);
}

ChannelSettings* RhythmChannelSettings::copy() const
{
    RhythmChannelSettings* rcs = new RhythmChannelSettings;
    *rcs = *this;

    return rcs;
}
