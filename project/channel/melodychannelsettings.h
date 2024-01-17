#ifndef MELODYCHANNELSETTINGS_H
#define MELODYCHANNELSETTINGS_H

#include "channelsettings.h"

class MelodyChannelSettings : public ChannelSettings
{
    public:
        MelodyChannelSettings();

        int patch() const;
        void setPatch(const int patch);

        bool operator==(const MelodyChannelSettings& o) const;

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

#endif // MELODYCHANNELSETTINGS_H
