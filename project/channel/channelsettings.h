#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <libbson-1.0/bson/bson.h>

class ChannelSettings
{
    public:
        virtual ~ChannelSettings();

        virtual bson_t toBSON() const = 0;
        virtual void fromBSON(bson_iter_t& bson) = 0;
};

#endif // CHANNELSETTINGS_H
