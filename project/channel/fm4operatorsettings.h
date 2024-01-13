#ifndef FM4OPERATORSETTINGS_H
#define FM4OPERATORSETTINGS_H

#include "settings.h"
#include "fm4envelopesettings.h"

class FM4OperatorSettings : public Settings
{
    public:
        FM4OperatorSettings();

        FM4EnvelopeSettings& envelopeSettings();
        const FM4EnvelopeSettings& envelopeSettings() const;
        int mul() const;
        int dt() const;
        int rs() const;
        bool am() const;

        void setEnvelopeSettings(const FM4EnvelopeSettings& settings);
        void setMul(const int mul);
        void setDt(const int dt);
        void setRs(const int rs);
        void setAm(const bool am);

        bool operator==(const FM4OperatorSettings& other) const;

    private:
        FM4EnvelopeSettings _envelopeSettings;
        int _mul;
        int _dt;
        int _rs;
        bool _am;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // FM4OPERATORSETTINGS_H
