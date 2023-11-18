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

    // ChannelSettings interface
public:
    ChannelSettings* copy() const;
};

#endif // TONECHANNELSETTINGS_H
