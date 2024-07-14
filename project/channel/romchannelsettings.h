#ifndef ROMCHANNELSETTINGS_H
#define ROMCHANNELSETTINGS_H

#include <QMap>

#include "channelsettings.h"

class ROMChannelSettings : public ChannelSettings
{
public:
    ROMChannelSettings();

    const QMap<int, int>& keySampleMappings() const;
    void setSample(const int key, const int patch);

    bool operator==(const ROMChannelSettings& o) const;

private:
    QMap<int, int> _keySampleMappings;

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
