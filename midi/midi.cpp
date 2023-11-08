#include "midi.h"

MIDI::MIDI()
{

}

QList<Track::Item*> MIDI::toTrackItems(const MIDITrack& midiTrack, const int division)
{
    QMap<int, Note> notes;
    QMap<int, quint32> starts;

    QList<Track::Item*> items;

    quint32 runningTime = 0;
    for (int i = 0; i < midiTrack.events(); i++) {
        const MIDITrackEvent& trackEvent = midiTrack.event(i);
        runningTime += trackEvent.deltaTime();

        try {
            const MIDIEvent& midiEvent = dynamic_cast<const MIDIEvent&>(trackEvent.event());

            if (midiEvent.isKeyOn()) {
                if (!notes.contains(midiEvent.data1())) {
                    notes[midiEvent.data1()].setKey(midiEvent.data1());
                    notes[midiEvent.data1()].setVelocity(midiEvent.data2()/127.0f * 100.0f);
                    starts[midiEvent.data1()] = runningTime;
                }
            } else if (midiEvent.isKeyOff()) {
                if (notes.contains(midiEvent.data1())) {
                    notes[midiEvent.data1()].setDuration((float)(runningTime - starts[midiEvent.data1()]) / division);

                    Track::Item i;
                    i._time = (float)starts[midiEvent.data1()] / division;
                    i._note = notes[midiEvent.data1()];

                    items.append(new Track::Item(i));

                    notes.remove(midiEvent.data1());
                    starts.remove(midiEvent.data1());
                }
            }
        } catch(std::exception e) {

        }
    }

    std::sort(items.begin(), items.end(), [](const Track::Item* a, const Track::Item* b){ return a->time() <= b->time(); });

    return items;
}


