#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>

#include "channelsettings.h"
#include "noisechannelsettings.h"

class Channel
{
        friend class BSON;

    public:
        enum Type {
            NONE = 0,
            TONE,
            NOISE,
        };

        Channel();
        Channel(const QString& name);

        bool enabled() const;
        void setEnabled(const bool enabled);

        Type type() const;
        void setType(const Type type);

        QString name() const;
        void setName(const QString& name);

        ChannelSettings& data();

    private:
        bool _enabled;
        Type _type;
        QString _name;

        ChannelSettings* _settings;

        static QString channelTypeToString(const Channel::Type type);
        static Channel::Type channelTypeFromString(const QString& str);
};

#endif // CHANNEL_H
