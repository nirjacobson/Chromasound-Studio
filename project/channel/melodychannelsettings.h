#ifndef MELODYCHANNELSETTINGS_H
#define MELODYCHANNELSETTINGS_H

#include "channelsettings.h"

class MelodyChannelSettings : public ChannelSettings
{
    public:
        enum Instrument {
            User = 0,
            Violin,
            Guitar,
            Piano,
            Flute,
            Clarinet,
            Oboe,
            Trumpet,
            Organ,
            Horn,
            Synthesizer,
            Harpsichord,
            Vibraphone,
            SynthesizerBass,
            AcousticBass,
            ElectricGuitar
        };

        MelodyChannelSettings();

        Instrument instrument() const;
        void setInstrument(const Instrument instr);

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
};

#endif // MELODYCHANNELSETTINGS_H
