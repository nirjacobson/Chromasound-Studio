#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include "settings.h"

class ChannelSettings : public Settings
{
    public:
        ChannelSettings();
        virtual ~ChannelSettings();

        int volume() const;
        void setVolume(const int volume);

    private:
        int _volume;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // CHANNELSETTINGS_H
