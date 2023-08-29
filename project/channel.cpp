#include "channel.h"

Channel::Channel()
    : _enabled(true)
    , _name("Channel")
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

QString Channel::name() const
{
    return _name;
}

void Channel::setName(const QString& name)
{
    _name = name;
}
