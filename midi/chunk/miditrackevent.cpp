#include "miditrackevent.h"

MIDITrackEvent::MIDITrackEvent(const quint8 lastMIDIStatus)
    : _lastMIDIStatus(lastMIDIStatus)
{

}

MIDITrackEvent::MIDITrackEvent()
{

}

MIDITrackEvent::~MIDITrackEvent()
{
    delete _event;
}

MIDITrackEvent& MIDITrackEvent::operator<<(QDataStream& stream)
{
    quint8 datum;

    _deltaTime = Util::getVariableLengthQuantity(stream);

    stream >> datum;

    if (datum == 0xF0 || datum == 0xF7) {
        SysexEvent* sysEvent = new SysexEvent(datum);
        *sysEvent << stream;
        _event = sysEvent;
    } else if (datum == 0xFF) {
        quint8 type;
        stream >> type;
        MetaEvent* metaEvent = new MetaEvent(type);
        *metaEvent << stream;
        _event = metaEvent;
    } else if (isMIDIStatus(datum)) {
        MIDIEvent* midiEvent = new MIDIEvent(datum);
        *midiEvent << stream;
        _event = midiEvent;
    } else {
        MIDIEvent* midiEvent = new MIDIEvent(_lastMIDIStatus, datum);
        *midiEvent << stream;
        _event = midiEvent;
    }

    return *this;
}

const Event* MIDITrackEvent::event() const
{
    return _event;
}

bool MIDITrackEvent::isMIDIStatus(const quint8 datum)
{
    quint8 highNibble = datum >> 4;
    return highNibble >= 0x8 && highNibble <= 0xE;
}
