#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>

#include "settings.h"
#include "noisechannelsettings.h"
#include "fmchannelsettings.h"

class Channel
{
        friend class BSON;

    public:
        enum Type {
            NONE = 0,
            TONE,
            NOISE,
            FM,
        };

        Channel();
        Channel(const QString& name);

        bool enabled() const;
        void setEnabled(const bool enabled);

        Type type() const;
        void setType(const Type type);

        QString name() const;
        void setName(const QString& name);

        Settings& settings();

    private:
        bool _enabled;
        Type _type;
        QString _name;

        Settings* _settings;

        static QString channelTypeToString(const Channel::Type type);
        static Channel::Type channelTypeFromString(const QString& str);
};

#endif // CHANNEL_H
