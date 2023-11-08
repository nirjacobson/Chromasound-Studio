#ifndef MIDITRACK_H
#define MIDITRACK_H

#include <QList>

#include "midichunk.h"
#include "miditrackevent.h"

class MIDITrack : public MIDIChunk
{
public:
    MIDITrack(quint32 offset, const QString& chunkType, quint32 length, const std::function<qint64 ()>& posFunc);
    ~MIDITrack();

    MIDITrack& operator<<(QDataStream& stream);

    int events() const;
    const MIDITrackEvent& event(const int idx) const;

private:
    QList<MIDITrackEvent*> _events;
    std::function<qint64(void)> _pos;
};

#endif // MIDITRACK_H
