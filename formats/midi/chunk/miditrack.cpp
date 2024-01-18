#include "miditrack.h"

MIDITrack::MIDITrack(const QString& chunkType, quint32 length, const std::function<qint64 ()>& posFunc)
    : MIDIChunk(chunkType, length)
    , _pos(posFunc)
{

}

MIDITrack::~MIDITrack()
{
    for (const MIDITrackEvent* event : _events) {
        delete event;
    }
}

MIDITrack& MIDITrack::operator<<(QDataStream& stream)
{
    qint64 startingPos = _pos();

    MIDITrackEvent* lastMIDITrackEvent = nullptr;
    while ((_pos() - startingPos) < length()) {
        MIDITrackEvent* trackEvent;
        if (lastMIDITrackEvent) {
            const MIDIEvent& midiEvent = dynamic_cast<const MIDIEvent&>(lastMIDITrackEvent->event());
            trackEvent = new MIDITrackEvent(midiEvent.status());
        } else {
            trackEvent = new MIDITrackEvent;
        }
        *trackEvent << stream;

        _events.append(trackEvent);

        try {
            const MIDIEvent& result = dynamic_cast<const MIDIEvent&>(trackEvent->event());
            lastMIDITrackEvent = trackEvent;
        } catch (std::exception e) { }
    }

    return *this;
}

int MIDITrack::events() const
{
    return _events.size();
}

const MIDITrackEvent& MIDITrack::event(const int idx) const
{
    return *_events[idx];
}

QByteArray MIDITrack::encode() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    QByteArray chunkData;
    for (const MIDITrackEvent* event : _events) {
        chunkData.append(event->encode());
    }

    stream << 'M';
    stream << 'T';
    stream << 'r';
    stream << 'k';
    stream << (quint32)chunkData.size();
    stream.writeRawData(chunkData.constData(), chunkData.size());
    return data;
}
