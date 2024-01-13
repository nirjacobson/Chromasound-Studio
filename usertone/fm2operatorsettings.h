#ifndef FM2OPERATORSETTINGS_H
#define FM2OPERATORSETTINGS_H

#include "project/channel/settings.h"
#include "fm2envelopesettings.h"

class FM2OperatorSettings : public Settings
{
    public:
        FM2OperatorSettings();

        FM2EnvelopeSettings& envelopeSettings();
        const FM2EnvelopeSettings& envelopeSettings() const;

        bool am() const;
        bool vib() const;
        bool ksr() const;
        int multi() const;
        int ksl() const;
        bool d() const;

        void setEnvelopeSettings(const FM2EnvelopeSettings& settings);
        void setAm(const bool am);
        void setVib(const bool vib);
        void setKsr(const bool ksr);
        void setMulti(const int multi);
        void setKsl(const int ksl);
        void setD(const bool d);

        bool operator==(const FM2OperatorSettings& other) const;

    private:
        FM2EnvelopeSettings _envelopeSettings;
        bool _am;
        bool _vib;
        bool _ksr;
        int _multi;
        int _ksl;
        bool _d;


        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // FM2OPERATORSETTINGS_H
