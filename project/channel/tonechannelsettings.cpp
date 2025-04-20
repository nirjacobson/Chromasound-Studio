#include "tonechannelsettings.h"
#include "project/channel/channel.h"

ToneChannelSettings::ToneChannelSettings()
{

}

QString ToneChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::TONE);
}

ChannelSettings* ToneChannelSettings::copy() const
{
    ToneChannelSettings* tcs = new ToneChannelSettings;
    *tcs = *this;

    return tcs;
}

bool ToneChannelSettings::supportsPitchChanges()
{
    return true;
}
