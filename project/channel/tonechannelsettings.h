#ifndef TONECHANNELSETTINGS_H
#define TONECHANNELSETTINGS_H

#include "channelsettings.h"

class ToneChannelSettings : public ChannelSettings
{
public:
    ToneChannelSettings();

    // ChannelSettings interface
public:
    QString type() const;
};

#endif // TONECHANNELSETTINGS_H
