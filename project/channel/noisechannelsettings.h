#ifndef NOISECHANNELSETTINGS_H
#define NOISECHANNELSETTINGS_H

#include <QString>

#include "settings.h"

class NoiseChannelSettings : public Settings
{
    public:
        enum NoiseType {
            Periodic,
            White
        };

        enum ShiftRate {
            FIVE_TWELVE,
            TEN_TWENTYFOUR,
            TWENTY_FORTYEIGHT
        };

        NoiseChannelSettings();

        NoiseType noiseType() const;
        void setNoiseType(const NoiseType type);

        ShiftRate shiftRate() const;
        void setShiftRate(const ShiftRate rate);

        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);

        bool operator==(const NoiseChannelSettings& other) const;
        bool operator!=(const NoiseChannelSettings& other) const;

    private:
        NoiseType _type;
        ShiftRate _rate;

        static QString noiseTypeToString(const NoiseType type);
        static QString shiftRateToString(const ShiftRate rate);

        static NoiseType noiseTypeFromString(const QString& str);
        static ShiftRate shiftRateFromString(const QString& str);
};

#endif // NOISECHANNELSETTINGS_H
