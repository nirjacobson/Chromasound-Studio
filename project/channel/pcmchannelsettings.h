#ifndef PCMCHANNELSETTINGS_H
#define PCMCHANNELSETTINGS_H

#include <QMap>

#include "channelsettings.h"

class PCMChannelSettings : public ChannelSettings
{
public:
    PCMChannelSettings();

    QMap<int, int>& keySampleMappings();
    const QMap<int, int>& keySampleMappings() const;
    void setSample(const int key, const int patch);

    bool operator==(const PCMChannelSettings& o) const;

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

#endif // PCMCHANNELSETTINGS_H
