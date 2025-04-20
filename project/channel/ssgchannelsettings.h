#ifndef SSGCHANNELSETTINGS_H
#define SSGCHANNELSETTINGS_H

#include "channelsettings.h"

class SSGChannelSettings : public ChannelSettings
{
    public:
        SSGChannelSettings();

        bool envelope() const;
        void setEnvelope(const bool enabled);

        bool noise() const;
        void setNoise(const bool enabled);

        bool tone() const;
        void setTone(const bool enabled);

        bool operator==(const SSGChannelSettings& other) const;

        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
        QString type() const;
        ChannelSettings* copy() const;

    private:
        bool _noise;
        bool _tone;
        bool _envelope;

        // ChannelSettings interface
    public:
        bool supportsPitchChanges();
};

#endif // SSGCHANNELSETTINGS_H
