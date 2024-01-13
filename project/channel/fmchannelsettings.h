#ifndef FMCHANNELSETTINGS_H
#define FMCHANNELSETTINGS_H

#include <iterator>

#include "fm4operatorsettings.h"
#include "channelsettings.h"
#include "algorithmsettings.h"
#include "lfosettings.h"

class FMChannelSettings : public ChannelSettings
{
    public:
        FMChannelSettings();

        typedef FM4OperatorSettings FourOperatorSettings[4];

        FourOperatorSettings& operators();
        const FourOperatorSettings& operators() const;

        AlgorithmSettings& algorithm();
        const AlgorithmSettings& algorithm() const;

        LFOSettings& lfo();
        const LFOSettings& lfo() const;

        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);

        bool operator==(const FMChannelSettings& other) const;

    private:
        FM4OperatorSettings _operators[4];
        AlgorithmSettings _algorithm;
        LFOSettings _lfo;

        // ChannelSettings interface
    public:
        QString type() const;

        // ChannelSettings interface
    public:
        ChannelSettings* copy() const;
};

#endif // FMCHANNELSETTINGS_H
