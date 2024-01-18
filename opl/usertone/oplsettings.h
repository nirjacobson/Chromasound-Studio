#ifndef OPLSETTINGS_H
#define OPLSETTINGS_H

#include "project/channel/settings.h"
#include "oploperatorsettings.h"

class OPLSettings : public Settings
{
    public:
        OPLSettings();

        typedef OPLOperatorSettings TwoOperatorSettings[2];

        TwoOperatorSettings& operators();
        const TwoOperatorSettings& operators() const;

        int tl() const;
        void setTl(const int tl);

        int fb() const;
        void setFb(const int fb);

        bool operator==(const OPLSettings& other) const;

    private:
        OPLOperatorSettings _operators[2];
        int _tl;
        int _fb;


        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // OPLSETTINGS_H
