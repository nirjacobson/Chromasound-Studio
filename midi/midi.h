#ifndef MIDI_H
#define MIDI_H

#include "../project/track.h"
#include "chunk/miditrack.h"

class MIDI
{
public:
    MIDI();

    static QList<Track::Item*> toTrackItems(const MIDITrack& midiTrack, const int division);
};

#endif // MIDI_H
