#ifndef MIDI_H
#define MIDI_H

#include "project/track.h"
#include "chunk/miditrack.h"
#include "midifile.h"

class MIDI
{
    public:
        MIDI();

        static QList<Track::Item*> toTrackItems(const MIDITrack& midiTrack, const int division);
        static void fromTrackItems(const Track& track, MIDITrack& midiTrack, const int division);
        static void fromTrack(const Track& track, MIDIFile& midiFile, const int division);

};

#endif // MIDI_H
