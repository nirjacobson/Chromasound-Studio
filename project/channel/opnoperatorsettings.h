#ifndef OPNOPERATORSETTINGS_H
#define OPNOPERATORSETTINGS_H

#include "settings.h"
#include "opnenvelopesettings.h"

class OPNOperatorSettings : public Settings
{
    public:
        OPNOperatorSettings();

        OPNEnvelopeSettings& envelopeSettings();
        const OPNEnvelopeSettings& envelopeSettings() const;
        int mul() const;
        int dt() const;
        int rs() const;
        bool am() const;

        void setEnvelopeSettings(const OPNEnvelopeSettings& settings);
        void setMul(const int mul);
        void setDt(const int dt);
        void setRs(const int rs);
        void setAm(const bool am);

        bool operator==(const OPNOperatorSettings& other) const;

    private:
        OPNEnvelopeSettings _envelopeSettings;
        int _mul;
        int _dt;
        int _rs;
        bool _am;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // OPNOPERATORSETTINGS_H
