#ifndef OPERATORSETTINGS_H
#define OPERATORSETTINGS_H

#include "settings.h"
#include "envelopesettings.h"

class OperatorSettings : public Settings
{
    public:
        OperatorSettings();

        EnvelopeSettings& envelopeSettings();
        const EnvelopeSettings& envelopeSettings() const;
        int mul() const;
        int dt() const;
        int rs() const;
        bool am() const;

        void setEnvelopeSettings(const EnvelopeSettings& settings);
        void setMul(const int mul);
        void setDt(const int dt);
        void setRs(const int rs);
        void setAm(const bool am);

        bool operator==(const OperatorSettings& other) const;

    private:
        EnvelopeSettings _envelopeSettings;
        int _mul;
        int _dt;
        int _rs;
        bool _am;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // OPERATORSETTINGS_H
