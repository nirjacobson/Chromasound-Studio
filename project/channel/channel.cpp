#include "channel.h"

Channel::Channel()
    : _enabled(true)
    , _type(Type::NONE)
    , _name("Channel")
    , _settings(nullptr)
{

}
Channel::Channel(const QString& name)
    : _enabled(true)
    , _type(Type::NONE)
    , _name(name)
    , _settings(nullptr)
{

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
        case NONE:
            _settings = nullptr;
            break;
        case TONE:
            _settings = nullptr;
            break;
        case NOISE:
            _settings = new NoiseChannelSettings;
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

ChannelSettings& Channel::data()
{
    return *_settings;
}

QString Channel::channelTypeToString(const Type type)
{
    switch (type) {
        case NONE:
            return "NONE";
        case TONE:
            return "TONE";
        case NOISE:
            return "NOISE";
    }

    return "NONE";
}

Channel::Type Channel::channelTypeFromString(const QString& str)
{
    if (str == "NONE") {
        return Channel::Type::NONE;
    }

    if (str == "TONE") {
        return Channel::Type::TONE;
    }

    if (str == "NOISE") {
        return Channel::Type::NOISE;
    }

    return Channel::Type::NONE;
}
