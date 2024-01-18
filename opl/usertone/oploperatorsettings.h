#ifndef OPLOPERATORSETTINGS_H
#define OPLOPERATORSETTINGS_H

#include "project/channel/settings.h"
#include "oplenvelopesettings.h"

class OPLOperatorSettings : public Settings
{
    public:
        OPLOperatorSettings();

        OPLEnvelopeSettings& envelopeSettings();
        const OPLEnvelopeSettings& envelopeSettings() const;

        bool am() const;
        bool vib() const;
        bool ksr() const;
        int multi() const;
        int ksl() const;
        bool d() const;

        void setEnvelopeSettings(const OPLEnvelopeSettings& settings);
        void setAm(const bool am);
        void setVib(const bool vib);
        void setKsr(const bool ksr);
        void setMulti(const int multi);
        void setKsl(const int ksl);
        void setD(const bool d);

        bool operator==(const OPLOperatorSettings& other) const;

    private:
        OPLEnvelopeSettings _envelopeSettings;
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

#endif // OPLOPERATORSETTINGS_H
