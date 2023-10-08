#ifndef FMCHANNELSETTINGS_H
#define FMCHANNELSETTINGS_H

#include <iterator>

#include "operatorsettings.h"
#include "settings.h"

class FMChannelSettings : public Settings
{
    public:
        FMChannelSettings();

        typedef OperatorSettings FourOperatorSettings[4];

        FourOperatorSettings& operators();
        const FourOperatorSettings& operators() const;
        int algorithm() const;
        int feedback() const;

        void setAlgorithm(const int alg);
        void setFeedback(const int fb);

        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);

        bool operator==(const FMChannelSettings& other) const;

    private:
        OperatorSettings _operators[4];
        int _algorithm;
        int _feedback;
};

#endif // FMCHANNELSETTINGS_H
