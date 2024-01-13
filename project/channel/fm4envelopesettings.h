#ifndef FM4ENVELOPESETTINGS_H
#define FM4ENVELOPESETTINGS_H

#include "settings.h"

class FM4EnvelopeSettings : public Settings
{
    public:
        FM4EnvelopeSettings();

        int ar() const;
        int t1l() const;
        int d1r() const;
        int t2l() const;
        int d2r() const;
        int rr() const;

        float arf() const;
        float t1lf() const;
        float d1rf() const;
        float t2lf() const;
        float d2rf() const;
        float rrf() const;

        void setAr(const int ar);
        void setT1l(const int t1l);
        void setD1r(const int d1r);
        void setT2l(const int t2l);
        void setD2r(const int d2r);
        void setRr(const int rr);

        bool operator==(const FM4EnvelopeSettings& other) const;

    private:
        int _ar;
        int _t1l;
        int _d1r;
        int _t2l;
        int _d2r;
        int _rr;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // FM4ENVELOPESETTINGS_H
