#include "tonechannelsettings.h"
#include "project/channel/channel.h"

ToneChannelSettings::ToneChannelSettings()
{

}

QString ToneChannelSettings::type() const
{
    return Channel::channelTypeToString(Channel::Type::TONE);
}
