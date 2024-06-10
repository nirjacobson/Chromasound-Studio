#ifndef ROMCHANNELSETTINGS_H
#define ROMCHANNELSETTINGS_H

#include "channelsettings.h"

class ROMChannelSettings : public ChannelSettings
{
public:
    ROMChannelSettings();

    int patch() const;
    void setPatch(const int patch);

    bool operator==(const ROMChannelSettings& o) const;

private:
    int _patch;

    // Settings interface
public:
    bson_t toBSON() const;
    void fromBSON(bson_iter_t& bson);

    // ChannelSettings interface
public:
    QString type() const;
    ChannelSettings* copy() const;

};

#endif // ROMCHANNELSETTINGS_H
