#ifndef RHYTHMCHANNELSETTINGS_H
#define RHYTHMCHANNELSETTINGS_H

#include "channelsettings.h"

class RhythmChannelSettings : public ChannelSettings
{
    public:
        enum Instrument {
            BassDrum,
            SnareDrum,
            TomTom,
            TopCymbal,
            HighHat
        };

        RhythmChannelSettings();

        Instrument instrument() const;
        void setInstrument(const Instrument instr);

        bool operator==(const RhythmChannelSettings& o) const;

    private:
        Instrument _instrument;

        static QString instrumentToString(const Instrument instr);
        static Instrument instrumentFromString(const QString& str);

        // Settings interface
    public:
        bson_t toBSON() const;
        void fromBSON(bson_iter_t& bson);

        // ChannelSettings interface
    public:
        QString type() const;
        ChannelSettings* copy() const;

        // ChannelSettings interface
    public:
        bool supportsPitchChanges();
};

#endif // RHYTHMCHANNELSETTINGS_H
