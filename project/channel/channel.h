#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>

#include "channelsettings.h"
#include "noisechannelsettings.h"
#include "fmchannelsettings.h"
#include "tonechannelsettings.h"
#include "ssgchannelsettings.h"
#include "melodychannelsettings.h"
#include "rhythmchannelsettings.h"
#include "pcmchannelsettings.h"

class Channel
{
        friend class BSON;

    public:
        enum Type {
            TONE = 0,
            NOISE,
            FM,
            SSG,
            MELODY,
            RHYTHM,
            PCM
        };

        Channel();
        Channel(const QString& name);

        bool enabled() const;
        void setEnabled(const bool enabled);

        Type type() const;
        void setType(const Type type);

        QString name() const;
        void setName(const QString& name);

        ChannelSettings& settings();
        const ChannelSettings& settings() const;

        int pitchRange() const;
        void setPitchRange(const int semitones);

        static QString channelTypeToString(const Channel::Type type);
        static Channel::Type channelTypeFromString(const QString& str);

    private:
        bool _enabled;
        Type _type;
        QString _name;
        int _pitchRange;

        ChannelSettings* _settings;
};

#endif // CHANNEL_H
