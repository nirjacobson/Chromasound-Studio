#ifndef SSGENVELOPESETTINGS_H
#define SSGENVELOPESETTINGS_H

#include "project/channel/settings.h"

class SSGEnvelopeSettings : public Settings
{
    public:
        SSGEnvelopeSettings();

        bool cont() const;
        bool att() const;
        bool alt() const;
        bool hold() const;

        void setCont(bool enable);
        void setAtt(bool enable);
        void setAlt(bool enable);
        void setHold(bool enable);

        bool operator==(const SSGEnvelopeSettings& other) const;

    private:
        bool _cont;
        bool _att;
        bool _alt;
        bool _hold;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // SSGENVELOPESETTINGS_H
