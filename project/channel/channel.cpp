#include "channel.h"

Channel::Channel()
    : _enabled(true)
    , _name("Channel")
    , _settings(nullptr)
{
    setType(Type::TONE);
}

Channel::Channel(const QString& name)
    : _enabled(true)
    , _name(name)
    , _settings(nullptr)
{
    setType(Type::TONE);
}

bool Channel::enabled() const
{
    return _enabled;
}

void Channel::setEnabled(const bool enabled)
{
    _enabled = enabled;
}

Channel::Type Channel::type() const
{
    return _type;
}

void Channel::setType(const Type type)
{
    _type = type;

    ChannelSettings* oldSettings = _settings;

    switch (type) {
        case TONE:
            _settings = new ToneChannelSettings;
            break;
        case NOISE:
            _settings = new NoiseChannelSettings;
            break;
        case FM:
            _settings = new FMChannelSettings;
            break;
        case SSG:
            _settings = new SSGChannelSettings;
            break;
        case MELODY:
            _settings = new MelodyChannelSettings;
            break;
        case RHYTHM:
            _settings = new RhythmChannelSettings;
            break;
        case PCM:
            _settings = new PCMChannelSettings;
            break;
    }

    if (oldSettings) {
        _settings->setVolume(oldSettings->volume());
        delete oldSettings;
    }
}

QString Channel::name() const
{
    return _name;
}

void Channel::setName(const QString& name)
{
    _name = name;
}

ChannelSettings& Channel::settings()
{
    return *_settings;
}

const ChannelSettings& Channel::settings() const
{
    return *_settings;
}


QString Channel::channelTypeToString(const Type type)
{
    switch (type) {
        case TONE:
            return "TONE";
        case NOISE:
            return "NOISE";
        case FM:
            return "FM";
        case SSG:
            return "SSG";
        case MELODY:
            return "MELODY";
        case RHYTHM:
            return "RHYTHM";
        case PCM:
            return "PCM";
    }

    return "TONE";
}

Channel::Type Channel::channelTypeFromString(const QString& str)
{
    if (str == "TONE") {
        return Channel::Type::TONE;
    }

    if (str == "NOISE") {
        return Channel::Type::NOISE;
    }

    if (str == "FM") {
        return Channel::Type::FM;
    }

    if (str == "SSG") {
        return Channel::Type::SSG;
    }

    if (str == "MELODY") {
        return Channel::Type::MELODY;
    }

    if (str == "RHYTHM") {
        return Channel::Type::RHYTHM;
    }

    if (str == "PCM") {
        return Channel::Type::PCM;
    }

    return Channel::Type::TONE;
}
