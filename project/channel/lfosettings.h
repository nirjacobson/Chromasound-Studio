#ifndef LFOSETTINGS_H
#define LFOSETTINGS_H

#include "settings.h"

class LFOSettings : public Settings
{
    public:
        LFOSettings();

        int ams() const;
        int fms() const;

        void setAMS(const int ams);
        void setFMS(const int fms);

        bool operator==(const LFOSettings& other) const;

    private:
        int _ams;
        int _fms;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // LFOSETTINGS_H
