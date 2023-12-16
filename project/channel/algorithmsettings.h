#ifndef ALGORITHMSETTINGS_H
#define ALGORITHMSETTINGS_H

#include "settings.h"

class AlgorithmSettings : public Settings
{
    public:
        AlgorithmSettings();

        int algorithm() const;
        int feedback() const;

        void setAlgorithm(const int algorithm);
        void setFeedback(const int feedback);

        bool operator==(const AlgorithmSettings& other) const;

    private:
        int _algorithm;
        int _feedback;

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);
};

#endif // ALGORITHMSETTINGS_H
