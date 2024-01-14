#include "melodychannelsettings.h"
#include "project/channel/channel.h"

MelodyChannelSettings::MelodyChannelSettings()
    : _instrument(Instrument::User)
{

}

MelodyChannelSettings::Instrument MelodyChannelSettings::instrument() const
{
    return _instrument;
}

void MelodyChannelSettings::setInstrument(const Instrument instr)
{
    _instrument = instr;
}

bool MelodyChannelSettings::operator==(const MelodyChannelSettings& o) const
{
    return _instrument == o._instrument;
}

QString MelodyChannelSettings::instrumentToString(const Instrument instr)
{
    switch (instr) {
        case User:
            return "User";
        case Violin:
            return "Violin";
        case Guitar:
            return "Guitar";
        case Piano:
            return "Piano";
        case Flute:
            return "Flute";
        case Clarinet:
            return "Clarinet";
        case Oboe:
            return "Oboe";
        case Trumpet:
            return "Trumpet";
        case Organ:
            return "Organ";
        case Horn:
            return "Horn";
        case Synthesizer:
            return "Synthesizer";
        case Harpsichord:
            return "Harpsichord";
        case Vibraphone:
            return "Vibraphone";
        case SynthesizerBass:
            return "SyntehesizerBass";
        case AcousticBass:
            return "AcousticBass";
        case ElectricGuitar:
            return "ElectricGuitar";
    }

    return "User";
}

MelodyChannelSettings::Instrument MelodyChannelSettings::instrumentFromString(const QString& str)
{
    if (str == "User") {
        return Instrument::User;
    }

    if (str == "Violin") {
        return Instrument::Violin;
    }

    if (str == "Guitar") {
        return Instrument::Guitar;
    }

    if (str == "Piano") {
        return Instrument::Piano;
    }

    if (str == "Flute") {
        return Instrument::Flute;
    }

    if (str == "Clarinet") {
        return Instrument::Clarinet;
    }

    if (str == "Oboe") {
        return Instrument::Oboe;
    }

    if (str == "Trumpet") {
        return Instrument::Trumpet;
    }

    if (str == "Organ") {
        return Instrument::Organ;
    }

    if (str == "Horn") {
        return Instrument::Horn;
    }

    if (str == "Synthesizer") {
        return Instrument::Synthesizer;
    }

    if (str == "Harpsichord") {
        return Instrument::Harpsichord;
    }

    if (str == "Vibraphone") {
        return Instrument::Vibraphone;
    }

    if (str == "SynthesizerBass") {
        return Instrument::SynthesizerBass;
    }

    if (str == "AcousticBass") {
        return Instrument::AcousticBass;
    }

    if (str == "ElectricGuitar") {
        return Instrument::ElectricGuitar;
    }

    return Instrument::User;
}

bson_t MelodyChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    BSON_APPEND_UTF8(&bson, "instrument", instrumentToString(_instrument).toStdString().c_str());

    return bson;
}

void MelodyChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t instrument;

    if (bson_iter_find_descendant(&bson, "instrument", &instrument) && BSON_ITER_HOLDS_UTF8(&instrument)) {
        _instrument = instrumentFromString(bson_iter_utf8(&instrument, nullptr));
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
