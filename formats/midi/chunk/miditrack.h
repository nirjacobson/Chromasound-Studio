#ifndef MIDITRACK_H
#define MIDITRACK_H

#include <QList>
#include <QIODevice>

#include "midichunk.h"
#include "miditrackevent.h"

class MIDITrack : public MIDIChunk
{
        friend class MIDI;

    public:
        MIDITrack(const QString& chunkType, quint32 length, const std::function<qint64 ()>& posFunc);
        ~MIDITrack();

        MIDITrack& operator<<(QDataStream& stream);

        int events() const;
        const MIDITrackEvent& event(const int idx) const;

        QString name(const int channel = 0) const;

        QByteArray encode() const;

    private:
        QList<MIDITrackEvent*> _events;
        std::function<qint64(void)> _pos;
};

#endif // MIDITRACK_H
