#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QString>

#include "settings.h"

class ChannelSettings : public Settings
{
    public:
        ChannelSettings();
        virtual ~ChannelSettings();

        int volume() const;
        void setVolume(const int volume);

        virtual QString type() const = 0;

        virtual ChannelSettings* copy() const = 0;

        virtual bool supportsPitchChanges() = 0;

        int pitchRange() const;
        void setPitchRange(const int semitones);

        int octaveOffset() const;
        void setOctaveOffset(const int offset);

    private:
        int _volume;
        int _octaveOffset;
        int _pitchRange;

        // Settings interface
    public:
        virtual bson_t toBSON() const;
        virtual void fromBSON(bson_iter_t& bson);
};

#endif // CHANNELSETTINGS_H
