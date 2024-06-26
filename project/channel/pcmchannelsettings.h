#ifndef PCMCHANNELSETTINGS_H
#define PCMCHANNELSETTINGS_H

#include <QString>

#include "channelsettings.h"

class PCMChannelSettings : public ChannelSettings
{
    public:
        PCMChannelSettings();

        const QString& path() const;
        void setPath(const QString& path);

    private:
        QString _filePath;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);

        // ChannelSettings interface
    public:
        QString type() const;

        // ChannelSettings interface
    public:
        ChannelSettings* copy() const;
};

#endif // PCMCHANNELSETTINGS_H
