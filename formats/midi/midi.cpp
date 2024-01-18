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

    std::sort(items.begin(), items.end(), [](const Track::Item* a, const Track::Item* b) {
        return a->time() < b->time();
    });

    return items;
}

void MIDI::fromTrackItems(const Track& track, MIDITrack& midiTrack, const int division)
{
    QList<Track::Item*> itemsCopy = track.items();

    std::sort(itemsCopy.begin(), itemsCopy.end(), [](const Track::Item* a, const Track::Item* b) {
        return a->time() < b->time();
    });

    struct KeyEvent {
        bool on;
        int key;
        int velocity;
        float time;
    };

    QList<KeyEvent> keyEvents;

    for (const Track::Item* item : itemsCopy) {
        struct KeyEvent keyEvent;

        keyEvent.on = true;
        keyEvent.key = item->note().key();
        keyEvent.velocity = item->note().velocity() / 100.0f * 127;
        keyEvent.time = item->time();

        keyEvents.append(keyEvent);

        keyEvent.on = false;
        keyEvent.velocity = 0;
        keyEvent.time = item->time() + item->note().duration();

        keyEvents.append(keyEvent);
    }

    std::sort(keyEvents.begin(), keyEvents.end(), [](const KeyEvent& a, const KeyEvent& b) {
        return a.time < b.time;
    });

    float lastTime = 0.0f;
    for (const KeyEvent& keyEvent : keyEvents) {
        MIDITrackEvent* trackEvent = new MIDITrackEvent;

        trackEvent->_deltaTime = (keyEvent.time - lastTime) * division;
        lastTime = keyEvent.time;

        MIDIEvent* midiEvent = new MIDIEvent(keyEvent.on ? 0x90 : 0x80);
        midiEvent->_data1 = keyEvent.key;
        midiEvent->_data2 = keyEvent.velocity;

        trackEvent->_event = midiEvent;

        midiTrack._events.append(trackEvent);
    }

    MIDITrackEvent* trackEvent = new MIDITrackEvent;
    trackEvent->_deltaTime = 0;

    MetaEvent* metaEvent = new MetaEvent(0x2F);
    metaEvent->_length = 0;

    trackEvent->_event = metaEvent;

    midiTrack._events.append(trackEvent);
}

void MIDI::fromTrack(const Track& track, MIDIFile& midiFile, const int division)
{
    MIDIHeader* header = new MIDIHeader("MThd", 6);
    header->_format = 1;
    header->_ntrks = 1;
    header->_division = division;

    MIDITrack* midiTrack = new MIDITrack("MTrk", 0, []() {
        return 0;
    });
    fromTrackItems(track, *midiTrack, division);

    midiFile._chunks.append(header);
    midiFile._chunks.append(midiTrack);
}


