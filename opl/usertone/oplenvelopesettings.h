#ifndef OPLENVELOPESETTINGS_H
#define OPLENVELOPESETTINGS_H

#include <QString>

#include "project/channel/settings.h"

class OPLEnvelopeSettings : public Settings
{
    public:
        enum EGType {
            Percussive = 0,
            Sustained
        };

        OPLEnvelopeSettings();

        EGType type() const;
        int ar() const;
        int dr() const;
        int sl() const;
        int rr() const;
        int tl() const;

        float arf() const;
        float drf() const;
        float slf() const;
        float rrf() const;
        float tlf() const;

        void setType(const EGType type);
        void setAr(const int ar);
        void setDr(const int dr);
        void setSl(const int sl);
        void setRr(const int rr);
        void setTl(const int tl);

        bool operator==(const OPLEnvelopeSettings& other) const;

    private:
        EGType _type;

        int _ar;
        int _dr;
        int _sl;
        int _rr;
        int _tl;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // OPLENVELOPESETTINGS_H
