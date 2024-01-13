#ifndef FM2SETTINGS_H
#define FM2SETTINGS_H

#include "project/channel/settings.h"
#include "fm2operatorsettings.h"

class FM2Settings : public Settings
{
    public:
        FM2Settings();

        typedef FM2OperatorSettings TwoOperatorSettings[2];

        TwoOperatorSettings& operators();
        const TwoOperatorSettings& operators() const;

        int tl() const;
        void setTl(const int tl);

        int fb() const;
        void setFb(const int fb);

        bool operator==(const FM2Settings& other) const;

    private:
        FM2OperatorSettings _operators[2];
        int _tl;
        int _fb;


        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // FM2SETTINGS_H
