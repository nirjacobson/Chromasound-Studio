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

    if (_settings) delete _settings;

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
        case PCM:
            _settings = new PCMChannelSettings;
            break;
        case SSG:
            _settings = new SSGChannelSettings;
            break;
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
        case PCM:
            return "PCM";
        case SSG:
            return "SSG";
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

    if (str == "PCM") {
        return Channel::Type::PCM;
    }

    if (str == "SSG") {
        return Channel::Type::SSG;
    }

    return Channel::Type::TONE;
}
